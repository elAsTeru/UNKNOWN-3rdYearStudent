﻿#include "MeshMgr.h"
#include <string>
#include "FileUtil.h"
#include "Logger.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "Dx12Wrapper.h"
#include "Helper/EnumIterator.h"
#include "Helper/StringHelper.h"
#include "Tool/JsonLoader.h"

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
		std::wstring absolutePath;			// 絶対パス

		// 相対からパス探索、成功なら絶対パスに変換
		if (!SearchFilePath(_FilePath, absolutePath))
		{
			ELOG("Error : Mesh File Not Found");
		}

		// wchar_t から char型(UTF-8)に変換します.
		auto path = System::Helper::ToStr(absolutePath);

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
		m_pScene = importer.ReadFile(path, flag);

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

namespace Sys
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
			typedef System::Helper::EnumIterator < Res::MeshType, Res::MeshType::Begin, Res::MeshType::End > typeIte;

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
		auto data = System::Tool::LoadJson("Resource/JsonData/MeshData.json");

		// メモリ予約
		singleton->meshes.reserve(data.MemberCount());
		// メッシュ読み込み
		typedef System::Helper::EnumIterator < Res::MeshType, Res::MeshType::Begin, Res::MeshType::End > typeItr;

		// 2重ロードチェック後、データ読み込みを行う
		for (auto ite = typeItr(); ite != typeItr(Res::MeshType::End); ++ite)
		{
			const std::string tempId = std::to_string(ite.GetId());
			const char* path = data[tempId.c_str()].GetString();

			for (auto ite2 = typeItr(); ite2 != typeItr(Res::MeshType::End); ++ite2)
			{
				const std::string tempId2 = std::to_string(ite2.GetId());
				const char* path2 = data[tempId2.c_str()].GetString();

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
			loader.Load(System::Helper::ToWstr(_Path).c_str(), resMesh);
		}

		// 読み込んだメッシュを格納 // どこでパスを記録するか
		{
			// メモリ予約
			singleton->meshes.reserve(resMesh.size());
			auto mesh = new(std::nothrow)Mesh();
			// メッシュの初期化
			mesh = MyDX::Dx12Wrapper::InitMesh(resMesh, singleton->meshes[_Type]);
			// 初期化が成功したら登録する
			if (mesh)
			{
				singleton->meshes[_Type] = mesh;
			}
			else
			{
				ELOG("Error : Missing Init Mesh");
			}
		}
	}
}