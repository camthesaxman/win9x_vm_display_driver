#include <windows.h>

#include "dibeng.h"
#include "gdidefs.h"
#include "minivdd.h"
#include "valmode.h"

#include "debug.h"

// __saveregs is needed on every function that this driver exports because
// the callers expect the DS and ES registers to be preserved.
#define DLLFUNC  WINAPI __saveregs

// int 0x2F functions
#define GET_VM_HANDLE              0x1683
#define GET_DEVICE_API_ENTRY_POINT 0x1684
#define STOP_IO_TRAP               0x4000
#define START_IO_TRAP              0x4007

extern int bga_hardware_detect(void);
extern int bga_phys_enable(void);

DWORD dwVideoMemSize = 16 * 1024 * 1024;
DWORD dwFrameBufSize = 0;
WORD wScanlineSize = 0;
void *lpDriverPDevice = NULL;
BOOL bReEnable = FALSE;
WORD wFrameBufSelector;
void FAR* dwVDDEntry;
WORD wVMHandle;

// video settings
WORD wXResolution = 640;
WORD wYResolution = 480;
WORD wBPP = 32;
WORD wDPI = 96;
WORD wPalettized = 0;

#define MAPTOWHITE 1

extern RGBQUAD palette4bpp[16];
extern RGBQUAD palette8bpp[256];

#pragma code_seg("_INIT")

void bga_set_palette(const RGBQUAD* colors, BYTE startIndex, int nColors)
{
    const RGBQUAD* color = colors;

    // write index of first color
    __asm {
        mov dx, 0x3C8
        mov al, startIndex
        out dx, al
    }
    while (nColors-- > 0)
    {
        // Contrary to the documentation and struct definitions, the RGBQUADs
        // passed into SetPalette seem to actually be red, green, blue, not blue, green, red
        BYTE r = color->rgbBlue / 4;
        BYTE g = color->rgbGreen / 4;
        BYTE b = color->rgbRed / 4;
        // write color value
        __asm {
            mov dx, 0x03C9
            mov al, r
            out dx, al
            mov al, g
            out dx, al
            mov al, b
            out dx, al
        }
        color++;
    }
}

WORD calc_scanline_size(WORD w, WORD bpp)
{
    return (w * bpp + 7) >> 3;
}

DWORD calc_framebuffer_size(WORD w, WORD h, WORD bpp)
{
    return (DWORD)calc_scanline_size(w, bpp) * h;
}

int is_supported_mode(int w, int h, int bpp)
{
    return (w >= 320 && h >= 240
     && (bpp == 4 || bpp == 8 || bpp == 16 || bpp == 32));
}

void load_display_settings(void)
{
    DISPLAYINFO dispInfo = {0};
    int result;

    // get info from registry
    void* pdispInfo = &dispInfo;
    DWORD size = sizeof(dispInfo);
    debug_print("load_display_settings\n");
    __asm {
        mov ax, VDD_GET_DISPLAY_CONFIG
        les di, pdispInfo
        mov ecx, size
        movzx ebx, wVMHandle
        call dwVDDEntry
        mov result, ax
    }

    switch (result)
    {
    default:
    case -1:
        debug_print("error reading registry\n");
        break;
    case VDD_GET_DISPLAY_CONFIG:
        debug_print("VDD not present\n");
        break;
    case 0:  // success
        debug_print("got resolution settings\n");
        if (is_supported_mode(dispInfo.diXRes, dispInfo.diYRes, dispInfo.diBpp)
         && dwVideoMemSize >= calc_framebuffer_size(dispInfo.diXRes, dispInfo.diYRes, dispInfo.diBpp))
        {
            wXResolution = dispInfo.diXRes;
            wYResolution = dispInfo.diYRes;
            wBPP = dispInfo.diBpp;
        }
        else
            debug_print("unsupported video mode\n");
        break;
    }

    wPalettized = (wBPP <= 8);

    debug_print("display configuration:\n");
    PRINTVAR(wXResolution);
    PRINTVAR(wYResolution);
    PRINTVAR(wBPP);
    PRINTVAR(wPalettized);
}

int (WINAPI* redrawScreenFunc)() = NULL;

// driver entry point - returns 1 on success
int DLLFUNC display_driver_init(void)
{
    WORD codeSeg;
    WORD dataSeg;

    debug_print("display_driver_init\n");

    __asm {
        mov codeSeg, cs
        mov dataSeg, ds
    }
    PRINTVAR(codeSeg);
    PRINTVAR(dataSeg);

    // Get VDD entry point (es:di)
    __asm {
        mov ax, GET_DEVICE_API_ENTRY_POINT
        mov bx, 0x000A
        int 0x2F
        mov WORD PTR dwVDDEntry, di
        mov WORD PTR dwVDDEntry+2, es
    }
    PRINTVAR(dwVDDEntry);

    // Get VM handle
    __asm {
        mov ax, GET_VM_HANDLE
        int 0x2F
        mov WORD PTR wVMHandle, bx
    }
    PRINTVAR(wVMHandle);

    if (!bga_hardware_detect())
        return 0;

    PRINTVAR(dwVideoMemSize);
    load_display_settings();

    return 1;
}

void redraw_screen(void)
{
	debug_print("redraw_screen\n");
    if (redrawScreenFunc == NULL)
    {
        HMODULE hUser = GetModuleHandle("USER");
        PRINTVAR(hUser);
        if (hUser != NULL)
        {
            redrawScreenFunc = GetProcAddress(hUser, (LPCSTR)275);
            PRINTVAR(redrawScreenFunc);
        }
    }
    if (redrawScreenFunc != NULL)
        redrawScreenFunc();
}

void DLLFUNC ResetHiResMode(void)
{
    debug_print("ResetHiResMode\n");
    bga_phys_enable();
    redraw_screen();
    debug_print("ResetHiResMode done\n");
}

static void register_with_vdd(void)
{
    debug_print("registering with VDD\n");
    __asm {
        // Give the mini-VDD some info
        mov eax, VDD_REGISTER_DISPLAY_DRIVER_INFO
        movzx ebx, wVMHandle  // ebx = VM handle
        xor edi, edi
        movzx edx, wFrameBufSelector
        mov ecx, dwVideoMemSize
        call dwVDDEntry
    }

    __asm {
        // Register driver
        mov eax, VDD_DRIVER_REGISTER
        movzx ebx, wVMHandle  // ebx = VM handle
        // es:di = address of ResetHiResMode
        mov di, cs
        mov es, di
        mov di, OFFSET ResetHiResMode
        mov ecx, dwFrameBufSize  // ecx = frame buffer size
        mov edx, -1              // edx = something to do with emulating VGA in a window?
        call dwVDDEntry
    }

    debug_print("disabling I/O trap\n");
    __asm {
        mov ax, STOP_IO_TRAP
        int 0x2F
    }
}

VOID FAR PASCAL CreateDIBPDevice(LPBITMAPINFO lpInfo, LPPDEVICE lpDevice,
     VOID *lpBits, WORD wFlags);

WORD wPDevSize = 0;
WORD wEnabled = 0;

void *memset(void *ptr, int value, long int num)
{
    unsigned char *p = ptr;
    while (num-- > 0)
        *p++ = value;
    return ptr;
}

void WINAPI SetPaletteTranslate(LPVOID pal);

// returns size of GDIINFO struct on success, or 0 on failure
UINT DLLFUNC Enable(LPVOID lpDevice, UINT style, LPSTR lpDeviceType, LPSTR lpOutputFile, LPVOID lpStuff)
{
    int i;

    if (style & 1)
    {
        GDIINFO *gi = lpDevice;

        debug_print("Enable: get gdiinfo\n");

        DIB_Enable(lpDevice, style, lpDeviceType, lpOutputFile, lpStuff);
        wPDevSize = gi->dpDEVICEsize;

        // Fill out GDIINFO struct
        gi->dpVersion = 0x400;  // designed for Windows 95
        gi->dpTechnology = DT_RASDISPLAY;
        gi->dpHorzSize = 208;
        gi->dpVertSize = 156;
        gi->dpNumFonts = 0;
        gi->dpCaps1 = C1_REINIT_ABLE|C1_BYTE_PACKED|C1_GLYPH_INDEX|C1_COLORCURSOR|C1_SLOW_CARD;
        gi->dpLogPixelsX = wDPI;
        gi->dpLogPixelsY = wDPI;
        gi->dpDCManage = DC_IGNOREDFNP;
        gi->dpHorzRes = wXResolution;
        gi->dpVertRes = wYResolution;
        gi->dpBitsPixel = wBPP;
        gi->dpPlanes = 1;

        if (wBPP == 8)
        {
            gi->dpNumPens = 16;
            gi->dpNumColors = 20;
            gi->dpNumBrushes = -1;
            gi->dpNumPalReg = 256;
            gi->dpPalReserved = 20;
            gi->dpColorRes = 18;
            gi->dpRaster |= RC_PALETTE|RC_DIBTODEV|RC_SAVEBITMAP;
            gi->dpDEVICEsize += sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
        }
        else if (wBPP < 8)
        {
            int nColors = (1 << wBPP);

            gi->dpNumPens = gi->dpNumColors = nColors;
            gi->dpNumBrushes = -1;
            gi->dpNumPalReg = 0;
            gi->dpPalReserved = 0;
            gi->dpColorRes = 0;
            gi->dpRaster |= RC_DIBTODEV;
            gi->dpDEVICEsize += sizeof(BITMAPINFOHEADER) + nColors * sizeof(RGBQUAD);
        }
        else  // if (wBPP > 8)
        {
            //PRINTVAR(gi->dpDEVICEsize);
            gi->dpNumPens = gi->dpNumColors = -1;
            gi->dpNumBrushes = -1;
            gi->dpNumPalReg = 0;
            gi->dpPalReserved = 0;
            gi->dpColorRes = 0;
            gi->dpRaster |= RC_DIBTODEV;
            gi->dpDEVICEsize += sizeof(BITMAPINFOHEADER);
            //PRINTVAR(gi->dpDEVICEsize);
        }
        return sizeof(GDIINFO);
    }
    else
    {
        DIBENGINE *dibeng = lpDevice;
        BITMAPINFO FAR *bi = (BITMAPINFO *)(dibeng + 1);  // immediately follows DIBENGINE struct

        debug_print("Enable: init pdevice\n");

        lpDriverPDevice = lpDevice;
        // compute size of scanline in bytes (rounded up)
        wScanlineSize = calc_scanline_size(wXResolution, wBPP);
        // compute size of framebuffer in bytes
        dwFrameBufSize = calc_framebuffer_size(wXResolution, wYResolution, wBPP);
        PRINTVAR(wScanlineSize);
        PRINTVAR(dwFrameBufSize);

        bga_phys_enable();  // physically enable hardware
        register_with_vdd();

        DIB_Enable(lpDevice, style, lpDeviceType, lpOutputFile, lpStuff);

        if (wPalettized)
            SetPaletteTranslate(NULL);

        // Fill out PDEVICE struct
        // TODO: maybe use CreateDIBPDevice to help with this?
        dibeng->deType = TYPE_DIBENG;
        dibeng->deWidth = wXResolution;
        dibeng->deHeight = wYResolution;
        dibeng->deWidthBytes = wScanlineSize;
        dibeng->deDeltaScan = wScanlineSize;
        dibeng->dePlanes = 1;
        dibeng->deBitsPixel = wBPP;
        dibeng->deReserved1 = 0;
        dibeng->delpPDevice = NULL;
        dibeng->deBitsSelector = wFrameBufSelector;
        dibeng->deBitsOffset = 0;
        dibeng->deFlags = MINIDRIVER|VRAM|BANKEDVRAM|BANKEDSCAN;
        if (wBPP == 16)
            dibeng->deFlags |= FIVE6FIVE;
        dibeng->deVersion = 0x400;  // designed for Windows 95
        dibeng->deBeginAccess = DIB_BeginAccess;
        dibeng->deEndAccess = DIB_EndAccess;
        dibeng->deDriverReserved = 0;
        dibeng->deBitmapInfo = bi;

        bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi->bmiHeader.biWidth = wXResolution;
        bi->bmiHeader.biHeight = wYResolution;
        bi->bmiHeader.biPlanes = 1;
        bi->bmiHeader.biBitCount = wBPP;
        // zero out the rest
        bi->bmiHeader.biCompression
            = bi->bmiHeader.biSizeImage
            = bi->bmiHeader.biXPelsPerMeter
            = bi->bmiHeader.biYPelsPerMeter
            = bi->bmiHeader.biClrUsed
            = bi->bmiHeader.biClrImportant
            = 0;

        if (wBPP == 8)
        {
            for (i = 0; i < 256; i++)
                bi->bmiColors[i] = palette8bpp[i];
            bga_set_palette(palette4bpp, 0, 256);
        }
        else if (wBPP == 4)
        {
            for (i = 0; i < 16; i++)
                bi->bmiColors[i] = palette4bpp[i];
            bga_set_palette(palette4bpp, 0, 16);
        }

        wEnabled = 1;

        debug_print("inited pdevice!\n");
        return 1;
    }

    return 0;
}

void DLLFUNC UserRepaintDisable(BYTE disable)
{
    // TODO: implement
    /*
    debug_print("UserRepaintDisable: ");
    PRINTVAR(disable);
    */
}

// Undocumented function. Code based on Windows 95 DDK samples
LONG DLLFUNC GetDriverResourceID(WORD wResId, void *lpResType)
{
    if (wResId != 3 || wDPI == 96)
        return wResId;
    return 2003;
}

// Called whenever a mode change is requested
UINT DLLFUNC ReEnable(LPPDEVICE lpDevice, void *lpGDIInfo)
{
	debug_print("ReEnable\n");
    bReEnable = TRUE;
    load_display_settings();
    // Call Enable to fill in a new GDIINFO block
    Enable(lpGDIInfo, 1, NULL, NULL, NULL);
    // Call Enable to create a new PDevice and init the hardware
    return Enable(lpDevice, 0, NULL, NULL, NULL);
    bReEnable = FALSE;
    return 1;
}

int DLLFUNC Disable(LPPDEVICE lpDevice)
{
    debug_print("Disable\n");
    // start I/O trapping
    debug_print("re-enabling I/O traps\n");
    __asm {
        mov ax, START_IO_TRAP
        int 0x2F
    }
    // unregister from the VDD
    debug_print("unregistering from VDD\n");
    PRINTVAR(dwVDDEntry);
    __asm {
        //int 3
        mov eax, VDD_DRIVER_UNREGISTER
        movzx ebx, wVMHandle
        call dwVDDEntry
    }
    // go back to VGA text mode 3
    debug_print("switching to VGA text mode\n");
    __asm {
        mov ax, 3
        int 0x10
    }
    debug_print("Disable: done\n");
    return -1;
}

UINT DLLFUNC ValidateMode(DISPVALMODE FAR *lpValMode)
{
    DWORD scrnSize;

    if (!is_supported_mode(lpValMode->dvmXRes, lpValMode->dvmYRes, lpValMode->dvmBpp))
        return VALMODE_NO_UNKNOWN;
    // do we have enough memory?
    scrnSize = calc_framebuffer_size(lpValMode->dvmXRes, lpValMode->dvmYRes, lpValMode->dvmBpp);
    if (dwVideoMemSize < scrnSize)
        return VALMODE_NO_NOMEM;
    return VALMODE_YES;
}

UINT DLLFUNC SetPalette(WORD wStartIndex, WORD wNumEntries, LPVOID lpPalette)
{
    DIB_SetPaletteExt(wStartIndex, wNumEntries, lpPalette, lpDriverPDevice);
    bga_set_palette(lpPalette, wStartIndex, wNumEntries);
    return 0;
}
