// DIB Engine structures and functions

typedef struct tagPDEVICE {
    short pdType;
} PDEVICE, *LPPDEVICE;

typedef struct tagDIBENGINE {  
    WORD         deType;          /* The type of drawing surface. Must be either TYPE_DIBENG (0x5250) or 0.
                                     If TYPE_DIBENG, the surface being drawn to is video memory, a
                                     DIB, or a color device-dependent bitmap (DDB).
                                     If 0, the surface is a monochrome bitmap, and the PDEVICE must
                                     be in the format of a PBITMAP structure, not a DIBENGINE structure. */
                #define TYPE_DIBENG 0x5250
    WORD         deWidth;          /* Width of DIB in pixels. */
    WORD         deHeight;         /* Height of DIB in pixels. */
    WORD         deWidthBytes;     /* Number of bytes in each raster line. The number of bytes must be a DWORD multiple;
                                      all raster lines must be aligned on 16-bit boundaries. */
    BYTE         dePlanes;         /* Number of planes in the bitmap. Must be one. */
    BYTE         deBitsPixel;      /* Number of bits per pixel. */
    DWORD        deReserved1;      /* Reserved, do not use */
    DWORD        deDeltaScan;      /* The number of bytes to the next scan line. */
    LPBYTE       delpPDevice;      /* Address of the physical device. This can be a pointer to either video memory, a
                                      device-dependent bitmap, or a device-independent bitmap. */
    DWORD        deBitsOffset;     /* Offset to an array of bits that specifies the pixels in the DIB.
                                      The array must be aligned on a 16-bit boundary. */
    WORD         deBitsSelector;   /* Selector to an array of bits that specifies the pixels in the DIB.
                                      The array must be aligned on a 16-bit boundary. */
    WORD         deFlags;          /* Additional flags. May be a combination of the following: */
            #define MINIDRIVER      0x0001  /* Display driver  */
            #define PALETTIZED      0x0002  /* Palletized device  */
            #define SELECTEDDIB     0x0004  /* DIB Section  */
            #define OFFSCREEN       0x0008  /* Offscreen surface (use with VRAM)  */
            #define BUSY            0x0010  /* Disable the frame buffer  */
            #define NOT_FRAMEBUFFER 0x0020  /* Example: 8514/a or ATIM8  */
            #define FIVE6FIVE       0x0040  /* 16 bpp, 565 color format.  */
            #define NON64KBANK      0x0080  /* Bank size is not 64K  */
            #define VRAM            0x8000  /* Physical surface (video memory)  */
            #define BANKEDVRAM      0x4000  /* VFlatD simulated  */
            #define BANKEDSCAN      0x2000  /* VFlatD simulated (rasters may cross bank boundary)  */
            #define PALETTE_XLAT    0x1000  /* Background palette xlat  */
            #define VGADITHER       0x0800  /* Dither to VGA colors (first 8, and last 8)  */
            #define CTCHANGE        0x0400  /* Color table has been changed  */
            #define DITHER256       0x0200  /* Dither to 256 fixed colors */
    WORD         deVersion;        /* Version number. The least significant bit is the minor version number
                                      and the most significant bit is the major version number.
                                      For Windows 95 (Windows 4.0) the version number is 0400h. */
    LPBITMAPINFO deBitmapInfo;     /* Pointer to the BITMAPINFOHEADER */
    void         (WINAPI *deBeginAccess)();  /* Begin surface access call back. */
    void         (WINAPI *deEndAccess)();    /* End surface access call back. */
    DWORD        deDriverReserved; /* Reserved for minidriver use. */
} DIBENGINE;


/* Enables the DIB engine and initializes the dpCurves through dpStyleLen members of a GDIINFO structure.
   The minidriver must call this function when initializing the members of the GDIINFO structure.
   This function must be called before CreateDIBPDevice. 
       Returns TRUE if successful, FALSE otherwise. */
/* The export ordinal for this function is 5. */
/* Because the DIB engine support features in addition to those supported by the display minidriver,
   the DIB engine may set bits in the GDIINFO structure other than were set by the minidriver. In 
   such cases, the display minidriver must leave those bits set. */
WORD WINAPI DIB_Enable(
        LPVOID lpDevInfo,      /* Address of a GDIINFO structure. */
        UINT   wAction,        /* One of the following actions:
                                  0x0001    Initialize the dpCurves through dpStyleLen members of the
                                            GDIINFO structure pointed to by lpDevInfo. 
                                  0x8001    Same as 0x0001. The high bit is set if an application has
                                            requested an information context. */
        LPSTR  lpDestDevType,  /* Address of a string specifying the device name.
                                  This string only applies to device drivers that support more than one
                                  model of the same device. The parameter can be NULL if only one model
                                  is supported. */
        LPSTR  lpOutputFile,   /* Address of a string specifying the name of the output port or file to
                                  receive output from the driver. The string must be a valid MS-DOS
                                  filename. If the lpOutputFile parameter is NULL, the output device is
                                  the system display. */
        LPVOID lpData          /* Address of DEVMODE structure containing information the minidriver's
                                  Enable function uses to initialize the device. It can be NULL if no
                                  such information is needed. */
);

#define FB_ACCESS     1  /* Grant direct access to the framebuffer. */
#define CURSOREXCLUDE 8  /* Exclude the cursor (if a software cursor) from the given rectangle. */

/* Grants direct access to the frame buffer or excludes the cursor from the given rectangle. */
/* The ordinal value for this function is 105. */
void WINAPI DIB_BeginAccess(
        LPPDEVICE lpDevice,  /* Address of the PDEVICE structure. */
        WORD Left,
        WORD Top,
        WORD Right,
        WORD Bottom,
        WORD Flags           /* Can be one of these values: FB_ACCESS, CURSOREXCLUDE */
);

/* Ends direct access to the frame buffer or removes the exclusion rectangle for the cursor. */
/* The ordinal value for this function is 106. */
void WINAPI DIB_EndAccess(
        LPPDEVICE lpDevice,  /* Address of the PDEVICE structure. */
        WORD Flags           /* Can be one of these values: FB_ACCESS, CURSOREXCLUDE */
);

/* Sets one or more entries in the hardware color palette to the specified colors. */
/* No return value. */
/* The export ordinal for this function is 405. */
void WINAPI DIB_SetPaletteExt(
    WORD nStartIndex,      /* A zero-based color-palette index. The function copies color values to the corresponding
                              hardware-palette entry and to subsequent entries up to the number specified by the
                              nNumEntries parameter. */
    WORD nNumEntries,      /* The number of entries to set in the hardware palette. */
    LPRGBQUAD lpPalette,   /* Address of an array of RGBQUAD structures specifying the colors to which to set the
                              hardware-palette entries. */
    LPPDEVICE lpDIBEngine  /* Address of PDEVICE structure that specifies color data for the display surface. */
);
