
#include "./number_utils.h"

// int to string
void itoa(int a, char buff[], int n)
{
    char *beg = buff;
    int sign;
    if ((sign = a) < 0) {
        a = -a;
    }
 
    do {
        *buff++ = '0' + a % n;
    } while ((a /= n) > 0);
 
    if (sign < 0) {
        *buff++ = '-';
    }
 
    *buff = '\0';
 
    char *end = buff - 1;
    while (beg < end) {
        char tmp = *beg;
        *beg++ = *end;
        *end-- = tmp;
    }

}

int atoi(const char* src)
{
    int s = 0;
    int isMinus = 0;

    while (*src == ' ') {
        src++; 
    }

    if (*src == '+' || *src == '-') {
        if (*src == '-') {
            isMinus = 1;
        }
        src++;
    }
    // return exception magic number
    else if (*src < '0' || *src > '9') {
        s = 2147483647;
        return s;
    }

    while (*src != '\0' && *src >= '0' && *src <= '9') {
        s = s * 10 + (*src - '0');
        src++;
    }
    return s * (isMinus ? -1 : 1);
}

char* double_to_str(double val, int min_precision, char* buf)
{
    char *cur, *end;
    
    sprintf(buf, "%.6lf", val);
    if (min_precision < 6) {
        cur = buf + strlen(buf) - 1;
        end = cur - 6 + min_precision; 
        while ((cur > end) && (*cur == '0')) {
            *cur = '\0';
            cur--;
        }
    }
    
    return buf;
}