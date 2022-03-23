typedef struct tagDISPLAYINFO {
        WORD  diHdrSize;               /* Size in bytes of the DISPLAYINFO structure. */
        WORD  diInfoFlags;             /* Informational flags */
        DWORD diDevNodeHandle;         /* Handle used to access information in the Registry about the display subsystem and passed to configuration management function calls. */
        BYTE  diDriverName[16];        /* Address of a null-terminated string containing the name of the driver. */
        WORD  diXRes;                  /* Width in pixels, of the display. */
        WORD  diYRes;                  /* Height in pixels, of the display. */
        WORD  diDPI;                   /* Number of dots per inch of the display. */
        BYTE  diPlanes;                /* Number of color planes. */
        BYTE  diBpp;                   /* Number of adjacent color bits for each pixel. */
        WORD  diRefreshRateMax;        /* Maximum refresh rate. */
        WORD  diRefreshRateMin;        /* Minimum refresh rate. */
        WORD  diLowHorz;               /* Horizontal sync polarity ranges. */
        WORD  diHighHorz;              /* Horizontal sync polarity ranges. */
        WORD  diLowVert;               /* Vertical sync polarity ranges. */
        WORD  diHighVert;              /* Vertical sync polarity ranges. */
        DWORD diMonitorDevNodeHandle;  /* Handle used to access Registry information about the monitor. */
        BYTE  diHorzSyncPolarity;      /* Sync polarity monitor settings. */
        BYTE  diVertSyncPolarity;      /* Sync polarity monitor settings. */
} DISPLAYINFO;

/*
 * VDD_DRIVER_REGISTER
 *
 * Call With
 * EAX = VDD_DRIVER_REGISTER equate (80H)
 * EBX = Magic Number
 * ECX = Total bytes used by the visible Windows screen
 * EDX = 0 if the mini-VDD supports VGA 4 plane virtualization in a window or -1 if the display requires that VGA 4 plane graphics programs run full-screen and can not be windowed.
 * ES:DI Pointer to ResetHiResMode which is a required Display driver function which will be called by the VDD in order to reenable the display hardware for Windows high resolution mode after returning from a full-screen MS-DOS virtual machine.
 *
 * Return Values
 * EAX = Total bytes of memory used by the screen (as passed in ECX to the function) plus the number of bytes used by the VDD for virtualizing VGA 4 plane graphics in a window.
 * EAX = VDD_DRIVER_REGISTER if you are running with a Windows 3.1 VDD and not the Windows 95 VDD. This means that the user somehow has installed the wrong VDD.
 *
 * Remarks
 * This routine should be called immediately after the Windows high resolution (hi-res) mode has been set by the display driver. The mini-VDD's function GET_VDD_BANK is called by this routine.
 * ResetHiResMode is a required display driver entry point which must be placed in a FIXED code segment in the display driver. It is called by the Windows 95 VDD in order to place the screen back into Windows hi-res mode after a user presses ALT-TAB or ALT-ENTER to switch away from a full screen MS-DOS seesion. ResetHiResMode typically is a simplified Enable call which reinitializes the hardware including the palette DAC and hardware cursor so that Windows will run again.
 * To support Far East DBCS, the display hardware and mini-VDD should be able to support VGA graphics in a window. The VDD supports VGA graphics in a window by allocating a 32K or 64K block of video memory right below the visible Windows screen. The mini-VDD then sets access to this memory (usually by banking it into physical address A000h) and allows planar access to this memory while maintaining the packed pixel Windows screen visibly. However, some hardware cannot simultaneously have the VGA data pathway and the packed-pixel hi-res pathway running. If your hardware cannot, set EDX to -1 and the VDD will not attempt VGA virtualization in a window.
 * This function calls the mini-VDD function GET_VDD_BANK.
 *
 * See also GET_VDD_BANK
 */
#define VDD_DRIVER_REGISTER 0x80

/*
 * VDD_DRIVER_UNREGISTER
 *
 * Call With
 * EAX = VDD_DRIVER_UNREGISTER equate (81H)
 * EBX = Magic Number
 * All other registers can contain private communication information which is passed directly to the mini-VDD function DISPLAY_DRIVER_DISABLING.
 *
 * Return Values
 * Nothing destroyed.
 *
 * Remarks
 * This function does an Interrupt 10h, function 0 call to set the hardware into text mode 3. It should be called during the display driver's Disable routine, right before the driver leaves Windows hi-res mode and returns to VGA mode.
 * This function calls the mini-VDD function DISPLAY_DRIVER_DISABLING which allows the mini-VDD to release I/O port trapping or memory traps that it may have set while running Windows.
 * This function calls the mini-VDD function DISPLAY_DRIVER_DISABLING.
 *
 * See also DISPLAY_DRIVER_DISABLING
 */
#define VDD_DRIVER_UNREGISTER 0x81

/*
 * VDD_REGISTER_DISPLAY_DRIVER_INFO
 * Call With
 * EAX = VDD_REGISTER_DISPLAY_DRIVER_INFO equate (83H)
 * EBX = Magic Number
 * All other registers can contain private communication information which will be passed directly to the mini-VDD function REGISTER_DISPLAY_DRIVER.
 *
 * Return Values
 * Whatever the mini-VDD and the display driver have arranged between themselves.
 *
 * Remarks
 * This routine sets up the close communication between the display driver and the mini-VDD. This function is sometimes called before and after the display driver sets the hardware into the Windows hi-res state. You could setup sub-functions of this call in the ECX register and call each of the sub-functions any time during display driver initialization depending on the needs of the display subsystem.
 * One possible use for this function would be for the display driver to pass a Selector:Offset of one or more flags to the mini-VDD. Suppose that the display hardware cannot change its hardware state in the middle of a memory to screen bitblt and that the VDD has to wait until this operation is finished before making a hardware state change such as running a windowed 4 Plane VGA application. The display driver could pass the address of an "InSrcBLT" flag to the mini-VDD in the ES:DI registers. The mini-VDD would then convert this Selector:Offset address to a linear address by means of the VMM function Map_Flat. Then the mini-VDD would pass this linear address to the Main VDD function VDD_Set_Sleep_Flag_Addr.
 * Another possible use for this function would be for the display driver to pass the address of a non-VFLATD screen selector to the mini-VDD. The mini-VDD could use this selector to access Memory Mapped Registers or the video RAM as a flat linear frame buffer. In order for windowed VGA virtualization to correctly take place, the Main VDD has to know about all ways that the display driver will access the video hardware (for example, via a hardware BLTer using I/O ports, via a hardware BLTer using memory mapped registers, via a flat linear selector that the display has allocated independently from VFLATD, or via a VFLATD selector). Thus, the display driver could pass down the selector of its memory mapped or flat linear selector to the mini-VDD via this function. The mini-VDD would then call the Main VDD function VDD_Register_Extra_Screen_Selector in order to register this selector.
 * Yet another possible use for this function would be to allow the mini-VDD to partially or fully place the video hardware into hi-res mode. It may be desirable to do this if port trapping by the VMM is of concern as it is in time-critical programming of the video hardware.
 * This function calls the mini-VDD function REGISTER_DISPLAY_DRIVER.
 *
 * See also VDD_Set_Sleep_Flag_Addr, REGISTER_DISPLAY_DRIVER, VDD_Register_Extra_Screen_Selector
 */
#define VDD_REGISTER_DISPLAY_DRIVER_INFO 0x83

/*
 * VDD_GET_DISPLAY_CONFIG
 * Call With
 * EAX = VDD_GET_DISPLAY_CONFIG equate (85H)
 * EBX = Magic Number
 * ECX = Size of DISPLAYINFO structure that is being passed.
 * ES:EDI = Pointer to DISPLAYINFO structure to fill (see MINIVDD.INC)
 *
 * Return Values
 * EAX = Return Code (0 for success, -1 for failure).
 *
 * Remarks
 * This is the display Plug & Play interface for Windows 95. This call returns a DISPLAYINFO data structure to the caller. The caller should allocate a DISPLAYINFO structure and then pass its address in the ES:EDI registers when making this call.
 * No mini-VDD functions are called during this function.
 */
#define VDD_GET_DISPLAY_CONFIG 0x85
