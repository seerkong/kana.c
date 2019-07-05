#include <stdio.h>
#include <stdlib.h>
#include "../../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>

int main(int argc, char const* argv[])
{
    if (!tb_init(tb_null, tb_null)) return 1;

    // TODO

    tb_exit();
    return 0;
}