#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>

// ANSI-färgkoder
static const char *PR_BLACK   = "\033[0;30m";
static const char *PR_RED     = "\033[0;31m";
static const char *PR_GREEN   = "\033[0;32m";
static const char *PR_YELLOW  = "\033[0;33m";
static const char *PR_BLUE    = "\033[0;34m";
static const char *PR_MAGENTA = "\033[0;35m";
static const char *PR_CYAN    = "\033[0;36m";
static const char *PR_WHITE   = "\033[0;37m";
static const char *PR_RESET   = "\033[0m";

// Generellt makro för färgad printf
#define PR_PRINTF(color, fmt, ...) \
    printf("%s" fmt "%s\n", color, ##__VA_ARGS__, PR_RESET)

// Specifika makron för olika moduler
#define PR_MAIN(fmt, ...)         PR_PRINTF(PR_YELLOW,  "Main: " fmt,         ##__VA_ARGS__)
#define PR_WIFI(fmt, ...)         PR_PRINTF(PR_MAGENTA, "Wi-Fi: " fmt,         ##__VA_ARGS__)
#define PR_NFC(fmt, ...)          PR_PRINTF(PR_CYAN,    "NFC-Client: " fmt,          ##__VA_ARGS__)
#define PR_SERVER(fmt, ...)       PR_PRINTF(PR_RED,     "Server: " fmt,       ##__VA_ARGS__)
#define PR_CLIENT(fmt, ...)       PR_PRINTF(PR_BLUE,    "Client: " fmt,       ##__VA_ARGS__)
// Lägg till fler efter behov...

#endif // PRINTER_H