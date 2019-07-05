#include <stdio.h>
#include <stdlib.h>
#include "../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>


int main(int argc, char const* argv[])
{
    Kon kstate;
    if (!KON_Init(&kstate)) {
        fprintf(stderr, "Could not initialize!\n");
        
        return 1;
    }
    printf("? help -h\n");
    KON_Finish(&kstate);
    return 0;
}