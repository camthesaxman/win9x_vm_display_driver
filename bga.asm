; bga.asm - Code for handling the Bochs Graphics Adapter
;
; Many virtualization programs including Bochs, VirtualBox and QEMU provide this
; VESA BIOS extension as a convenient way to configure VESA modes.
; Due to the large amount of port I/O and low-level calls, this module is
; implemented in assembly language.

.MODEL MEDIUM
.386


INCLUDE vflatd.inc


;-------------------------------------------------------------------------------
; Imports
;-------------------------------------------------------------------------------
EXTERN _dwFrameBufSize    : DWORD
EXTERN _dwVideoMemSize    : DWORD
EXTERN _wXResolution      : WORD
EXTERN _wYResolution      : WORD
EXTERN _wBPP              : WORD
EXTERN _wFrameBufSelector : WORD


;-------------------------------------------------------------------------------
; Defines
;-------------------------------------------------------------------------------
GET_VM_HANDLE              EQU 1683h
GET_DEVICE_API_ENTRY_POINT EQU 1684h
STOP_IO_TRAP               EQU 4000h
START_IO_TRAP              EQU 4007h
VFLATD_VXD                 EQU 11Fh

; Bochs Graphics Adapter ports
VBE_DISPI_IOPORT_INDEX EQU 01CEh
VBE_DISPI_IOPORT_DATA  EQU 01CFh
; Bochs Graphics Adapter register indexes
VBE_DISPI_INDEX_ID     EQU 0000h
VBE_DISPI_INDEX_XRES   EQU 0001h
VBE_DISPI_INDEX_YRES   EQU 0002h
VBE_DISPI_INDEX_BPP    EQU 0003h
VBE_DISPI_INDEX_ENABLE EQU 0004h
VBE_DISPI_INDEX_BANK   EQU 0005h


;-------------------------------------------------------------------------------
; Variables
;-------------------------------------------------------------------------------
_DATA_BSS SEGMENT PUBLIC 'FAR_DATA'
    _vflatdEntry     DD 0
    _dwFrameBufAddr  DD 0
    PUBLIC _vflatdEntry
    PUBLIC _dwFrameBufAddr
_DATA_BSS ENDS
ASSUME ds:_DATA_BSS


;-------------------------------------------------------------------------------
; Code (16-bit)
;-------------------------------------------------------------------------------
_INIT SEGMENT WORD PUBLIC USE16 'CODE'

; Enables the Bochs Graphics Adapter hardware
; Returns:
;    1 on success, 0 on failure
bga_phys_enable_ PROC FAR PUBLIC
    call bga_phys_reset
    test ax, ax
    jz done  ; failed
    call bga_vflatd_init
  done:
    retf
bga_phys_enable_ ENDP

; Resets the Bochs Graphics Adapter hardware
; Returns:
;    1 on success, 0 on failure
bga_phys_reset PROC NEAR
    cli  ; disable interrupts so we don't get preempted while manipulating ports

    ; check ID of adapter (must be between 0xB0C0 and 0xB0C5)
    mov ax, VBE_DISPI_INDEX_ID
    call bga_read_reg
    cmp ax, 0B0C0h
    jb failure
    cmp ax, 0B0C5h
    ja failure

    ; TODO: find some way to determine actual memory size
    mov _dwVideoMemSize, (32 * 1024 * 1024)

    ; disable VBE extensions
    mov ax, VBE_DISPI_INDEX_ENABLE
    xor bx, bx
    call bga_write_reg
    ; set X resolution
    mov ax, VBE_DISPI_INDEX_XRES
    mov bx, _wXResolution
    call bga_write_reg
    ; set Y resolution
    mov ax, VBE_DISPI_INDEX_YRES
    mov bx, _wYResolution
    call bga_write_reg
    ; set bits per pixel
    mov ax, VBE_DISPI_INDEX_BPP
    mov bx, _wBPP
    call bga_write_reg
    ; re-enable VBE extensions
    mov ax, VBE_DISPI_INDEX_ENABLE
    mov bx, 1
    call bga_write_reg

  success:
    mov ax, 1
    jmp done
  failure:
    xor ax, ax
  done:

    sti  ; re-enable interrupts

    ret
bga_phys_reset ENDP

; Initializes the VflatD device, which simulates flat memory access to the framebuffer
; Returns:
;    1 on success, 0 on failure
bga_vflatd_init PROC NEAR
    push bp
    push si
    push di

    mov eax, _vflatdEntry
    test eax, eax
    jnz success  ; already initialized

    ; Get VFLATD entry point
    mov ax, GET_DEVICE_API_ENTRY_POINT
    mov bx, VFLATD_VXD
    int 2Fh
    mov WORD PTR _vflatdEntry, di
    mov WORD PTR _vflatdEntry+2, es

    ; Call VflatD_Query to register driver
    mov dx, VflatD_Query
    call DWORD PTR _vflatdEntry

    ; Create virtual framebuffer
    xor edi, edi
    lea di, OFFSET BankSwitchRoutine  ; di = bank switch code offset
    mov ax, _INIT32
    mov es, ax                 ; es = bank switch code segment
    mov ecx, BankSwitchRoutineSize
    mov eax, _dwFrameBufSize    ; eax = frame buffer size
    mov ebx, 10000h            ; ebx = bank size
    mov esi, 0A0000h           ; esi = physical memory address of bank
    mov dx, VflatD_Create_Virtual_Frame_Buffer
    call DWORD PTR _vflatdEntry
    jc failure
    mov _wFrameBufSelector, ax
    mov _dwFrameBufAddr, edx

  success:
    mov ax, 1
    jmp done
  failure:
    xor ax, ax
  done:

    pop di
    pop si
    pop bp
    ret
bga_vflatd_init ENDP

; Reads a Bochs Graphics Adapter register
; Parameters:
;    ax = register index
; Returns:
;    value of register
bga_read_reg PROC NEAR
    ; select register to read
    mov dx, VBE_DISPI_IOPORT_INDEX
    out dx, ax
    ; read value
    mov dx, VBE_DISPI_IOPORT_DATA
    in ax, dx
    ret
bga_read_reg ENDP

; Writes a Bochs Graphics Adapter register
; Parameters:
;    ax = register index
;    bx = value
bga_write_reg PROC NEAR
    ; select register to write
    mov dx, VBE_DISPI_IOPORT_INDEX
    out dx, ax
    ; write value
    mov ax, bx
    mov dx, VBE_DISPI_IOPORT_DATA
    out dx, ax
    ret
bga_write_reg ENDP

_INIT ENDS


;-------------------------------------------------------------------------------
; Code (32-bit)
;-------------------------------------------------------------------------------
_INIT32 SEGMENT PUBLIC USE32 'CODE'

; Framebuffer bank switching routine
; When calling VflatD_Create_Virtual_Frame_Buffer, this routine is copied
; inline and is run in 32-bit ring 0 as a page fault handler by VFLATD. Hence
; there is no "ret" at the end. All registers must be saved except for EAX, EDX,
; and Flags.
; Parameters:
;    eax = bank to be swapped in
PUBLIC BankSwitchRoutine
BankSwitchRoutine:
    shl eax, 16    ; save bank number
    ; Select register index
    mov dx, 01CEh  ; VBE_DISPI_IOPORT_INDEX (0x01CE)
    mov ax, 0005h  ; VBE_DISPI_INDEX_BANK (0x005)
    out dx, ax
    ; Write bank number
    shr eax, 16
    inc edx        ; VBE_DISPI_IOPORT_DATA (0x01CF)
    out dx, ax
BankSwitchRoutineSize = $ - BankSwitchRoutine

_INIT32 ENDS


END
