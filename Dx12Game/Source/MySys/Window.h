#include <Windows.h>
#include <cstdint>

namespace MySys
{
	class Window final
	{
	public:
		Window(uint32_t _Width, uint32_t _Height);
		~Window();
		bool Init();
		HWND GetHandle() { return hWnd; }

	private:

		//Private Variable
		HINSTANCE	hInst;	// インスタンスハンドル
		HWND		hWnd;	// ウィンドウハンドル
		UINT32		width;	// ウィンドウ幅
		UINT32		height;	// ウィンドウ高

		// Private Method
		static LRESULT CALLBACK WndProc(HWND _HWnd, UINT _Msg, WPARAM _Wp, LPARAM _Lp);
	};
}