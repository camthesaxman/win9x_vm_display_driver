; pci.asm - probing and reading from PCI configuration

.MODEL MEDIUM
.386


;-------------------------------------------------------------------------------
; Code
;-------------------------------------------------------------------------------
_INIT SEGMENT WORD PUBLIC USE16 'CODE'

; Reads a 32-bit value from the PCI configuration space
; Parameters:
;	 al = bus number
;	 ah = device number
;	 bl = function number
;	 bh = offset
; Returns:
;    32-bit value of PCI config register
pci_config_read_dword PROC NEAR PUBLIC
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
	; write out the address
	mov eax, ecx
	mov dx, 0CF8h
	out dx, eax
	; read in the data
	mov dx, 0CFCh
	in eax, dx
	ret
pci_config_read_dword ENDP

; Finds a PCI device using a brute force scan
; Parameters:
;	 eax = vendor (lower 16 bits), device (upper 16 bits)
; Returns:
;    on success: al = bus number, ah = device number
;    on error: ax = -1
pci_find_device PROC NEAR PUBLIC
	push esi
	mov esi, eax   ; save param

	mov al, 0  ; al = bus
  next_bus:
	mov ah, 0  ; ah = device
  next_device:
 
	; check vendor and device
	push ax
	mov bx, 0
	call pci_config_read_dword
	cmp eax, esi  ; is this it?
	pop ax

	; if so, return it
	je done

	inc ah
	cmp ah, 32
	jb next_device
	inc al
	jnz next_bus

	mov ax, -1

  done:
	pop esi
	ret
pci_find_device ENDP

_INIT ENDS


END
