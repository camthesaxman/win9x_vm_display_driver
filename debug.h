
#define PRINTVAR(var) \
    do { \
        debug_print(#var ": "); \
        debug_print(debug_hex((unsigned long)var)); \
        debug_print("\n"); \
    } while (0)

void debug_print(const char* string);
char* debug_hex(unsigned long int n);

#ifndef DEBUG
#define debug_print(string) do {} while (0)
#endif
