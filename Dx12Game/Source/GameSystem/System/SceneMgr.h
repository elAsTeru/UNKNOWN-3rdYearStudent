#pragma once
#include <unordered_map>
//参考
//https://qiita.com/o_joh4869/items/a0d23fb26897762c1990

namespace Scene
{
	class Base;

	//SceneMgr Class
	class Mgr
	{
		Mgr();
		~Mgr();
		Mgr(const Mgr&) = delete;
		Mgr& operator = (const Mgr&) = delete;
		static Mgr* singleton;

	public:
		static void OnInit();
		static void OnTerm();

		static bool Update();

		// 未実装
		//std::unordered_map< tag, std::vector<SceneBase*>> SceneData:

		/// <summary>
		/// シーンを読み込み、読み込みが完了したらtrueを返却
		/// </summary>
		static void LoadScene(std::string _Name);
		static void Quit() { singleton->isQuit = true; }

		///// <summary>
		///// シーンの読み込み終了後、任意のタイミングでシーンを切り替える
		///// </summary>
		//static void SwapScene();
	private:
		Base* nowScene;
		Base* nextScene;
		bool isQuit;
	};
}