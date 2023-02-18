#include "ResMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <codecvt>
#include <cassert>

//assimp/scene.hのインクルードで発生した
//エラーの説明 : C2589 '(':スコープ解決演算子(::)の右側にあるトークンは使えません。
//エラーの原因 : 関数テンプレート max が Visual C++ のグローバル マクロ max と競合しています。
//
//解決策① : プロジェクト プロパティを設定し、定義済みのプロセッサに NOMINMAX を追加して、Visual C++ の min / max マクロ定義の使用を禁止します。
//方法	 : プロジェクトプロパティ - > C / C++ - > プリプロセッサ - > プリプロセッサ定義(事前定義されたコンパイルスイッチ NOMINMAX をここに追加)
//副作用 : ただし、visual C++ で定義されている定義は、double と int を自動的に一致させ、上記の設定を行うと、コード内で int 型のデータを 1.0 で手動で乗算して double の目的を達成します。
//
//解決策② : 番目の方法: かっこを追加すると、Vsual C++ の min / max マクロ定義と区別されます(今回のエラーとは少し違う)
//size. Width = std::max(size. Width, elementSize.Width);
//size.Width = (std::max)(size.Width, elementSize.Width);
//
//引用(中国語) : https://blog.csdn.net/xidianzhimeng/article/details/21181279?%3E

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

    //std::wstring型に変換する
    std::wstring Convert(const aiString& path)
    {
        wchar_t temp[256]{};
        size_t  size;
        mbstowcs_s(&size, temp, path.C_Str(), 256);
        return std::wstring(temp);
    }

    class MeshLoader
    {

    public:
        MeshLoader();
        ~MeshLoader();

        //Public Methods
        bool Load(const wchar_t* filename, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials);

    private:
        //Private Variables
        const aiScene* m_pScene = nullptr;   // シーンデータ.

        //Private Methods
        void ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh);
        void ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial);
    };

    MeshLoader::MeshLoader()
        : m_pScene(nullptr)
    { /* DO_NOTHING */
    }

    MeshLoader::~MeshLoader()
    { /* DO_NOTHING */
    }

    //-----------------------------------------------------------------------------
    //      メッシュをロードします.
    //-----------------------------------------------------------------------------
    bool MeshLoader::Load
    (
        const wchar_t* filename,
        std::vector<ResMesh>& meshes,
        std::vector<ResMaterial>& materials
    )
    {
        if (filename == nullptr)
        {
            return false;
        }

        //// wchar_t から char型(UTF-8)に変換します.
        auto path = ToUTF8(filename);

        Assimp::Importer importer;
        unsigned int flag = 0;
        flag |= aiProcess_Triangulate;                  // 全てのメッシュのすべての面データを三角形にする
        flag |= aiProcess_PreTransformVertices;         // ノードグラフを削除し、それらのどーどが持つローカル変換行列で全ての頂点を事前に変換
        flag |= aiProcess_CalcTangentSpace;             // インポートされたメッシュに対して接線ベクトルと従接線ベクトルを計算する
        flag |= aiProcess_GenSmoothNormals;             // メッシュ内の全ての頂点についてスムース法線ベクトルを生成する
        flag |= aiProcess_GenUVCoords;                  // 非UVマッピングを適切なテクスチャ座標のチャンネルへと変換する
        flag |= aiProcess_RemoveRedundantMaterials;     // 冗長または参照されていないマテリアルを検索し、削除する
        flag |= aiProcess_OptimizeMeshes;               // メッシュ数を最適化する

        //// ファイルを読み込み.
        m_pScene = importer.ReadFile(path, flag);

        // チェック.
        if (m_pScene == nullptr)
        {
            return false;
        }

        // メッシュのメモリを確保.
        meshes.clear();
        meshes.resize(m_pScene->mNumMeshes);

        // メッシュデータを変換.
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            const auto pMesh = m_pScene->mMeshes[i];
            ParseMesh(meshes[i], pMesh);
        }

        // マテリアルのメモリを確保.
        materials.clear();
        materials.resize(m_pScene->mNumMaterials);

        // マテリアルデータを変換.
        for (size_t i = 0; i < materials.size(); ++i)
        {
            const auto pMaterial = m_pScene->mMaterials[i];
            ParseMaterial(materials[i], pMaterial);
        }

        // 不要になったのでクリア.
        importer.FreeScene();
        m_pScene = nullptr;

        // 正常終了.
        return true;
    }

    //-----------------------------------------------------------------------------
    //      メッシュデータを解析します.
    //-----------------------------------------------------------------------------
    void MeshLoader::ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh)
    {
        // マテリアル番号を設定.
        dstMesh.MaterialId = pSrcMesh->mMaterialIndex;

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
            assert(face.mNumIndices == 3);  // 三角形化しているので必ず3になっている.

            dstMesh.Indices[i * 3 + 0] = face.mIndices[0];
            dstMesh.Indices[i * 3 + 1] = face.mIndices[1];
            dstMesh.Indices[i * 3 + 2] = face.mIndices[2];
        }
    }

    //-----------------------------------------------------------------------------
    //      マテリアルデータを解析します.
    //-----------------------------------------------------------------------------
    void MeshLoader::ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial)
    {
        // 拡散反射成分.
        {
            aiColor3D color(0.0f, 0.0f, 0.0f);

            if (pSrcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            {
                dstMaterial.Diffuse.x = color.r;
                dstMaterial.Diffuse.y = color.g;
                dstMaterial.Diffuse.z = color.b;
            }
            else
            {
                dstMaterial.Diffuse.x = 0.5f;
                dstMaterial.Diffuse.y = 0.5f;
                dstMaterial.Diffuse.z = 0.5f;
            }
        }

        // 鏡面反射成分.
        {
            aiColor3D color(0.0f, 0.0f, 0.0f);

            if (pSrcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
            {
                dstMaterial.Specular.x = color.r;
                dstMaterial.Specular.y = color.g;
                dstMaterial.Specular.z = color.b;
            }
            else
            {
                dstMaterial.Specular.x = 0.0f;
                dstMaterial.Specular.y = 0.0f;
                dstMaterial.Specular.z = 0.0f;
            }
        }

        // 鏡面反射強度.
        {
            auto shininess = 0.0f;
            if (pSrcMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
            {
                dstMaterial.Shininess = shininess;
            }
            else
            {
                dstMaterial.Shininess = 0.0f;
            }
        }

        // ディフューズマップ.
        {
            aiString path;
            if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
            {
                dstMaterial.DiffuseMap = Convert(path);
            }
            else
            {
                dstMaterial.DiffuseMap.clear();
            }
        }

        // スペキュラーマップ.
        {
            aiString path;
            if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS)
            {
                dstMaterial.SpecularMap = Convert(path);
            }
            else
            {
                dstMaterial.SpecularMap.clear();
            }
        }

        // シャイネスマップ.
        {
            aiString path;
            if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == AI_SUCCESS)
            {
                dstMaterial.ShininessMap = Convert(path);
            }
            else
            {
                dstMaterial.ShininessMap.clear();
            }
        }

        // 法線マップ
        {
            aiString path;
            if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS)
            {
                dstMaterial.NormalMap = Convert(path);
            }
            else
            {
                if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_HEIGHT(0), path) == AI_SUCCESS)
                {
                    dstMaterial.NormalMap = Convert(path);
                }
                else
                {
                    dstMaterial.NormalMap.clear();
                }
            }
        }
    }

}// namespace

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
const D3D12_INPUT_ELEMENT_DESC MeshVertex::InputElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};
const D3D12_INPUT_LAYOUT_DESC MeshVertex::InputLayout = { MeshVertex::InputElements, MeshVertex::InputElementCount };
static_assert(sizeof(MeshVertex) == 44, "Vertex struct/layout mismatch");


//-----------------------------------------------------------------------------
//      メッシュをロードします.
//-----------------------------------------------------------------------------
bool LoadMesh
(const wchar_t* filename,std::vector<ResMesh>& meshes, std::vector<ResMaterial>&  materials)
{
    MeshLoader loader;
    return loader.Load(filename, meshes, materials);
}
