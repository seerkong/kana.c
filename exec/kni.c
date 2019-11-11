#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/kana.h"
#include "core/interp/interp.h"
#include "commander.h"


Kana* kana;
int ExecMode;   // 1 file mode, 2 repl mode

Kana* InitKana()
{
    kana = KN_Init();
    if (kana == NULL) {
        fprintf(stderr, "initialize konstate failed!\n");
        exit(1);
    }

    return kana;
}

// void ExitFailure(Kana* kana)
// {
//     KN_Finish(kana);
// }

// void ExitSuccess(Kana* kana)
// {
//     KN_Finish(kana);
// }

// static void Repl(command_t *self) {
//     KN_DEBUG("start repl: enabled\n");
//     ExecMode = 2;
// }

// static void EnableLog(command_t* self) {
//     ENABLE_DEBUG = 1;
// }

// static void EvalFile(command_t *self) {
//     ExecMode = 1;
//     KN_DEBUG("eval file: %s\n", self->arg);
//     KN_EvalFile(kana, self->arg);
// }

// static void PrintHelp(command_t *self)
// {
//     KN_DEBUG("? help -h\n");
// }

// int RunMain(int argc, char **argv)
// {
//     command_t cmd;
//     command_init(&cmd, argv[0], "0.0.1");
//     command_option(&cmd, "-d", "--debug", "enable debug log", EnableLog);
//     command_option(&cmd, "-r", "--repl", "start repl", Repl);
//     // command_option(&cmd, "-f", "--file <arg>", "script file path arg", EvalFile);
//     command_option(&cmd, "-h", "--help [arg]", "help info", PrintHelp);
//     command_parse(&cmd, argc, argv);

//     KN_DEBUG("additional args:\n");
//     for (int i = 0; i < cmd.argc; ++i) {
//         KN_DEBUG("  - '%s'\n", cmd.argv[i]);
//     }
    

//     command_free(&cmd);
//     return KN_TRUE;
// }

// int LoadInitScript()
// {
//     const char* initScriptPath = "/usr/local/etc/kana/init.kl";
//     if ((access(initScriptPath, F_OK)) != -1) {
//         if (access(initScriptPath, R_OK) != -1) {
//             KN_EvalFile(kana, initScriptPath);
//         }
//     }
// }

int main(int argc, char const* argv[])
{
    ExecMode = 1;
    Kana* kana = InitKana();
    
    // ENABLE_DEBUG = 0;
    ENABLE_DEBUG = 1;
    // if (RunMain(argc, argv)) {
    //     ExitFailure(kana);
    // }
    // else {
    //     ExitSuccess(kana);
    // }

    // LoadInitScript();

    if (ExecMode == 1 && argc > 1) {
        KN_EvalFile(kana, argv[1]);
    }
    else {
        KN_EvalFile(kana, "/Users/peacock/lang/kana/kana_nunbox/examples/knative/hello.kl");
    }

    // KonEnv* rootEnv = KN_MakeRootEnv(kana);
    // int codeLen = 2;
    // KnOp codeBlock[2] = {
    //     {.code=OP_ADD, .a=1, .b=2},
    //     {.code=OP_LAND}
    // };

    // KN result = ExecByteCode(kana, rootEnv, codeBlock, codeLen);
    // KN result = ExecByteCode2(kana, rootEnv, codeBlock, codeLen);
    // KN formated = KN_ToFormatString(kana, result, true, 0, "  ");
    // printf("%s\n", KN_StringToCstr(formated));

    return 0;
}
