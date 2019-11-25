#include "module.h"

void KN_DirAccessorImportNative(Kana* kana, KonAccessor* slot, NativeExportConf conf)
{
    NativeExportItem* items = conf.items;
    for (int i = 0; i < conf.len; i++) {
        NativeExportItem item = items[i];
        KN value;
        switch (item.type) {
            case KN_NATIVE_EXPORT_PROC: {
                value = MakeNativeProcedure(
                    kana,
                    item.proc.type,
                    item.proc.funcRef,
                    item.proc.paramNum,
                    item.proc.hasVAList,
                    item.proc.hasVAMap
                );
            }
        }

        KN_DirAccessorPutKeyValue(
            kana,
            KON_2_KN(slot),
            item.name,
            value,
            "r",
            NULL
        );
    }
}

KN KonNumber_Import(Kana* kana, KonEnv* env)
{
    NativeExportConf conf = Kmod_Number_Export(kana);
    KN_EnvImportNative(kana, env, conf);

    KonAccessor* slot = KN_MakeDirAccessor(kana, "dr", NULL);
    KN_DirAccessorImportNative(kana, slot, conf);
    return KON_2_KN(slot);
}

KN KN_Module_Export(Kana* kana, KonEnv* env)
{
    KonAccessor* module = (KonAccessor*)KN_MakeDirAccessor(kana, "drw", NULL);
    
    KN_DirAccessorPutKeyProperty(
        kana,
        KON_2_KN(module),
        "number",
        (KN)KonNumber_Import(kana, env)
    );
    return KON_2_KN(module);
}
