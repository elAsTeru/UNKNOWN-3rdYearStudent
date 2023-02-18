#pragma once

#include "DescriptorPool.h"
#include <DirectXTK12/ResourceUploadBatch.h>
#include "Texture.h"
#include "ConstantBuffer.h"
#include <map>

class Material
{
    Material(const Material&) = delete;
    void operator = (const Material&) = delete;
public:
    enum TEXTURE_USAGE
    {
        TEXTURE_USAGE_DIFFUSE = 0,  //!< ディフューズマップ
        TEXTURE_USAGE_SPECULAR,     //!< スペキュラーマップ
        TEXTURE_USAGE_SHININESS,    //!< シャイネスマップ
        TEXTURE_USAGE_NORMAL,       //!< 法線マップ

        TEXTURE_USAGE_BASE_COLOR,   //!< ベースカラーマップとして利用します.
        TEXTURE_USAGE_METALLIC,     //!< メタリックマップとして利用します.
        TEXTURE_USAGE_ROUGHNESS,    //!< ラフネスマップとして利用します.

        TEXTURE_USAGE_COUNT
    };

    Material();
    ~Material();


    //Public Methods

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="pDevice">デバイス</param>
    /// <param name="pPool">ディスクリプタプール</param>
    /// <param name="bufferSize">1マテリアル当たりの定数バッファのサイズ</param>
    /// <param name="count">マテリアル数</param>
    /// <returns>true:初期化成功 / false:初期化失敗/returns>
    bool Init(ID3D12Device* pDevice, DescriptorPool* pPool, size_t bufferSize, size_t count);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// テクスチャを設定
    /// </summary>
    /// <param name="ndex">マテリアル番号</param>
    /// <param name="usage">テクスチャの使用用途</param>
    /// <param name="path">テクスチャパス</param>
    /// <param name="batch">リソースアップロードバッチ</param>
    /// <returns></returns>
    bool SetTexture(size_t ndex, TEXTURE_USAGE usage, const std::wstring& path, DirectX::ResourceUploadBatch& batch);

    /// <summary>
    /// 定数バッファのポインタを取得
    /// </summary>
    /// <param name="index">取得するマテリアル番号</param>
    /// <returns>指定された番号に一致する定数バッファのポインタが返却される
    ///          一致するものがない場合[nullptr]が返却される</returns>
    void* GetBufferPtr(size_t index) const;

    /// <summary>
    /// 定数バッファのポインタを取得
    /// </summary>
    /// <param name="index">取得するマテリアル番号</param>
    /// <returns>指定された番号に一致する定数バッファのポインタが返却される
    ///          一致するものがない場合[nullptr]が返却される</returns>
    template<typename T>
    T* GetBufferPtr(size_t index) const
    {
        return reinterpret_cast<T*>(GetBufferPtr(index));
    }

    /// <summary>
    /// 定数バッファのGPU仮想アドレスを取得
    /// </summary>
    /// <param name="index">取得するマテリアル番号</param>
    /// <returns>指定された番号に一致する定数バッファのGPU仮想アドレスを返却</returns>
    D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

    /// <summary>
    /// テクスチャハンドルを取得
    /// </summary>
    /// <param name="index">取得するマテリアル番号</param>
    /// <param name="usage">取得するテクスチャの使用用途</param>
    /// <returns>指定された番号に一致するテクスチャのディスクリプタハンドルが返却される</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

    /// <summary>
    /// マテリアル数を取得
    /// </summary>
    /// <returns>マテリアル数が返却される</returns>
    size_t GetCount() const;

private:
    struct Subset
    {
        ConstantBuffer* pCostantBuffer;                     //!< 定数バッファです.
        D3D12_GPU_DESCRIPTOR_HANDLE     TextureHandle[TEXTURE_USAGE_COUNT]; //!< テクスチャハンドルです.
    };


    //Private Variables
    std::map<std::wstring, Texture*>    m_pTexture;     //!< テクスチャです.
    std::vector<Subset>                 m_Subset;       //!< サブセットです.
    ID3D12Device* m_pDevice;                            //!< デバイスです.
    DescriptorPool* m_pPool;                            //!< ディスクリプタプールです(CBV_UAV_SRV).
};

constexpr auto TU_DIFFUSE       = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR      = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS     = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL        = Material::TEXTURE_USAGE_NORMAL;

constexpr auto TU_BASE_COLOR    = Material::TEXTURE_USAGE_BASE_COLOR;
constexpr auto TU_METALLIC      = Material::TEXTURE_USAGE_METALLIC;
constexpr auto TU_ROUGHNESS     = Material::TEXTURE_USAGE_ROUGHNESS;