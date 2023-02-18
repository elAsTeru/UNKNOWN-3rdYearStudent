//includes
#include "App.h"
#include "Window.h"
#include "Timer.h"
#include "GameObjectMgr.h"
#include "SceneMgr.h"
#include "DX12Wrapper.h"
#include "MeshMgr.h"
#include "MaterialMgr.h"
#include "ColliderMgr.h"
#include "Score.h"
#include "Sound.h"
#include "InputMgr.h"
#include "EffekseerMgr.h"

// テスト用
#include "LineRes.h"

App::App(uint32_t _Width, uint32_t _Height):
	width(_Width),
	height(_Height)
{
	
}

App::~App()
{
}

void App::Run()
{
	// 正常に初期化できたらメインループを実行
	if (Init())
	{ MainLoop(); }

	Term();
}

bool App::Init()
{
	// Windowクラスの初期化
	window = new MySys::Window(width, height);
	if(!window->Init())
	{ return false; }

	if (!MyDX::Dx12Wrapper::OnInit(window->GetHandle(), width, height)) { return false; }	// ライブラリ初期化(D3D12の初期化に入る)
	MySys::Timer::OnInit();																	// デルタタイム初期化
	MySys::ColliderMgr::OnInit();
	GameObject::Mgr::OnInit();																// ゲームオブジェクトの初期化
	MySys::Res::MeshMgr::OnInit();															// メッシュ管理の初期化
	MySys::Res::MaterialMgr::OnInit();														// マテリアル管理の初期化
	MyObj::Sound::OnInit();
	Effect::EffekseerMgr::OnInit();
	Scene::Mgr::OnInit();																	// シーン初期化
	MyObj::Score::OnInit();
	Input::Mgr::OnInit();																	// TK12::XInputの初期化

	//正常終了
	return true;
}

void App::MainLoop()
{
	MSG msg{};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			MySys::Timer::Update();
			MyDX::Dx12Wrapper::RenderBefore();
			MyDX::Dx12Wrapper::RenderBegin();
			MyDX::Dx12Wrapper::Render();						// 描画処理

			Effect::EffekseerMgr::SyncronizeEffekseerCamera();
			Effect::EffekseerMgr::Draw();

			MyDX::Dx12Wrapper::RenderEnd();
			Input::Mgr::Update();								// 入力状態更新
			if (!Scene::Mgr::Update())							// シーン更新
			{
				break;	// 処理終了
			}
			GameObject::Mgr::Update();							// オブジェクトの更新
			MySys::ColliderMgr::Update();						// 当たり判定の更新
		}
	}
}

void App::Term()
{
	Input::Mgr::OnTerm();					// TK12::XInput終了
	MyObj::Score::OnTerm();
	Scene::Mgr::OnTerm();					// シーン終了
	Effect::EffekseerMgr::OnTerm();
	MyObj::Sound::OnTerm();
	MySys::Res::MaterialMgr::OnTerm();		// マテリアル管理終了
	MySys::Res::MeshMgr::OnTerm();			// メッシュ管理終了
	GameObject::Mgr::OnTerm();				// ゲームオブジェクト管理の終了		// 解放エラー
	MySys::ColliderMgr::OnTerm();
	MySys::Timer::OnTerm();					// デルタタイム終了

	MyDX::Dx12Wrapper::OnTerm();			// ライブラリ終了
	
	delete window;							// ウィンドウ終了
	window = nullptr;
}