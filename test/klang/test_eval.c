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

TEST Native_And(void) {
    char* path = "../../../../examples/knative/and.kl";
    // char* path = "~/lang/kanascript/kana-c/examples/knative/and.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Blk(void) {
    char* path = "../../../../examples/knative/blk.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Cond(void) {
    char* path = "../../../../examples/knative/cond.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Do(void) {
    char* path = "../../../../examples/knative/do.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Func(void) {
    char* path = "../../../../examples/knative/func.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_If(void) {
    char* path = "../../../../examples/knative/if.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Lambda(void) {
    char* path = "../../../../examples/knative/lambda.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Letset(void) {
    char* path = "../../../../examples/knative/letset.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Math(void) {
    char* path = "../../../../examples/knative/math.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ(KN_FALSE, result);
    PASS();
}

TEST Native_Or(void) {
    char* path = "../../../../examples/knative/or.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Writeln(void) {
    char* path = "../../../../examples/knative/writeln.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_CallCC(void) {
    char* path = "../../../../examples/knative/callcc.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_For(void) {
    char* path = "../../../../examples/knative/for.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_ForLoopList(void) {
    char* path = "../../../../examples/knative/for-loop-list.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Pipe(void) {
    char* path = "../../../../examples/knative/pipe.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Accessor(void) {
    char* path = "../../../../examples/knative/accessor.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_MkDispatcher(void) {
   char* path = "../../../../examples/knative/mk-dispatcher.kl";
    // char* path = "~/lang/kanascript/kana-c/examples/knative/mk-dispatcher.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_ProtoObj(void) {
//   char* path = "../../../../examples/knative/proto-obj.kl";
     char* path = "~/lang/kanascript/kana-c/examples/knative/proto-obj.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_Shell(void) {
    char* path = "../../../../examples/knative/sh.kl";
    KN_EvalFile(kstate, path);
    PASS();
}

TEST Native_ReturnFromRoot(void) {
    char* path = "../../../../examples/knative/return-from-root.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(42), result, "%d");
    PASS();
}

TEST Native_ReturnFromLambda(void) {
    char* path = "../../../../examples/knative/return-from-lambda.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(48), result, "%d");
    PASS();
}

TEST Native_ReturnFromFunc(void) {
    char* path = "../../../../examples/knative/return-from-func.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(48), result, "%d");
    PASS();
}

TEST Native_ReturnFromBlk(void) {
    char* path = "../../../../examples/knative/return-from-blk.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(42), result, "%d");
    PASS();
}

TEST Native_Quote(void) {
    char* path = "../../../../examples/knative/symbol.kl";
    KN result = KN_EvalFile(kstate, path);
    // ASSERT_EQ_FMT(KN_MAKE_FIXNUM(42), result, "%d");
    PASS();
}

TEST Native_Eval(void) {
    char* path = "../../../../examples/knative/eval.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(4), result, "%d");
    PASS();
}

TEST Native_Apply(void) {
    char* path = "../../../../examples/knative/apply.kl";
    KN result = KN_EvalFile(kstate, path);
    ASSERT_EQ_FMT(KN_MAKE_FIXNUM(30), result, "%d");
    PASS();
}

SUITE(suite) {
   RUN_TEST(Native_Math);
   
   RUN_TEST(Native_Writeln);

   RUN_TEST(Native_And);
   RUN_TEST(Native_Or);

   RUN_TEST(Native_Letset);

   RUN_TEST(Native_If);
   RUN_TEST(Native_Do);
   RUN_TEST(Native_Cond);
   
   RUN_TEST(Native_Blk);
   RUN_TEST(Native_Func);
   RUN_TEST(Native_Lambda);

   RUN_TEST(Native_CallCC);

   RUN_TEST(Native_For);
   RUN_TEST(Native_ForLoopList);
   RUN_TEST(Native_Pipe);

   RUN_TEST(Native_ReturnFromRoot);
   RUN_TEST(Native_ReturnFromLambda);
   RUN_TEST(Native_ReturnFromFunc);
   RUN_TEST(Native_ReturnFromBlk);
   
   
   RUN_TEST(Native_Quote);

   RUN_TEST(Native_Eval);

   RUN_TEST(Native_Apply);
   
   RUN_TEST(Native_Accessor);
   
   RUN_TEST(Native_Shell);

   RUN_TEST(Native_MkDispatcher);

    RUN_TEST(Native_ProtoObj);
}

GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);

    kstate = KN_Init();
    // ENABLE_DEBUG = 1;
    ENABLE_DEBUG = 0;
    RUN_SUITE(suite);

    KN_Finish(kstate);

    GREATEST_MAIN_END();
}
