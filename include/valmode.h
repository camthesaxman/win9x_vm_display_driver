typedef struct tagDISPVALMODE {
    WORD dvmSize;  // Size of this structure
    WORD dvmBpp;   // Color bit depth
    WORD dvmXRes;  // X resolution
    WORD dvmYRes;  // Y resolution
} DISPVALMODE;

// ValidateMode return values
#define VALMODE_YES         0  // mode is valid
#define VALMODE_NO_WRONGDRV 1  // wrong driver for display adapter
#define VALMODE_NO_NOMEM    2  // not enough memory for settings
#define VALMODE_NO_NODAC    3  // DAC can't handle colors
#define VALMODE_NO_UNKNOWN  4  // unspecified problem
