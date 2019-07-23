#include <stdio.h>
#include <stdlib.h>
#include "../../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>

int main(int argc, char const* argv[])
{
    KonState* kstate = KON_Init();


    // if (!tb_init(tb_null, tb_null)) {
    //     return 1;
    // }

    // 初始化流
    tb_stream_ref_t istream = tb_stream_init_from_url("../../../../samples/kon/cell.kon");

    // 初始化流
    if (istream) {
        // 打开流
        if (tb_stream_open(istream)) {
            // 初始化读取器
            KonTokenizer* tokenizer = KSON_TokenizerInit(kstate);
            printf("before KSON_TokenizerOpenStream\n");

            // 如果KSON_TokenizerOpen里的最后一个参数owner为true
            // 则KSON_TokenizerClose后，不需要tb_stream_exit(istream)
            bool openRes = KSON_TokenizerOpenStream(tokenizer, istream);
            if (openRes)
            {
                // 初始化xml读取器事件
                tb_size_t event = KON_TOKEN_EOF;

                printf("before KSON_TokenizerNext\n");
                // 遍历所有xml节点元素， 如果返回空事件， 则结束
                while ((event = KSON_TokenizerNext(tokenizer)) && event != KON_TOKEN_EOF)
                {
                    // printf("meet token id %d", tokenizer->TokenKind);
                    KSON_TokenToString(tokenizer);
                    switch (event)
                    {
                    case KON_TOKEN_LITERAL_NUMBER: 
                        {
                            printf("meet num, NumIsPositive: %d, NumBeforeDot %s, NumAfterDot %s ,NumIsPowerPositive %d, NumAfterPower %s\n",
                                tokenizer->NumIsPositive,
                                tb_string_cstr(&tokenizer->NumBeforeDot),
                                tb_string_cstr(&tokenizer->NumAfterDot),
                                tokenizer->NumIsPowerPositive,
                                tb_string_cstr(&tokenizer->NumAfterPower)
                            );
                        }
                        break;
                    default:
                        break;
                    }
                }
                KSON_TokenizerCloseStream(tokenizer);
                // 释放读取器 
                KSON_TokenizerExit(tokenizer);
            }
        }
        
        // 释放流
        tb_stream_exit(istream);
    }

    // tb_exit();
    KON_Finish(kstate);
    return 0;
}