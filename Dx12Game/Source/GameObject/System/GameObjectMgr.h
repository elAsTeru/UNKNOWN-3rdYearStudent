#pragma once
#include <unordered_map>
#include <vector>
#include "Tag.h"
#include "MeshType.h"

namespace GameObject
{
	class Base;

	class Mgr
	{
		// シングルトン
		Mgr();
		~Mgr();
		static Mgr* singleton;

	public:
		//Public Methods
		static void OnInit();	// 一度のみ初期化
		static void OnTerm();	// 終了
		static void Update();	// 更新
		static void Draw();		// 描画

		/// <summary>
		/// 対象のTag内のアクティブなオブジェクト数を返却する
		/// </summary>
		static int GetActiveNum(const Tag _Tag);

		/// <summary>
		/// 名前が一致する有効中のゲームオブジェクトを取得
		/// </summary>
		static Base* FindActiveObj(const std::string _GameObjName);

		/// <summary>
		/// 名前が一致する無効中のゲームオブジェクトを取得
		/// </summary>
		static Base* FindDeactiveObj(const std::string _GameObjName);

		/// <summary>
		///  全ての空間から名前が一致するゲームオブジェクトを取得
		/// </summary>
		static Base* Find(const std::string _GameObjName);

		/// <summary>
		/// 全てのゲームオブジェクトを無効化
		/// </summary>
		static void DeactivateAll();

		/// <summary>
		/// 名前が一致するゲームオブジェクトを全て無効化
		/// </summary>
		/// <param name="_GameObjName"></param>
		static void DeactivateSomeAll(const std::string _GameObjName);

		/// <summary>
		/// オブジェクトを追加
		/// </summary>
		static void AddObject(Base* _Object);
	private:
		// Private Variable
		std::unordered_map < Tag, std::vector<Base*>> objects;		// タグ付きのオブジェクトリスト
		std::vector <Base*> pendingData;							// 追加や削除の保留リスト
		
		// Private Method
		static void RemoveObject(Base* _Object);					// オブジェクトを削除
		static const void RemoveAllObject();						// 全オブジェクトを削除

		template<class T> static void CreatePool(uint16_t _Size);
	};
}