#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "core/kon.h"

KonState* kstate;
SUITE(suite);

TEST Tokenize_Cell(void) {
        
    char* filePathOrigin = "~/lang/konscript/kon-c/examples/kon/cell.kon";
    KxStringBuffer* sb = KN_ReadFileContent(filePathOrigin);
    printf("%s\n", KxStringBuffer_Cstr(sb));
    KxStringBuffer_Clear(sb);


    PASS();
}


SUITE(suite) {
    RUN_TEST(Tokenize_Cell);

}

GREATEST_MAIN_DEFS();


int main(int argc, char const* argv[])
{
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);
    kstate = KN_Init();
    RUN_SUITE(suite);

    KN_Finish(kstate);
    GREATEST_MAIN_END();

    return 0;
}
