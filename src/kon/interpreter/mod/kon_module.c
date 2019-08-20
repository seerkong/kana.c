#include "kon_number.h"
#include "kon_string.h"
#include "kon_list.h"
#include "kon_vector.h"
#include "kon_table.h"
#include "kon_cell.h"
#include "kon_accessor.h"

#include "kon_module.h"

KN KonModule_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* module = (KonAccessor*)KON_MakeDirAccessor(kstate, "drw", NULL);
    
    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "number",
        KonNumber_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "string",
        KonString_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "list",
        KonList_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "vector",
        KonVector_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "table",
        KonTable_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "cell",
        KonCell_Export(kstate, env)
    );

    KON_DirAccessorPutKeyProperty(
        kstate,
        module,
        "accessor",
        KonAccessor_Export(kstate, env)
    );

    return (KN)module;
}
