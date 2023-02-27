#include "Window.h"
#include <DirectXTK12/Keyboard.h>

namespace
{
	const auto ClassName = TEXT("WindowClass");	// ウィンドウクラス名
}

namespace Sys
{
	Window::Window(uint32_t _Width, uint32_t _Height) :
		hWnd(nullptr),
		hInst(nullptr),
		width(_Width),
		height(_Height)
	{}

	Window::~Window()
	{
		// ウィンドウの登録解除
		if (hInst)
		{
			UnregisterClass(ClassName, hInst);
		}
		hInst = nullptr;
		hWnd = nullptr;
	}

	bool Window::Init()
	{
		// インスタンスハンドル取得
		auto hInst = GetModuleHandle(nullptr);
		if (!hInst)
		{
			return false;
		}

		// ウィンドウの設定
		WNDCLASSEX wc{};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
		wc.hCursor = LoadCursor(hInst, IDC_ARROW);
		wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = ClassName;
		wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

		// ウィンドウ登録
		if (!RegisterClassEx(&wc))
		{
			return false;
		}

		// インスタンスハンドル取得
		hInst = hInst;

		// ウィンドウサイズの設定
		RECT rc{};
		rc.right = static_cast<LONG>(width);
		rc.bottom = static_cast<LONG>(height);

		// ウィンドウサイズの調整
		auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		AdjustWindowRect(&rc, style, FALSE);

		// ウィンドウ生成
		hWnd = CreateWindowEx
		(
			0,
			ClassName,
			TEXT("UNKNOWN"),
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			nullptr,
			nullptr,
			hInst,
			nullptr
		);

		if (!hWnd)
		{
			return false;
		}

		// ウィンドウ表示
		ShowWindow(hWnd, SW_SHOWNORMAL);
		// ウィンドウ更新
		UpdateWindow(hWnd);
		// ウィンドウにフォーカスを設定
		SetFocus(hWnd);

		// 正常終了
		return true;
	}

	//ウィンドウプロシージャ
	LRESULT CALLBACK Window::WndProc(HWND _HWnd, UINT _Msg, WPARAM _Wp, LPARAM _Lp)
	{
		switch (_Msg)
		{
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
			DirectX::Keyboard::ProcessMessage(_Msg, _Wp, _Lp);
			break;
		case WM_SYSKEYDOWN:
			if (_Wp == VK_RETURN && (_Lp & 0x60000000) == 0x20000000)
			{
				// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
				// ALT + ENTER ホットキーによるフルスクリーントルグを実装する場所
			}
			DirectX::Keyboard::ProcessMessage(_Msg, _Wp, _Lp);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			DirectX::Keyboard::ProcessMessage(_Msg, _Wp, _Lp);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
		return DefWindowProc(_HWnd, _Msg, _Wp, _Lp);
	}
}