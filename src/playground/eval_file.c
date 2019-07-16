#include <stdio.h>
#include <stdlib.h>
#include "../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>

int main(int argc, char const* argv[])
{
    KonState kstate;
    KON_Init(&kstate);
    // if (!KON_Init(&kstate)) {
    //     fprintf(stderr, "Could not initialize!\n");

    //     return 1;
    // }

    if (!tb_init(tb_null, tb_null)) {
        return 1;
    }

    char* path;// = "~/lang/konscript/kon-c/samples/kon/native/writeln.kon";

    
    if (argc > 1) {
        path = argv[1];
    }
    else {
        printf("need file path\n");
//        exit(1);
        path = "~/lang/konscript/kon-c/samples/kon/native/letset.kon";
    }
    
    KON_EvalFile(&kstate, path);

    tb_exit();
//    KON_Finish(&kstate);
    return 0;
}
