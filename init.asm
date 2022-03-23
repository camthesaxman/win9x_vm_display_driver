.MODEL LARGE
.386


;-------------------------------------------------------------------------------
; Imports
;-------------------------------------------------------------------------------
EXTERN display_driver_init : FAR
EXTERN GlobalSmartPageLock : FAR


;-------------------------------------------------------------------------------
; Code (16-bit)
;-------------------------------------------------------------------------------
_INIT SEGMENT WORD PUBLIC USE16 'CODE'

; Driver entry point. It is called at boot time when the driver is loaded.
; Parameters:
;   cx = size of heap
;   di = module handle
;   ds = automatic data segment
;   es:si = address of command line
; Return:
;   ax = 1 on success
DriverInit PROC FAR PUBLIC
	push bp
	push si
	push di

	push _INIT
	call GlobalSmartPageLock

	call display_driver_init

	pop di
	pop si
	pop bp
	ret
DriverInit ENDP

; Watcom object files expect this symbol to be defined
PUBLIC  _big_code_
_big_code_ LABEL FAR

_INIT ENDS


; Set DriverInit as the DLL entry point
END DriverInit
