; VflatD_Query
;
; mov   dx, VflatD_Query          ; Function number
; call  VFlatDApiProcAdr
; jz    errorhandler
; mov   VerNum, eax
; mov   FrameBufSelector, ebx
; mov   FrameBufSize, ecx
; mov   FrameBufBase, edx
;
; Retrieves the VFLATD version number and other information. Uses Flags.
;
; Returns the following information and clears the carry flag if successful; otherwise, sets the carry flag.
; EAX = VFLATD version
; EBX = GDT selector to the frame buffer
; ECX = Size of frame buffer
; EDX = Linear base of frame buffer. (if linear mode)
VflatD_Query EQU 0

; VflatD_Create_Virtual_Frame_Buffer
;
; mov    dl, VflatD_Create_Virtual_Frame_Buffer  ; Function number
; mov    dh, Flags                        ; Must be zero
; mov    eax, FrameBufSize                ; Size of frame buffer in bytes
; mov    ebx, BankSize                    ; Size of bank, in bytes
; mov    esi, BankLocation                ; Location of bank
; push   ds
; pop    es
; lea    di, BankSwitchProc               ; Bank switch code
; movzx  edi, di
; mov    cx, BankSwitchProcLength         ; Length of bank switch code
; call   VFlatDApiProcAdr
; jc     errorhandler
; mov    VideoSelector, ax
; mov    VideoAddress, edx
;
; Registers a display driver's bank-switch function and retrieves a selector for a logical frame
; buffer. Uses Flags.
; * Returns a selector for the logical frame buffer in AX, returns a linear address for that frame
;   buffer in EDX, and clears the carry flag if successful; otherwise, sets the carry flag.
;
; Flags
;   Options flags. Currently, no flags are defined; this parameter must be zero.
; FrameBufSize
;   Size of frame buffer, in bytes.
; BankSize
;   Size of bank, in bytes.
; BankLocation
;   Location of bank, for example, A0000h.
; BankSwitchProc
;   Address of display driver's bank switch procedure.
; BankSwitchProcLength
;   Length of display driver's bank-switch procedure.
; The bank-switch procedure must be written using 32-bit code and must be a maximum of 100 bytes in
; length. When the procedure is called, EAX will identify the new bank to be swapped into the
; physical frame buffer. The procedure must save all registers except EAX, EDX, and Flags, and it
; must not have a ret instruction (since the code is copied inline).
VflatD_Create_Virtual_Frame_Buffer EQU 3

; VflatD_Create_Physical_Frame_Buffer
;
; mov   dl, VflatD_Create_Physical_Frame_Buffer  ; Function number
; mov   dh, Flags                         ; Must be zero
; mov   eax, FrameBufBase                 ; Physical base of frame buffer
; mov   ecx, FrameBufSize                 ; Size of frame buffer, in bytes
; call  VFlatDApiProcAdr
; jc    errorhandler
; mov   VideoSelector, ax
; mov    VideoAddress, edx
;
; Retrieves a GDT selector to a flat video buffer. Uses Flags.
; * Returns a selector for the logical frame buffer in AX, returns a linear address for that frame
;   buffer in EDX, and clears the carry flag if successful; otherwise, sets the carry flag.
;
; Flags
;   Options flags. Currently, no flags are defined; this parameter must be zero.
; FrameBufBase
;   Physical base of frame buffer. The physical base is the address of the frame buffer in physical memory.
; FrameBufSize
;   Size of frame buffer, in bytes.
VflatD_Create_Physical_Frame_Buffer EQU 4
