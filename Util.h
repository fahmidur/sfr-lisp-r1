#ifndef __SFR_UTIL_H
#define __SFR_UTIL_H

#include <stdio.h>
#include <stdlib.h>

// Not guaranteed to be unique for all strings.
// This is not a cryptographically strong hash.
size_t Util_cstr_hash_djb2(char* str);

size_t Util_sizet_max(size_t a, size_t b);

void Util_heading1(char isbeg, char* str);

#endif
