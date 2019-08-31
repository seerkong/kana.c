#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/kon.h"
#include "commander.h"


KonState* kstate;
int ExecMode;   // 1 file mode, 2 repl mode

KonState* InitKonState()
{
    kstate = KN_Init();
    if (kstate == NULL) {
        fprintf(stderr, "initialize konstate failed!\n");
        exit(1);
    }

    return kstate;
}

void ExitFailure(KonState* kstate)
{
    KN_Finish(kstate);
}

void ExitSuccess(KonState* kstate)
{
    KN_Finish(kstate);
}

static void Repl(command_t *self) {
    KN_DEBUG("start repl: enabled\n");
    ExecMode = 2;
}

static void EnableLog(command_t* self) {
    ENABLE_DEBUG = 1;
}

static void EvalFile(command_t *self) {
    ExecMode = 1;
    KN_DEBUG("eval file: %s\n", self->arg);
    KN_EvalFile(kstate, self->arg);
}

static void PrintHelp(command_t *self)
{
    KN_DEBUG("? help -h\n");
}

int RunMain(int argc, char **argv)
{
    command_t cmd;
    command_init(&cmd, argv[0], "0.0.1");
    command_option(&cmd, "-d", "--debug", "enable debug log", EnableLog);
    command_option(&cmd, "-r", "--repl", "start repl", Repl);
    // command_option(&cmd, "-f", "--file <arg>", "script file path arg", EvalFile);
    command_option(&cmd, "-h", "--help [arg]", "help info", PrintHelp);
    command_parse(&cmd, argc, argv);

    KN_DEBUG("additional args:\n");
    for (int i = 0; i < cmd.argc; ++i) {
        KN_DEBUG("  - '%s'\n", cmd.argv[i]);
    }
    

    command_free(&cmd);
    return KN_TRUE;
}

int LoadInitScript()
{
    const char* initScriptPath = "/usr/local/etc/kunu/init.kl";
    if ((access(initScriptPath, F_OK)) != -1) {
        if (access(initScriptPath, R_OK) != -1) {
            KN_EvalFile(kstate, initScriptPath);
        }
    }
}

int main(int argc, char const* argv[])
{
    ExecMode = 1;
    KonState* kstate = InitKonState();
    
    ENABLE_DEBUG = 0;
    // ENABLE_DEBUG = 1;
    // if (RunMain(argc, argv)) {
    //     ExitFailure(kstate);
    // }
    // else {
    //     ExitSuccess(kstate);
    // }

    LoadInitScript();

    if (ExecMode == 1 && argc > 1) {
        KN_EvalFile(kstate, argv[1]);
    }
    else {
        // KN_EvalFile(kstate, "/Users/kongweixian/lang/kunu/kunu.c/examples/knative/mk-dispatcher.kl");
    }
    
    return 0;
}
