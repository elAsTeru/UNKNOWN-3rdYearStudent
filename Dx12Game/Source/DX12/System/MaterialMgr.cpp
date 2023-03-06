#include "MaterialMgr.h"
#include "Dx12Wrapper.h"

namespace Sys
{
	MaterialMgr* MaterialMgr::singleton = nullptr;

	MaterialMgr::MaterialMgr()
	{
		singleton = nullptr;
	}

	MaterialMgr::~MaterialMgr()
	{
	}

	void MaterialMgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new MaterialMgr;
		}
	}

	void MaterialMgr::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void MaterialMgr::LoadTex(const wchar_t* _FilePath)
	{
		//DirectX::ResourceUploadBatch batch(MyDx::Dx12Wrapper::Get)
		
	}

	void MaterialMgr::SetTextureSet(
		const std::wstring&				base_path,
		Material&						material,
		DirectX::ResourceUploadBatch&	batch)
	{
		std::wstring pathBC = base_path + L"_bc.dds";
		std::wstring pathM = base_path + L"_m.dds";
		std::wstring pathR = base_path + L"_r.dds";
		std::wstring pathN = base_path + L"_n.dds";

		material.SetTexture(0, TU_BASE_COLOR,	pathBC,	batch);
		material.SetTexture(0, TU_METALLIC,		pathM,	batch);
		material.SetTexture(0, TU_ROUGHNESS,	pathR,	batch);
		material.SetTexture(0, TU_NORMAL,		pathN,	batch);
	}
}