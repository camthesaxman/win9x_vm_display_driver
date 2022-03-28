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

;========================================================================
;==     Name:           I4M,U4M                                        ==
;==     Operation:      integer four byte multiply                     ==
;==     Inputs:         DX;AX   integer M1                             ==
;==                     CX;BX   integer M2                             ==
;==     Outputs:        DX;AX   product                                ==
;==     Volatile:       CX, BX destroyed                               ==
;========================================================================
__U4M PROC PUBLIC
    xchg    ax,bx      ; swap low(M1) and low(M2)
    push    ax         ; save low(M2)
    xchg    ax,dx      ; exchange low(M2) and high(M1)
    or      ax,ax      ; if high(M1) non-zero
    jz @f              ; then
    mul   dx           ; - low(M2) * high(M1)
  @@:                  ; endif
    xchg    ax,cx      ; save that in cx, get high(M2)
    or      ax,ax      ; if high(M2) non-zero
    jz @f              ; then
    mul   bx           ; - high(M2) * low(M1)
    add   cx,ax        ; - add to total
  @@:                  ; endif
    pop     ax         ; restore low(M2)
    mul     bx         ; low(M2) * low(M1)
    add     dx,cx      ; add previously computed high part
    ret                ; and return!!!
__U4M ENDP

_INIT ENDS

; Set DriverInit as the DLL entry point
END DriverInit
