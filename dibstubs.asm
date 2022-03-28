; This file contains driver functions which simply forward to the built-in
; Windows implementations in DIBENG.DLL.


.MODEL MEDIUM
.386


DIBDEBUG EQU 0


;-------------------------------------------------------------------------------
; Imports
;-------------------------------------------------------------------------------
IFDEF DEBUG
EXTERN debug_print_     : NEAR
ENDIF
EXTERN _lpDriverPDevice : DWORD
EXTERN _wPalettized     : WORD


;-------------------------------------------------------------------------------
; Code
;-------------------------------------------------------------------------------
_INIT SEGMENT WORD PUBLIC USE16 'CODE'

; Simple jump to the target function
DIB_STUB MACRO _name, _target
IF DIBDEBUG
    LOCAL string
    string db "&_name" , 10, 0
ENDIF
    EXTERN _target:FAR
    PUBLIC _name
    _name:
IF DIBDEBUG
    ; print name
    push es
    push ds
    push dx
    mov dx, cs
    mov ax, OFFSET string
    push cs
    call debug_print_
    pop dx
    pop ds
    pop es
ENDIF
    ; jump to DIB function
    jmp _target
ENDM

; inserts an extra parameter onto the stack, then jumps to the target function
DIB_STUB_EX MACRO _name, _target, _param
IF DIBDEBUG
    LOCAL string
    string DB "&_name" , 10, 0
ENDIF
    EXTERN _target:FAR
    PUBLIC _name
    _name:
IF DIBDEBUG
    ; print name
    push es
    push ds
    push dx
    mov dx, cs
    mov ax, OFFSET string
    push cs
    call debug_print_
    pop dx
    pop ds
    pop es
ENDIF
    ; insert extra param onto stack
    push es
    mov ax, DGROUP
    mov es, ax
    pop ax
    ASSUME es:_DATA
    pop ecx
    push es:_param
    push ecx
    mov es, ax
    ; jump to DIB function
    jmp _target
ENDM

DIB_STUB BitBlt,              DIB_BitBlt
DIB_STUB Control,             DIB_Control
DIB_STUB ColorInfo,           DIB_ColorInfo
DIB_STUB GetCharWidth,        DIB_GetCharWidth
DIB_STUB Inquire,             DIB_Inquire
DIB_STUB DibToDevice,         DIB_DibToDevice
DIB_STUB CreateDIBitmap,      DIB_CreateDIBitmap
DIB_STUB EnumDFonts,          DIB_EnumDFonts
DIB_STUB Output,              DIB_Output
DIB_STUB Pixel,               DIB_Pixel
DIB_STUB SelectBitmap,        DIB_SelectBitmap
DIB_STUB BitmapBits,          DIB_BitmapBits
DIB_STUB ScanLR,              DIB_ScanLR
DIB_STUB DeviceMode,          DIB_DeviceMode
DIB_STUB StretchBlt,          DIB_StretchBlt
DIB_STUB StretchDIBits,       DIB_StretchDIBits
DIB_STUB DeviceBitmap,        DIB_DeviceBitmap
DIB_STUB SetAttribute,        DIB_SetAttribute
DIB_STUB FastBorder,          DIB_FastBorder
;DIB_STUB SetPalette,          DIB_SetPalette
DIB_STUB ExtTextOut,          DIB_ExtTextOut
DIB_STUB StrBlt,              DIB_StrBlt

DIB_STUB_EX SetCursor,           DIB_SetCursorExt,           _lpDriverPDevice
DIB_STUB_EX DibBlt,              DIB_DibBltExt,              _wPalettized
DIB_STUB_EX RealizeObject,       DIB_RealizeObjectExt,       _lpDriverPDevice
DIB_STUB_EX GetPalette,          DIB_GetPaletteExt,          _lpDriverPDevice
DIB_STUB_EX SetPaletteTranslate, DIB_SetPaletteTranslateExt, _lpDriverPDevice
DIB_STUB_EX GetPaletteTranslate, DIB_GetPaletteTranslateExt, _lpDriverPDevice
DIB_STUB_EX UpdateColors,        DIB_UpdateColorsExt,        _lpDriverPDevice
DIB_STUB_EX CheckCursor,         DIB_CheckCursorExt,         _lpDriverPDevice
DIB_STUB_EX MoveCursor,          DIB_MoveCursorExt,          _lpDriverPDevice
DIB_STUB_EX EnumObj,             DIB_EnumObj,                _lpDriverPDevice

_INIT ENDS


END
