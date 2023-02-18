#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif//defined(DEBUG) || defined(_DEBUG)

#include "App.h"

/*Unicodeプログラミングモデルに準拠した移植性のあるコードを*/
/*作成する場合は「mainではなくwmain」を使用する。          */
int wmain(int argc, wchar_t** argv, wchar_t** evnp)
{
#if defined(DEBUG) || defined(_DEBUG)
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(100);
#endif//defined(DEBUG) || defined(_DEBUG)

    App(1920, 1080).Run();
}

//デバッグ用処理
/* #if・#endifで囲まれた部分はメモリリークが発生したときに   */
/* Detected memory leaks!                                    */
/* Dumping objects ->                                        */
/* (148) normal block at 0x000001CBD90A1470, 64 bytes long.  */
/* Data: < > CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD */
/* Object dump complete                                      */
/* と表示され、これは148回目に確保した64バイトメモリがリーク */
/* していることを示している。                                */

//_CtrSetBreakAlloc()
/* デバッグブレークするメソッド。                            */
/* リークが発生しているメモリ確保回数を引数にすることで      */
/* その個所にデバッグブレークをかけてくれる                  */