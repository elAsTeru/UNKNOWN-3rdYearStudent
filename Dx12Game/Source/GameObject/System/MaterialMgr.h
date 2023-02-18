#pragma once
#include <vector>
#include "Material.h"

namespace MySys::Res
{
	class MaterialMgr
	{
		MaterialMgr();
		~MaterialMgr();
		static MaterialMgr* singleton;	// シングルトン

	public:
		// シングルトン生成、終了関数
		static void OnInit();
		static void OnTerm();

		// テクスチャ読み込み
		static void LoadTex(const wchar_t* _FilePath);

		// Public Method
		static void SetTextureSet(
			const std::wstring& _BasePath,
			Material& _Material,
			DirectX::ResourceUploadBatch& _Batch);
	private:
		// Public Variable
		//std::vector<Material> texData;	// テクスチャデータ
		//std::vector<Material> matData;	// マテリアルデータ
	};
}