#include "kn_regex.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/sregex/sregex/sregex.h"

#define REG_MALLOC_FREE_ERR -100


static int
process_string(sre_char *s, size_t len, sre_program_t *prog, sre_int_t *ovector,
    size_t ovecsize, sre_uint_t ncaps)
{
    sre_uint_t                   i, j;
    sre_int_t                    rc;
    sre_char                    *p;
    unsigned                     gen_empty_buf;
    sre_int_t                   *pending_matched;
    sre_pool_t                  *pool;
    sre_vm_pike_ctx_t           *pctx;

    p = malloc(1);
    if (p == NULL) {
        return REG_MALLOC_FREE_ERR;
    }

    pool = sre_create_pool(1024);
    if (pool == NULL) {
        free(p);
        return REG_MALLOC_FREE_ERR;
    }

    // sre_reset_pool(pool);

    pctx = sre_vm_pike_create_ctx(pool, prog, ovector, ovecsize);

    rc = sre_vm_pike_exec(pctx, s, len, 1 /* eof */, NULL);

    
    sre_destroy_pool(pool);
    free(p);
    return rc;
}


KN KN_Regex_Match(KonState* kstate, KN regExpr, KN strToMatch)
{
    const char* regExprStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(regExpr));
    const char* strToMatchCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(strToMatch));
    KonVector* result = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    result->vector = KxVector_Init();


    sre_uint_t           i, n;
    const char          *flags_str = NULL;
    int                 *multi_flags = NULL;
    sre_int_t            err_offset = -1, err_regex_id;
    sre_pool_t          *ppool; /* parser pool */
    sre_pool_t          *cpool; /* compiler pool */
    sre_regex_t         *re;
    sre_program_t       *prog;
    sre_uint_t           ncaps;
    sre_int_t           *ovector;
    size_t               ovecsize;
    sre_char            *s, *p;
    size_t               len;
    unsigned             from_stdin = 0;
    sre_int_t            nregexes = 1;

    ppool = sre_create_pool(1024);
    if (ppool == NULL) {
        // TODO throw error
        return result;
    }

    // ncaps: how many '()' pairs
    re = sre_regex_parse(ppool, (sre_char *) regExprStr, &ncaps,
                             0, &err_offset);
    // dump regex tree
    // sre_regex_dump(re);
    // printf("ncaps: %d\n", ncaps);

    cpool = sre_create_pool(1024);
    if (cpool == NULL) {
        return result;
    }

    prog = sre_regex_compile(cpool, re);
    if (prog == NULL) {
        fprintf(stderr, "failed to compile the regex.\n");
        sre_destroy_pool(ppool);
        sre_destroy_pool(cpool);
        return result;
    }

    sre_destroy_pool(ppool);
    ppool = NULL;
    re = NULL;

    // dump bytecode
    // sre_program_dump(prog);

    ovecsize = 2 * (ncaps + 1) * sizeof(sre_int_t);
    ovector = malloc(ovecsize);
    if (ovector == NULL) {
        return result;
    }


    len = strlen(strToMatchCstr);
    p = (sre_char *) strToMatchCstr;

    s = malloc(len);
    if (s == NULL) {
        free(ovector);
        return result;
    }

    memcpy(s, p, len);

    int rc = process_string(s, len, prog, ovector, ovecsize, ncaps);
    if (rc >= 0) {
        for (i = 0; i < 2 * (ncaps + 1); i += 2) {
            printf(" (%ld, %ld)", (long) ovector[i], (long) ovector[i + 1]);
        
            KonVector* match = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
            match->vector = KxVector_Init();
            KxVector_Push(match->vector, KN_MAKE_FIXNUM(ovector[i]));
            KxVector_Push(match->vector, KN_MAKE_FIXNUM(ovector[i + 1]));

            KxVector_Push(result->vector, match);
        }

        printf("\n");

    }
    else {
        switch (rc) {
        case SRE_AGAIN:
            printf("again\n");
            break;

        case SRE_DECLINED:
            printf("no match\n");
            break;

        case SRE_ERROR:
            printf("error\n");
            break;

        default:
            printf("unknown (%d)\n", (int) rc);
            break;
        }
    }



    free(s);

    sre_destroy_pool(cpool);
    prog = NULL;
    cpool = NULL;
    free(ovector);
    
    return result;
}

KonAccessor* KN_Regex_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "", NULL);

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "match",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_Regex_Match, 2, 0, 0),
        "r",
        NULL
    );

    return slot;
}
