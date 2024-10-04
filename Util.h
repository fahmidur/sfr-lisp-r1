#ifndef __SFR_UTIL_H
#define __SFR_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void dbg_printf(char* fmt, ...);

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Not guaranteed to be unique for all strings.
// This is not a cryptographically strong hash.
size_t Util_cstr_hash_djb2(char* str);

size_t Util_sizet_max(size_t a, size_t b);

void Util_cstr_print(char* cstr);

void Util_heading1(char isbeg, char* str);

void Util_indent(int size);


// See https://en.wikipedia.org/wiki/ANSI_escape_code
#define VT_COLOR_BLACK_FG 30
#define VT_COLOR_BLACK_BG 40
#define VT_COLOR_RED_FG 31
#define VT_COLOR_RED_BG 41
#define VT_COLOR_GREEN_FG 32
#define VT_COLOR_GREEN_BG 42
#define VT_COLOR_YELLOW_FG 33
#define VT_COLOR_YELLOW_BG 43
#define VT_COLOR_BLUE_FG 34
#define VT_COLOR_BLUE_BG 44
#define VT_COLOR_MAGENTA_FG 35
#define VT_COLOR_MAGENTA_BG 45
#define VT_COLOR_CYAN_FG 36
#define VT_COLOR_CYAN_BG 46
#define VT_COLOR_WHITE_FG 37
#define VT_COLOR_WHITE_BG 47
#define VT_COLOR_BRIGHT_BLACK_GRAY_FG 90
#define VT_COLOR_BRIGHT_BLACK_GRAY_BG 100
#define VT_COLOR_BRIGHT_RED_FG 91
#define VT_COLOR_BRIGHT_RED_BG 101
#define VT_COLOR_BRIGHT_GREEN_FG 92
#define VT_COLOR_BRIGHT_GREEN_BG 102
#define VT_COLOR_BRIGHT_YELLOW_FG 93
#define VT_COLOR_BRIGHT_YELLOW_BG 103
#define VT_COLOR_BRIGHT_BLUE_FG 94
#define VT_COLOR_BRIGHT_BLUE_BG 104
#define VT_COLOR_BRIGHT_MAGENTA_FG 95
#define VT_COLOR_BRIGHT_MAGENTA_BG 105
#define VT_COLOR_BRIGHT_CYAN_FG 96
#define VT_COLOR_BRIGHT_CYAN_BG 106
#define VT_COLOR_BRIGHT_WHITE_FG 97
#define VT_COLOR_BRIGHT_WHITE_BG 107
#define VT_RESET 0

void Util_vt_set(int code);

#endif
