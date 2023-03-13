#include "MeshMgr.h"
#include <string>
#include "Logger.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "Dx12Wrapper.h"
#include "EnumIterator.h"
#include "Helper/StringHelper.h"
#include "Tool/JsonLoader.h"
#include "InlineUtility.h"
#include "Helper/StringHelper.h"

// テスト用
//#include "Dir.h

/// <summary>
/// メッシュローダークラス用
/// </summary>
namespace
{
	/// <summary>
	/// メッシュローダークラス
	/// </summary>
	class MeshLoader
	{
	public:
		MeshLoader() = default;
		~MeshLoader() = default;

		// Public Method
		bool Load(const wchar_t* _FilePath, std::vector<ResMesh>& _Mesh);
	private:
		// Private Variable
		const aiScene* m_pScene = nullptr;   // シーンデータ.

		//Private Methods
		void ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh);
	};

	bool MeshLoader::Load(const wchar_t* _FilePath, std::vector<ResMesh>& _Mesh)
	{
		Assimp::Importer importer;
		unsigned int flag = 0;

		flag |= aiProcess_Triangulate;
		flag |= aiProcess_PreTransformVertices;
		flag |= aiProcess_CalcTangentSpace;
		flag |= aiProcess_GenSmoothNormals;
		flag |= aiProcess_GenUVCoords;
		flag |= aiProcess_RemoveRedundantMaterials;
		flag |= aiProcess_OptimizeMeshes;

		// ファイルを読み込み.
		m_pScene = importer.ReadFile(Sys::Helper::ToStr(_FilePath), flag);

		// チェック.
		if (m_pScene == nullptr)
		{
			return false;
		}

		// メッシュのメモリを確保
		_Mesh.clear();
		_Mesh.resize(m_pScene->mNumMeshes);

		// メッシュデータを変換.
		for (size_t i = 0; i < _Mesh.size(); ++i)
		{
			const auto pMesh = m_pScene->mMeshes[i];
			ParseMesh(_Mesh[i], pMesh);
		}

		// 不要になったのでクリア.
		importer.FreeScene();
		m_pScene = nullptr;

		// 正常終了.
		return true;
	}

	void MeshLoader::ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh)
	{
		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// 頂点データのメモリを確保.
		dstMesh.Vertices.resize(pSrcMesh->mNumVertices);

		for (auto i = 0u; i < pSrcMesh->mNumVertices; ++i)
		{
			auto pPosition = &(pSrcMesh->mVertices[i]);
			auto pNormal = &(pSrcMesh->mNormals[i]);
			auto pTexCoord = (pSrcMesh->HasTextureCoords(0)) ? &(pSrcMesh->mTextureCoords[0][i]) : &zero3D;
			auto pTangent = (pSrcMesh->HasTangentsAndBitangents()) ? &(pSrcMesh->mTangents[i]) : &zero3D;

			dstMesh.Vertices[i] = MeshVertex(
				DirectX::XMFLOAT3(pPosition->x, pPosition->y, pPosition->z),
				DirectX::XMFLOAT3(pNormal->x, pNormal->y, pNormal->z),
				DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y),
				DirectX::XMFLOAT3(pTangent->x, pTangent->y, pTangent->z)
			);
		}

		// 頂点インデックスのメモリを確保.
		dstMesh.Indices.resize(pSrcMesh->mNumFaces * 3);

		for (auto i = 0u; i < pSrcMesh->mNumFaces; ++i)
		{
			const auto& face = pSrcMesh->mFaces[i];
			assert(face.mNumIndices == 3);  // 三角形化しているので必ず3になっている

			dstMesh.Indices[i * 3 + 0] = face.mIndices[0];
			dstMesh.Indices[i * 3 + 1] = face.mIndices[1];
			dstMesh.Indices[i * 3 + 2] = face.mIndices[2];
		}
	}
}

namespace MyDX
{
	MeshMgr* MeshMgr::singleton = nullptr; // 実体
	
	MeshMgr::MeshMgr()
	{
		singleton = nullptr;
	}

	MeshMgr::~MeshMgr()
	{}

	void MeshMgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new MeshMgr;
		}
		singleton->Init();
	}

	void MeshMgr::OnTerm()
	{
		if (singleton)
		{
			// メッシュ読み込み
			typedef EnumIterator < Res::MeshType, Res::MeshType::Begin, Res::MeshType::End > typeIte;

			for (auto ite = typeIte(); ite != typeIte(Res::MeshType::End); ++ite)
			{
				SafeTerm(singleton->meshes[*ite]);
			}
			singleton->meshes.clear();

			delete singleton;
			singleton = nullptr;
		}
	}

	void MeshMgr::Init()
	{
		auto data = Sys::Tool::LoadJson("Resource/JsonData/MeshData.json");

		// メモリ予約
		singleton->meshes.reserve(data.MemberCount());
		// メッシュ読み込み
		typedef EnumIterator < Res::MeshType, Res::MeshType::Begin, Res::MeshType::End > typeItr;

		// 2重ロードチェック後、データ読み込みを行う
		for (auto ite = typeItr(); ite != typeItr(Res::MeshType::End); ++ite)
		{
			std::string tempKey = std::to_string(ite.GetId());
			const char* key = tempKey.c_str();

			const char* path = data[key].GetString();

			for (auto ite2 = typeItr(); ite2 != typeItr(Res::MeshType::End); ++ite2)
			{
				tempKey = std::to_string(ite2.GetId());
				const char* key2 = tempKey.c_str();

				const char* path2 = data[key2].GetString();

				if (path == path2 && ite != ite2)
				{
					// 2重ロード
					ELOG("Error : Mesh Double Loading");
				}
			}
			LoadMesh(*ite, path);
		}
	}

	void MeshMgr::LoadMesh(const Res::MeshType& _Type, const char* _Path)
	{
		std::vector<ResMesh> resMesh;		// 読み込んだメッシュを格納する
		//メッシュ読み込み
		{
			MeshLoader loader;
			loader.Load(Sys::Helper::ToWstr(_Path).c_str(), resMesh);
		}

		// メッシュデータの初期化と格納
		singleton->meshes[_Type] = singleton->InitMesh(resMesh);
	}

	Mesh* MeshMgr::InitMesh(std::vector<ResMesh>& _ResMesh)const
	{
		// メッシュデータが入る
		auto mesh = new(std::nothrow)Mesh;
		if (mesh == nullptr)
		{
			ELOG("Error : Out of memory.");
		}

		// DX12に対応させるために初期化
		ID3D12Device* dev = MyDX::Dx12Wrapper::GetDevice();
		for (auto res : _ResMesh)
		{
			// DX12に対応させるための初期化処理
			if (!mesh->Init(dev, res))
			{
				ELOG("Error : Mesh Init Failed.");
				delete mesh;
				return nullptr;
			}
		}

		return mesh;
	}
}