#ifndef NUMBER_UTILS_H
#define NUMBER_UTILS_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void itoa(int a, char str[], int n);
int atoi(const char* src);

char* double_to_str(double val, int min_precision, char* buf);

#endif