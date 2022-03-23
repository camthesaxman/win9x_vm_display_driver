typedef struct tagGDIINFO {
    short int           dpVersion;      /* Version number. The high-order byte specifies the major version, the
					   low-order byte the minor version. For example, in a device driver developed
					   for Windows 3.1, this member should contain 0x030A. */
    short int           dpTechnology;   /* Device technology. Can be one of these values: */
        #define DT_PLOTTER    0  /* Vector plotter */
        #define DT_RASDISPLAY 1  /* Raster display */
        #define DT_RASPRINTER 2  /* Raster printer */
    short int           dpHorzSize;     /* Width, in millimeters, of the physical display surface. */
    short int           dpVertSize;     /* Height, in millimeters, of the physical display surface. */
    short int           dpHorzRes;      /* Width, in pixels, of the display surface. For nonraster devices, this width
					   is equivalent to the number of vertical grid lines used by the device to plot
                                           points on the display surface. In such cases, a pixel is defined to be the
                                           smallest mark the device can draw. */
    short int           dpVertRes;      /* Height, in raster lines, of the display. For nonraster devices, this height
					   is equivalent to the number of horizontal grid lines used by the device to
					   plot points on the display surface. In such cases, a raster line is
					   equivalent to a gridline. */
    short int           dpBitsPixel;    /* Number of adjacent bits on each plane required to define a single pixel. */
    short int           dpPlanes;       /* Number of planes required to define the pixels.
                                           For a typical raster device with red, green, and blue bit planes
                                           (such as a 3-plane EGA), this member is 3. */
    short int           dpNumBrushes;   /* Number of device-specific brushes supported by the device. */
    short int           dpNumPens;      /* Number of device-specific pens supported by the device. */
    short int           dpCapsFE;       /* Capability bits for Far East fonts. Can be a combination of these values: */
        #define FEC_TT_DBCS   0x0020  /* Can output DBCS True Type fonts */
        #define FEC_WIFE_ABLE 0x0080  /* Can handle WIFE font as Engine font */
    short int           dpNumFonts;     /* Number of device-specific fonts supported by the device. */
    short int           dpNumColors;    /* Number of entries in the color table for the device or the number of reserved
                                           colors for a palette-capable device. */
    short int           dpDEVICEsize;   /* Size, in bytes, of the PDEVICE structure for this device.
                                           It must be at least two bytes. */
    unsigned short int  dpCurves;       /* Curve-drawing capabilities. This member specifies the types of curves a
					   device can draw, whether the interior of those figures that can be handled
					   can be brushed in, and whether the borders of those figures that can be
					   handled can be drawn with wide lines, styled lines, or lines that are both
					   wide and styled. Can be a combination of these values: */
        #define CC_NONE       0x0000  /* Curves not supported. */
        #define CC_CIRCLES    0x0001  /* Draws circles. */
        #define CC_PIE        0x0002  /* Draws pie wedges. */
        #define CC_CHORD      0x0004  /* Draws chord arcs. */
        #define CC_ELLIPSES   0x0008  /* Draws ellipses. */
        #define CC_WIDE       0x0010  /* Draws wide lines. */
        #define CC_STYLED     0x0020  /* Draws styled lines. */
        #define CC_WIDESTYLED 0x0040  /* Draws lines that are wide and styled. */
        #define CC_INTERIORS  0x0080  /* Draws interiors. */
        #define CC_ROUNDRECT  0x0100  /* Draws round rectangles. */
        #define CC_POLYBEZIER 0x0200  /* Draws poly Bezier splines. */
    unsigned short int  dpLines;        /* Polyline and line-drawing capabilities. This member also specifies the types
                                           of lines a device can draw, whether the interior of figures can be brushed
					   in, and whether the borders of figures can be drawn with wide lines, styled
					   lines, or lines that are both wide and styled. Can be a combination of these
					   values: */
        #define LC_NONE         0x0000  /* Lines not supported. */
        #define LC_POLYSCANLINE 0x0001  /* Draws a group of scanlines. */
        #define LC_POLYLINE     0x0002  /* Draws polylines. */
        #define LC_WIDE         0x0010  /* Draws wide lines. */
        #define LC_STYLED       0x0020  /* Draws styled lines. */
        #define LC_WIDESTYLED   0x0040  /* Draws wide styled lines. */
        #define LC_INTERIORS    0x0080  /* Draws interiors. */
    unsigned short int  dpPolygonals;   /* Polygon-, rectangle-, and scan-line drawing capabilities. This member also
                                           specifies whether the interior of those figures that can be handled can be
                                           brushed in, and whether the borders of those figures that can be handled can
                                           be drawn with wide lines, styled lines, or lines that are both wide and
                                           styled. Can be a combination of these values: */
        #define PC_NONE        0x0000  /* Polygons not supported. */
        #define PC_ALTPOLYGON  0x0001  /* Draws alternate-fill polygons. */
        #define PC_RECTANGLE   0x0002  /* Draws rectangles. */
        #define PC_WINDPOLYGON 0x0004  /* Draws winding-number-fill polygons. */
        #define PC_SCANLINE    0x0008  /* Draws scan lines. */
        #define PC_WIDE        0x0010  /* Draws wide borders. */
        #define PC_STYLED      0x0020  /* Draws styled borders. */
        #define PC_WIDESTYLED  0x0040  /* Draws borders that are wide and styled. */
        #define PC_INTERIORS   0x0080  /* Draws interiors. */
        #define PC_POLYPOLYGON 0x0100  /* Draws multiple polygons. */
        #define PC_PATHS       0x0200  /* Draws paths. */
    unsigned short int  dpText;         /* Level of text support the device driver provides. Can be a combination of
                                           these values: */
        #define TC_OP_CHARACTER 0x0001  /* Generates character-precision text. If this value is not given (or implied by
                                           the TC_OP_STROKE value), the driver can generate string-precision text only. */
        #define TC_OP_STROKE    0x0002  /* Generates stroke-precision text. This value implies the TC_OP_CHARACTER
                                           value. */
        #define TC_CP_STROKE    0x0004  /* Draws partially clipped characters. If this value is not given, the character
                                           must be entirely within the clipping region to be drawn. */
        #define TC_CR_90        0x0008  /* Rotates characters in 90-degree increments. If this value is not given (or
                                           implied by the TC_CR_ANY value), the driver cannot rotate text. */
        #define TC_CR_ANY       0x0010  /* Rotates characters to any angle. This value implies the TC_CR_90 value. */
        #define TC_SF_X_YINDEP  0x0020  /* Scales characters independently along the x- and y-axes. If this value is not
                                           given, the driver may be able to scale characters but not independently along
                                           the axes. */
        #define TC_SA_DOUBLE    0x0040  /* Scales characters by doubling. If this value is not given (or implied by the
                                           TC_SA_INTEGER or TC_SA_CONTIN values), the driver cannot scale text. */
        #define TC_SA_INTEGER   0x0080  /* Scales characters by integral multiples. This value implies the TC_SA_DOUBLE
                                           value. */
        #define TC_SA_CONTIN    0x0100  /* Scales characters by any multiple. This value implies the TC_SA_DOUBLE and
                                           TC_SA_INTEGER values. */
        #define TC_EA_DOUBLE    0x0200  /* Generates bold characters by doubling the weight. If this value is not given,
                                           the driver cannot modify character weights. */
        #define TC_IA_ABLE      0x0400  /* Generates italic characters by skewing. */
        #define TC_UA_ABLE      0x0800  /* Generates underlined characters. */
        #define TC_SO_ABLE      0x1000  /* Generates struck-out characters. */
        #define TC_RA_ABLE      0x2000  /* Can use raster fonts to generate text. */
        #define TC_VA_ABLE      0x4000  /* Can use vector fonts to generate text. */
        #define TC_RESERVED     0x8000  /* Reserved; must be zero. */
    unsigned short int  dpClip;         /* Clipping capabilities. Can be one of these values: */
        #define CP_NONE      0x0000  /* Cannot clip. */
        #define CP_RECTANGLE 0x0001  /* Can clip using a single rectangle. */
        #define CP_REGION    0x0002  /* Not supported. */
        #define CP_REGION32  0x0004  /* The driver will receive one of three values for lpClipRect: NULL (meaning cannot
                                        clip), or a far pointer to either a REGION or a RECT structure. (If rgn_id is 
                                        0x8000 it is a REGION structure; otherwise, it is a RECT structure.) */
    unsigned short int  dpRaster;       /* Raster capabilities. Can be a combination of these values: */
        #define RC_NONE       0x0000  /* No raster capabilities. */
        #define RC_BITBLT     0x0001  /* Can transfer bitmaps. The driver exports the BitBlt function. */
        #define RC_BANDING    0x0002  /* Requires banding support. */
        #define RC_SCALING    0x0004  /* Requires scaling support. */
        #define RC_SAVEBITMAP 0x0040  /* Saves bitmaps locally in shadow memory. The driver exports the
                                         SaveScreenBitmap function. */
        #define RC_PALETTE    0x0100  /* Does color-palette management. */
        #define RC_DIBTODEV   0x0200  /* Transfers DIBs directly to device. The driver exports the SetDIBitsToDevice
                                         function. */
        #define RC_BIGFONT    0x0400  /* Supports Windows 3.x fonts. If this value is not given, GDI ensures that the
                                         driver receives Windows 2.x fonts only. */
        #define RC_STRETCHBLT 0x0800  /* Stretches and compresses bitmaps while transferring the bitmap. The driver
                                         exports the StretchBlt function. */
        #define RC_FLOODFILL  0x1000  /* Perform flood filling. The driver exports the FloodFill function. */
        #define RC_STRETCHDIB 0x2000  /* Stretches and compresses device-independent bitmaps while transferring the
                                         bitmap. The driver exports the StretchDIBits function. */
        #define RC_DEVBITS    0x8000  /* Supports device bitmaps. Driver exports the BitmapBits and SelectBitmap
                                         function. */
    short int           dpAspectX;      /* Relative width of a device pixel. This value, in the range 1 through 1000,
                                           helps specify the device's aspect ratio. */
    short int           dpAspectY;      /* Relative height of a device pixel. This value, in the range 1 through 1000,
                                           helps specify the device's aspect ratio. */
    short int           dpAspectXY;     /* Relative diagonal width of a device pixel. This value, in the range 1 through
                                           1000, helps specify the device's aspect ratio. It must be equal to the square
                                           root of the sum of the squares of dpAspectX and dpAspectY. */
    short int           dpStyleLen;     /* Minimum length of a dot generated by a styled pen. The length is relative to
                                           the width of a device pixel and should be given in the same units as
                                           dpAspectX. For example, if dpAspectX is 5 and the minimum length required is
					   3 pixels, dpStyleLen should be 15. */
    POINT               dpMLoWin;       /* Width and height of the metric (low resolution) window. The width is
                                           dpHorzSize*10; the height is dpVertSize*10. */
    POINT               dpMLoVpt;       /* Horizontal and vertical resolutions of the metric (low resolution) viewport.
                                           The horizontal resolution is dpHorzRes; the vertical resolution is
					   –dpVertRes. */
    POINT               dpMHiWin;       /* Width and height of the metric (high resolution) window. The width is
                                           dpHorzSize*100; the height is dpVertSize*100. */
    POINT               dpMHiVpt;       /* Horizontal and vertical resolutions of the metric (high resolution) viewport.
                                           The horizontal resolution is dpHorzRes; the vertical resolution is
					   –dpVertRes. */
    POINT               dpELoWin;       /* Width and height of the English (low resolution) window. The width is
                                           dpHorzSize*1000; the height is dpVertSize*1000. */
    POINT               dpELoVpt;       /* Horizontal and vertical resolutions of the English (low resolution) viewport.
                                           The horizontal resolution is dpHorzRes*254; the vertical resolution is
                                           –dpVertRes*254. */
    POINT               dpEHiWin;       /* Width and height of the English (high resolution) window. The width is
                                           dpHorzSize*10,000; the height is dpVertSize*10,000. */
    POINT               dpEHiVpt;       /* Horizontal and vertical resolutions of the English (high resolution)
					   viewport. The horizontal resolution is dpHorzRes*254; the vertical resolution
					   is –dpVertRes*254. */
    POINT               dpTwpWin;       /* Width and height of the twip window. There are 20 twips per 1 printer's point
                                           and 72 printer's points per inch. The width is dpHorzSize*14400; the height
					   is dpVertSize*14400. */
    POINT               dpTwpVpt;       /* Horizontal and vertical resolutions of the twip viewport. The horizontal
                                           resolution is dpHorzRes*254; the vertical resolution is –dpVertRes*254. */
    short int           dpLogPixelsX;   /* Number of pixels per logical inch along a horizontal line on the display
                                           surface. This value is used to match fonts. */
    short int           dpLogPixelsY;   /* Number of pixels per logical inch along a vertical line on the display
                                           surface. This value is used to match fonts. */
    short int           dpDCManage;     /* Device-context management capabilities. Can be one of these values: */

        /* 0x0000                        Driver allows multiple DCs. It creates a new PDEVICE structure for each DC that
                                         specifies a new device and filename pair, but uses the same PDEVICE structure
                                         for any subsequent DCs that specify the same device and filename pair. */
        #define DC_SPDEVICE   0x0001  /* Driver allows multiple DCs but it creates a new PDEVICE structure for each DC
                                         regardless of whether the device and filename pairs are the same. */
        #define DC_1PDEVICE   0x0002  /* Driver allows multiple DCs but only if all DCs have unique device and filename
                                         pairs. The driver creates a PDEVICE structure for each DC. The driver returns
                                         an error on any attempt to create a second DC with an existing device and
                                         filename pair. */
        #define DC_IGNOREDFNP 0x0004  /* Driver allows multiple DCs but only creates one PDEVICE structure . All DCs
                                         share the same PDEVICE structure regardless of the device and filename pairs. */
        /* 0x0006                        Driver allows only one DC. The driver returns an error on any attempt to create
                                         a second DC. */
    unsigned short int  dpCaps1;        /* Additional raster abilities. Can be one of these values: */
        #define C1_TRANSPARENT    0x0001  /* Driver supports transparent BitBlts. */
        #define TC_TT_ABLE        0x0002  /* Informs GDI that the driver is capable of producing TrueType as raster
                                             fonts. The driver must call the dmExtTextOut function to draw the raster
                                             font into the bitmap. The value is similar to TC_RA_ABLE. */
        #define C1_TT_CR_ANY      0x0004  /* Driver can rotate TrueType fonts. */
        #define C1_EMF_COMPLIANT  0x0008  /* Driver supports metafile spooling. */
        #define C1_DIBENGINE      0x0010  /* Driver is DIB engine compliant. */
        #define C1_GAMMA_RAMP     0x0020  /* Driver supports gamma ramp setting. */
        #define C1_ICM            0x0040  /* Driver does some form of ICM support. */
        #define C1_REINIT_ABLE    0x0080  /* Driver supports ReEnable call. */
        #define C1_GLYPH_INDEX    0x0100  /* Driver supports glyph index fonts. */
        #define C1_BIT_PACKED     0x0200  /* Driver supports bit-packed glyphs. */
        #define C1_BYTE_PACKED    0x0400  /* Driver supports byte-packed glyphs. */
        #define C1_COLORCURSOR    0x0800  /* Driver supports color-cursors and async SetCursors. */
        #define C1_CMYK_ABLE      0x1000  /* Driver supports CMYK COLOREF structures. */
        #define C1_SLOW_CARD      0x2000  /* Driver has little or no acceleration (for example, VGA). */
    long                dpSpotSizeX;    /* Horizontal spot size for TrueType fonts on this device. */
    long                dpSpotSizeY;    /* Vertical spot size for TrueType fonts on this device. */
    short int           dpNumPalReg;    /* Number of entries in the device's palette. This is the total number of
                                          simultaneous colors available for palette-capable devices. Nonpalette-capable
                                          devices ignore this value. */
    short int           dpPalReserved;  /* Even number of reserved system colors available for palette-capable devices.
					   Nonpalette-capable devices ignore this value. */
    short int           dpColorRes;     /* Palette resolution, which equals the number of bits going into video DACS.
					   Nonpalette-capable devices ignore this value. */
} GDIINFO;
