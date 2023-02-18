#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "MeshType.h"

namespace MyRes
{
	enum class MeshType : unsigned char;
}

namespace MySys::Res
{
	class MeshMgr
	{
		MeshMgr();
		~MeshMgr();
		static MeshMgr* singleton;	// シングルトン

	public:
		// Public Method
		static void OnInit();									// クラスの使用開始
		static void OnTerm();									// クラスの使用終了
		static void Init();										// 初期化
		static void LoadMesh(const MyRes::MeshType& _Type);		// 読み込み

		static const Mesh& GetMesh(const MyRes::MeshType _Type) { return *singleton->meshes[_Type]; }	// メッシュデータの取得

	private:
		// Private Variable
		std::unordered_map<MyRes::MeshType, Mesh*>meshes;		// 読み込んだメッシュのデータ群
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