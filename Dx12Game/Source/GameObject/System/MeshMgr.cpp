#include "MeshMgr.h"
#include <string>
#include "FileUtil.h"
#include "Logger.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "Dx12Wrapper.h"
#include "MeshData.h"

namespace
{
	//UTF-8文字列に変換する
	std::string ToUTF8(const std::wstring& value)
	{
		auto length = WideCharToMultiByte(
			CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);
		auto buffer = new char[length];

		WideCharToMultiByte(
			CP_UTF8, 0U, value.data(), -1, buffer, length, nullptr, nullptr);

		std::string result(buffer);
		delete[] buffer;
		buffer = nullptr;

		return result;
	}
}

/// <summary>
/// メッシュローダークラス用
/// </summary>
namespace MySys
{
	namespace Res
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
			auto pathUTF8 = ToUTF8(absolutePath);

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
			m_pScene = importer.ReadFile(pathUTF8, flag);

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
}

/// <summary>
/// メッシュ管理用
/// </summary>
namespace MySys
{
	namespace Res
	{
		MeshMgr* MeshMgr::singleton = nullptr;		// 実態ポインタ

		/// <summary>
		/// コンストラクタ
		/// </summary>
		MeshMgr::MeshMgr()
		{
			singleton = nullptr;
		}
		
		/// <summary>
		/// デストラクタ
		/// </summary>
		MeshMgr::~MeshMgr()
		{
		}
		
		/// <summary>
		/// 初期化：実行後その他関数が使用可能になる
		/// </summary>
		void MeshMgr::OnInit()
		{
			if (!singleton)
			{
				singleton = new MeshMgr;
			}
			Init();
		}
		
		/// <summary>
		/// 終了：実行後関数等が使用できなくなる
		/// </summary>
		void MeshMgr::OnTerm()
		{
			if (singleton)
			{
				// 全てのメッシュデータをロード
				for (auto type = MyRes::MeshType::Board; type != MyRes::MeshType::End; ++type)
				{
					SafeTerm(singleton->meshes[type]);
				}
				singleton->meshes.clear();

				delete singleton;
				singleton = nullptr;
			}
		}

		void MeshMgr::Init()
		{
			// 全てのメッシュデータをロード
			for (auto type = MyRes::MeshType::Board; type != MyRes::MeshType::End; ++type)
			{
				// ?同じものを読み込んでいないか比較する
				for (auto comparison = MyRes::MeshType::Board; comparison != MyRes::MeshType::End; ++comparison)
				{
					if ((type != comparison) && (MyRes::MeshData::pathMap[type] == MyRes::MeshData::pathMap[comparison]))
					{
						ELOG("Error : Mesh File Double Load");
					}
				}
				LoadMesh(type);
			}
		}

		void MeshMgr::LoadMesh(const MyRes::MeshType& _Type)
		{
			std::vector<ResMesh> resMesh;		// 読み込んだメッシュを格納する
			//メッシュ読み込み
			{
				MeshLoader loader;
				loader.Load(MyRes::MeshData::pathMap[_Type],resMesh);
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
}