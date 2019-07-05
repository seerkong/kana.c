#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <tbox/tbox.h>

#include "prefix.h"
#include "utils/number_utils.h"



int KON_Init(Kon* kstate)
{
    kon_debug("offsetof(struct KonStruct, value) %lu", offsetof(struct KonStruct, Value));
    // if (!tb_init(tb_null, tb_null)) {
    //     return 1;
    // }
    return 0;
}


int KON_Finish(Kon* kstate)
{
    // exit tbox
    // tb_exit();
    return 0;
}
