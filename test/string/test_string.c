#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "core/kon.h"
#include "../../lib/tbox/tbox.h"

KonState* kstate;
SUITE(suite);

TEST NewDelete(void) {
    KxStringBuffer* buff = KxStringBuffer_New();
    KxStringBuffer_Destroy(buff);
    PASS();
}

TEST Prepend(void) {
    KxStringBuffer* buff = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(buff, "abc");
    KxStringBuffer_NPrependCstr(buff, "pt", 2);
    printf("%s\n", KxStringBuffer_Cstr(buff));
    PASS();
}

SUITE(suite) {
    RUN_TEST(NewDelete);
    RUN_TEST(Prepend);
}

GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    if (!tb_init(tb_null, tb_null)) {
        exit(1);
    }
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);

    RUN_SUITE(suite);


    GREATEST_MAIN_END();
    tb_exit();
}
