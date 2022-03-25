; pci.asm - probing and reading from PCI configuration

.MODEL MEDIUM
.386


;-------------------------------------------------------------------------------
; Code
;-------------------------------------------------------------------------------
_INIT SEGMENT WORD PUBLIC USE16 'CODE'

; Combines a PCI bus number, device number, function number, and register
; offset to form the IO address of the register
; Parameters:
;	 al = bus number
;	 ah = device number
;	 bl = function number
;	 bh = offset
; Returns:
;       eax = IO address
pci_make_io_address PROC NEAR PUBLIC
	; construct address in ecx
	; The PCI address is as follows:
	; | Bit 31 | Bits 30-24 | Bits 23-16 | Bits 15-11 | Bits 10-8 | Bits 7-0   |
	; | enable | reserved   | bus number | device num | func num  | reg offset |
	movzx ecx, al  ; bus
	shl ecx, 5
	or cl, ah      ; device
	shl ecx, 3
	or cl, bl      ; func
	shl ecx, 8
	or cl, bh      ; offset
	or ecx, 80000000h  ; enable bit
        mov eax, ecx
        ret
pci_make_io_address ENDP

; Reads a 32-bit value from the PCI configuration space
; Parameters:
;    eax = PCI address
; Returns:
;    32-bit value of PCI config register
pci_config_read_dword PROC NEAR PUBLIC
	; write out the address
	mov dx, 0CF8h
	out dx, eax
	; read in the data
	mov dx, 0CFCh
	in eax, dx
	ret
pci_config_read_dword ENDP

; Writes a 32-bit value to the PCI configuration space
; Parameters:
;    eax = PCI address
;    ebx = value to write
pci_config_write_dword PROC NEAR PUBLIC
	; write out the address
	mov dx, 0CF8h
	out dx, eax
	; write the data
        mov eax, ebx
	mov dx, 0CFCh
        out dx, eax
	ret
pci_config_write_dword ENDP

; Finds a PCI device using a brute force scan
; Parameters:
;	 eax = vendor (lower 16 bits), device (upper 16 bits)
; Returns:
;    on success: eax = base address of configuration space
;    on error: ax = -1
pci_get_device_config_base PROC NEAR PUBLIC
	push esi
	mov esi, eax   ; save param in esi

	mov al, 0  ; al = bus
  next_bus:
	mov ah, 0  ; ah = device
  next_device:
        push ax
 
	; get IO address
        xor bx, bx
        call pci_make_io_address
        mov ecx, eax
        ; read vendor and device IDs
        call pci_config_read_dword
	cmp eax, esi  ; is this it?

	pop ax

	; if it matches, return the address
	je done

	inc ah
	cmp ah, 32
	jb next_device
	inc al
	jnz next_bus

	mov ecx, -1

  done:
        mov eax, ecx
	pop esi
	ret
pci_get_device_config_base ENDP

_INIT ENDS


END
