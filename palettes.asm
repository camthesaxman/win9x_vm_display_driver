_DATACONST SEGMENT PUBLIC 'FAR_DATA'

NONSTATIC  EQU 80h
MAPTOWHITE EQU 01h

PUBLIC _palette4bpp
PUBLIC _palette8bpp

; 16 color palette
ALIGN 4
_palette4bpp:
    ;  blu  grn  red  flags
    DB   0,   0,   0, 0           ; Black
    DB   0,   0, 128, 0           ; Dark Red
    DB   0, 128,   0, 0           ; Dark Green
    DB   0, 128, 128, 0           ; Dark Yellow
    DB 128,   0,   0, 0           ; Dark Blue
    DB 128,   0, 128, 0           ; Dark Purple
    DB 128, 128,   0, 0           ; Dark Cyan
    DB 192, 192, 192, MAPTOWHITE  ; Light Gray
    DB 128, 128, 128, MAPTOWHITE  ; Dark Gray
    DB   0,   0, 255, 0           ; Light Red
    DB   0, 255,   0, MAPTOWHITE  ; Light Green
    DB   0, 255, 255, MAPTOWHITE  ; Light Yellow
    DB 255,   0,   0, 0           ; Light Blue
    DB 255,   0, 255, 0           ; Light Purple
    DB 255, 255,   0, MAPTOWHITE  ; Light Cyan
    DB 255, 255, 255, MAPTOWHITE  ; White

; 256 color palette
ALIGN 4
_palette8bpp:
    ;  blu  grn  red  flags
    ; System Reserved colors
    DB   0,   0,   0, 0                     ; Black
    DB   0,   0, 128, 0                     ; Dark Red
    DB   0, 128,   0, 0                     ; Dark Green
    DB   0, 128, 128, 0                     ; Dark Yellow
    DB 128,   0,   0, 0                     ; Dark Blue
    DB 128,   0, 128, 0                     ; Dark Purple
    DB 128, 128,   0, 0                     ; Dark Cyan
    DB 192, 192, 192, MAPTOWHITE            ; Light Gray
    DB 192, 220, 192, NONSTATIC+MAPTOWHITE  ; Money Green
    DB 240, 202, 166, NONSTATIC+MAPTOWHITE  ; Cool Blue
    ; Palette Manager colors
REPT 236
    DB   0,   0,   0, NONSTATIC
ENDM
    ; System Reserved colors
    DB 240, 251, 255, NONSTATIC+MAPTOWHITE  ; Off White
    DB 164, 160, 160, NONSTATIC+MAPTOWHITE  ; Medium Gray
    DB 128, 128, 128, MAPTOWHITE            ; Dark Grey
    DB   0,   0, 255, 0                     ; Light Red
    DB   0, 255,   0, MAPTOWHITE            ; Light Green
    DB   0, 255, 255, MAPTOWHITE            ; Light Yellow
    DB 255,   0,   0, 0                     ; Light Blue
    DB 255,   0, 255, 0                     ; Light Purple
    DB 255, 255,   0, MAPTOWHITE            ; Light Cyan
    DB 255, 255, 255, MAPTOWHITE            ; White

_DATACONST ENDS


END
