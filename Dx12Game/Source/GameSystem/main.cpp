#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif//defined(DEBUG) || defined(_DEBUG)

#include "App.h"

/*Unicode�v���O���~���O���f���ɏ��������ڐA���̂���R�[�h��*/
/*�쐬����ꍇ�́umain�ł͂Ȃ�wmain�v���g�p����B          */
int wmain(int argc, wchar_t** argv, wchar_t** evnp)
{
#if defined(DEBUG) || defined(_DEBUG)
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(100);
#endif//defined(DEBUG) || defined(_DEBUG)

    App(1920, 1080).Run();
}

//�f�o�b�O�p����
/* #if�E#endif�ň͂܂ꂽ�����̓��������[�N�����������Ƃ���   */
/* Detected memory leaks!                                    */
/* Dumping objects ->                                        */
/* (148) normal block at 0x000001CBD90A1470, 64 bytes long.  */
/* Data: < > CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD */
/* Object dump complete                                      */
/* �ƕ\������A�����148��ڂɊm�ۂ���64�o�C�g�����������[�N */
/* ���Ă��邱�Ƃ������Ă���B                                */

//_CtrSetBreakAlloc()
/* �f�o�b�O�u���[�N���郁�\�b�h�B                            */
/* ���[�N���������Ă��郁�����m�ۉ񐔂������ɂ��邱�Ƃ�      */
/* ���̌��Ƀf�o�b�O�u���[�N�������Ă����                  */