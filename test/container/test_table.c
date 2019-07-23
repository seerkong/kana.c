#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include <tbox/tbox.h>

extern SUITE(test_hashtable);


GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);
    
    RUN_SUITE(test_hashtable);

    GREATEST_MAIN_END();
}
