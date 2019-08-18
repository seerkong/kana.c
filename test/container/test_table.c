#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "tbox/tbox.h"

extern SUITE(test_hashtable);

extern SUITE(test_hashtable_iter);


GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    if (!tb_init(tb_null, tb_null)) {
        exit(1);
    }
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);
    
    RUN_SUITE(test_hashtable);
    RUN_SUITE(test_hashtable_iter);

    GREATEST_MAIN_END();
    tb_exit();
}
