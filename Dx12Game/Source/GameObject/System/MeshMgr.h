#pragma once
#include <unordered_map>
#include <d3d12.h>
#include "Resource/Mesh.h"
#include "MeshType.h"

namespace Sys
{
	class MeshMgr
	{
		MeshMgr();
		~MeshMgr();
		static MeshMgr* singleton;	// シングルトン

	public:
		// Public Method

		/// <summary> MeshMgrの機能を使用開始 </summary>
		static void OnInit();
		/// <summary> MeshMgrの機能を使用終了 </summary>
		static void OnTerm();
		void Init();
		void LoadMesh(const Res::MeshType& _Type, const char* _Path);
		static const Mesh& GetMesh(const Res::MeshType& _Type) { return *singleton->meshes[_Type]; }

	private:
		// Private Variable

		std::unordered_map<Res::MeshType, Mesh*>meshes;	// 読み込んだメッシュデータ
		Res::MeshType meshType;
	};
}

// 〇モデルのレンダリングには
// ・モデルのデータ
// ・マテリアル(渡さなかったらできればランバートで表示したい今はとりあえずテクスチャ)


// 〇レンダーコンポーネント
// Dx12側にデータを渡して描画できるようにする


// 〇モデルデータ管理クラス
// ・モデル読み込み機能
// ・モデルデータコンテナ(モデルパス、モデルデータ)
// ・同じモデルのロード防止機能

// 〇マテリアル管理クラス
// ・マテリアル構造体
// ・テクスチャ読み込み機能
// ・テクスチャコンテナ(テクスチャパス、テクスチャ)
// ・マテリアルコンテナ(マテリアルに使用されているテクスチャ番号)
// ・同じテクスチャのロード防止機能