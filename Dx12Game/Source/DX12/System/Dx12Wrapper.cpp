#include "Dx12Wrapper.h"

// DirectXTKが提供する機能
// https://qiita.com/katsusanw/items/c42b737d8d91d903474a

#include <cmath>
#include <d3dcompiler.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/VertexTypes.h>
#include <DirectXTK12/CommonStates.h>
#include <DirectXTK12/DirectXHelpers.h>
#include <DirectXTex/d3dx12.h>
#include <cassert>
#include "FileUtil.h"
#include "Helper/DX12Helper.h"
#include "Logger.h"
#include "Tool/DXTK12Font.h"
#include "MaterialMgr.h"
#include "Resource/LineRes.h"
#include "Timer.h"
#include "EnumIterator.h"
#include "InlineUtility.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dcompiler.lib")

// 仮インクルード
#include "MeshMgr.h"

namespace
{
    // Transform structure
    struct alignas(256) CTransform
    {
        DirectX::XMMATRIX   World;      // ワールド行列です
        DirectX::XMMATRIX   View;       // ビュー行列です
        DirectX::XMMATRIX   Proj;       // 射影行列です
        float Alpha;
    };

    struct alignas(256) CbLine
    {
        DirectX::XMMATRIX   World;      // ワールド行列です
        DirectX::XMMATRIX   View;       // ビュー行列です
        DirectX::XMMATRIX   Proj;       // 射影行列です
        DirectX::XMFLOAT4   Color;
    };

    struct MaterialBuffer
    {
        Vector3 Diffuse;        // 拡散反射
        float   Alpha;          // 透過度
        Vector3 Specular;       // 鏡面反射率
        float Shininess;        // 鏡面反射強度
    };

    struct DirLightBuffer
    {
        Vector3 direction; // 向き
        Vector3 color;     // ライトカラー
    };
}

namespace MyDX
{
    constexpr uint16_t MaxLineRectCB = 100;
    constexpr uint16_t MaxBasicMeshCB = 400;
    constexpr uint16_t MaxPhongMeshCB = 600;
    constexpr uint16_t MaxUICB = 100;

    Dx12Wrapper* Dx12Wrapper::singleton = nullptr;

    constexpr float ClsClr[4] = { 0,0,0,1 };

    Dx12Wrapper::Dx12Wrapper():
        hWnd          (nullptr),
        width           (0),
        height          (0),
        frameIndex      (0)
    {
        singleton = nullptr;
    }

    Dx12Wrapper::~Dx12Wrapper() 
    {
    }

    bool Dx12Wrapper::OnInit(HWND _hWnd, uint32_t _width, uint32_t _height)
    {
        if (!singleton)
        {
            singleton = new Dx12Wrapper;

            singleton->hWnd = _hWnd;
            singleton->width = _width;
            singleton->height = _height;

            if (!InitD3D()) { return false; }

            // TKSpriteFont
            DXTK12Font::OnInit(singleton->device.Get(),singleton->queue.Get(),singleton->fence,singleton->viewport);

            if (!InitRender()) { return false; }
        }
        return true;
    }

    void Dx12Wrapper::OnTerm()
    {
        Term();
        TermD3D();
    }

    void Dx12Wrapper::SetDirLightDir(const Vector3 _Dir)
    {
        if (singleton->dirLight != nullptr)
        {
            singleton->dirLight->GetPtr<DirLightBuffer>()->direction = _Dir;
        }
    }

    bool Dx12Wrapper::InitD3D()
    {
#if defined(DEBUG) || defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debug;
            auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
            if (SUCCEEDED(hr)) { debug->EnableDebugLayer(); }  // デバッグレイヤーを有効化
        }
#endif
        // フィーチャーレベルが対応していない可能性があるので、合致する設定を探す
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        // デバイスの生成
        for (auto lv : levels)
        {
            if (SUCCEEDED(D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&singleton->device))))
            {
                break;
            }
        }
        // デバイスの生成に失敗
        if (singleton->device == nullptr)
        {
            return false;
        }

        // コマンドキューの生成.
        {
            D3D12_COMMAND_QUEUE_DESC desc{};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 0;

            auto hr = singleton->device->CreateCommandQueue(&desc, IID_PPV_ARGS(singleton->queue.GetAddressOf()));
            if (FAILED(hr)) { return false; }
        }

        // スワップチェインの生成
        {
            // DXGIファクトリーの生成
            ComPtr<IDXGIFactory4> pFactory = nullptr;
            auto hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
            if (FAILED(hr)) { return false; }

            // スワップチェインの設定
            DXGI_SWAP_CHAIN_DESC desc{};
            desc.BufferDesc.Width                   = singleton->width;
            desc.BufferDesc.Height                  = singleton->height;
            desc.BufferDesc.RefreshRate.Numerator   = 60;
            desc.BufferDesc.RefreshRate.Denominator = 1;
            desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
            desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count                   = 1;
            desc.SampleDesc.Quality                 = 0;
            desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount                        = FrameCount;
            desc.OutputWindow                       = singleton->hWnd;
            desc.Windowed                           = true;                                    // false : FullScreen
            desc.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            // スワップチェインの生成
            ComPtr<IDXGISwapChain> pSwapChain;
            hr = pFactory->CreateSwapChain(singleton->queue.Get(), &desc, pSwapChain.GetAddressOf());
            if (FAILED(hr)) { return false; }

            // IDXGISwapChain3 を取得
            hr = pSwapChain.As(&singleton->swapChain);
            if (FAILED(hr)) { return false; }

            // バックバッファ番号を取得
            singleton->frameIndex = singleton->swapChain->GetCurrentBackBufferIndex();

            // 不要になったので解放
            pFactory.Reset();
            pSwapChain.Reset();
        }

        //ディスクリプタプールの生成
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc{};

            desc.NodeMask = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 4096;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (!DescriptorPool::Create(singleton->device.Get(), &desc, &singleton->pool[POOL_TYPE_RES]))
            {
                return false;
            }

            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            desc.NumDescriptors = 2048;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (!DescriptorPool::Create(singleton->device.Get(), &desc, &singleton->pool[POOL_TYPE_SMP]))
            {
                return false;
            }

            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = 2048;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (!DescriptorPool::Create(singleton->device.Get(), &desc, &singleton->pool[POOL_TYPE_RTV]))
            {
                return false;
            }

            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            desc.NumDescriptors = 2048;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (!DescriptorPool::Create(singleton->device.Get(), &desc, &singleton->pool[POOL_TYPE_DSV]))
            {
                return false;
            }
        }

        // コマンドリストの生成.
        {
            if (!singleton->cmdList.Init(singleton->device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT, FrameCount))
            { return false; }
        }

        // レンダーターゲットビューの生成.
        {
            for (auto i = 0u; i < FrameCount; ++i)
            {
                if (!singleton->colorTarget[i].InitFromBackBuffer(
                    singleton->device.Get(),
                    singleton->pool[POOL_TYPE_RTV],
                    i,
                    singleton->swapChain.Get()))
                {
                    return false;
                }
            }
        }

        // 深度ステンシルバッファの生成
        {
            if (!singleton->depthTarget.Init(
                singleton->device.Get(),
                singleton->pool[POOL_TYPE_DSV],
                singleton->width,
                singleton->height,
                DXGI_FORMAT_D32_FLOAT))
            {
                return false;
            }
        }

        // フェンスの生成.
        {
            if (!singleton->fence.Init(singleton->device.Get())) { return false; }
        }

        // ビューポートの設定.
        {
            singleton->viewport.TopLeftX = 0.0f;
            singleton->viewport.TopLeftY = 0.0f;
            singleton->viewport.Width = float(singleton->width);
            singleton->viewport.Height = float(singleton->height);
            singleton->viewport.MinDepth = 0.0f;
            singleton->viewport.MaxDepth = 1.0f;
        }

        // シザー矩形の設定.
        {
            singleton->scissor.left = 0;
            singleton->scissor.right = singleton->width;
            singleton->scissor.top = 0;
            singleton->scissor.bottom = singleton->height;
        }

        // 正常終了.
        return true;
    }

    void Dx12Wrapper::DrawMesh(const DirectX::XMMATRIX _Mat, Res::MeshType _MeshType, Res::MaterialType _MatType, const float _Alpha)
    {
        singleton->drawMeshData.emplace_back(DrawMeshDesc(_Mat, _MeshType, _MatType, _Alpha));
    }

    void Dx12Wrapper::DrawMesh2D(const DirectX::XMMATRIX _Mat, Res::MeshType _MeshType, Res::MaterialType _MatType, const float _Alpha)
    {
        singleton->drawMesh2DData.emplace_back(DrawMeshDesc(_Mat, _MeshType, _MatType, _Alpha));
    }

    void Dx12Wrapper::DrawRect(const DirectX::XMMATRIX& _Matrix, const DirectX::XMFLOAT4& _Color)
    {
        singleton->drawRectData.push_back({ _Matrix, _Color });
    }



    ID3D12Resource* Dx12Wrapper::MakeTexture(int _Color)
    {
        D3D12_HEAP_PROPERTIES texHeapProp = {};
        texHeapProp.Type                    = D3D12_HEAP_TYPE_CUSTOM;
        texHeapProp.CPUPageProperty         = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
        texHeapProp.MemoryPoolPreference    = D3D12_MEMORY_POOL_L0;
        texHeapProp.CreationNodeMask        = 0;
        texHeapProp.VisibleNodeMask         = 0;

        D3D12_RESOURCE_DESC resDesc         = {};
        resDesc.Format                      = DXGI_FORMAT_R8G8B8A8_UNORM;
        resDesc.Width                       = 4;	//幅
        resDesc.Height                      = 4;	//高さ
        resDesc.DepthOrArraySize            = 1;
        resDesc.SampleDesc.Count            = 1;
        resDesc.SampleDesc.Quality          = 0;
        resDesc.MipLevels                   = 1;
        resDesc.Dimension                   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resDesc.Layout                      = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resDesc.Flags                       = D3D12_RESOURCE_FLAG_NONE;

        ID3D12Resource* colorBuff = nullptr;

        auto result = singleton->device->CreateCommittedResource(
            &texHeapProp,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr,
            IID_PPV_ARGS(&colorBuff));
        if (FAILED(result)) {
            return nullptr;
        }
        std::vector<unsigned char> data(4 * 4 * 4);
        fill(data.begin(), data.end(), _Color);

        //データ転送
        result = colorBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, static_cast<UINT>(data.size()));
        return colorBuff;
    }

    void Dx12Wrapper::TermD3D()
    {
        // GPUの処理完了を待つ
        singleton->fence.Sync(singleton->queue.Get());

        // フェンス破棄
        singleton->fence.Term();

        // レンダーターゲットビュー破棄
        for (auto i = 0u; i < FrameCount; ++i) { singleton->colorTarget[i].Term(); }

        //深度ステンシルビューの破棄
        singleton->depthTarget.Term();

        //コマンドリストの破棄
        singleton->cmdList.Term();

        //ディスクリプタプールの破棄
        for (auto i = 0; i < POOL_COUNT; ++i)
        {
            if (singleton->pool[i] != nullptr)
            {
                singleton->pool[i]->Release();
                singleton->pool[i] = nullptr;
            }
        }

        // スワップチェインの破棄
        singleton->swapChain.Reset();

        // コマンドキューの破棄
        singleton->queue.Reset();

        // デバイスの破棄
        singleton->device.Reset();
    }

    void Dx12Wrapper::Present(uint32_t interval)
    {
        // 画面に表示.
        singleton->swapChain->Present(interval, 0);
        // 完了待ち
        singleton->fence.Wait(singleton->queue.Get(), INFINITE);
        // フレーム番号を更新
        singleton->frameIndex = singleton->swapChain->GetCurrentBackBufferIndex();
    }

    bool Dx12Wrapper::InitRender()
    {
        {
            int materialSize = 1;   // 今はすべてのマテリアルは１枚のテクスチャで作成
            // マテリアルの初期化
            typedef EnumIterator < Res::MaterialType, Res::MaterialType::Begin, Res::MaterialType::End > typeItr;
            for (auto itr = typeItr(); itr != typeItr(Res::MaterialType::End); ++itr)
            {
                if (!singleton->materials[*itr].Init(singleton->device.Get(), singleton->pool[POOL_TYPE_RES], sizeof(MaterialBuffer), materialSize))
                {
                    ELOG("Error : Material::Init() Failed");
                    return false;
                }
            }

            //リソースバッチを用意.
            DirectX::ResourceUploadBatch batch(singleton->device.Get());

            //バッチ開始
            batch.Begin();

            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/normal",      singleton->materials[Res::MaterialType::Normal],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/white",       singleton->materials[Res::MaterialType::White],    batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/orange",      singleton->materials[Res::MaterialType::Orange],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/green",       singleton->materials[Res::MaterialType::Green],    batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/unk",         singleton->materials[Res::MaterialType::Unk],      batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/default",     singleton->materials[Res::MaterialType::Default],  batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/gray",        singleton->materials[Res::MaterialType::Gray],     batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/red",         singleton->materials[Res::MaterialType::Red],      batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/u",           singleton->materials[Res::MaterialType::U],        batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/u2",          singleton->materials[Res::MaterialType::U2],       batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/black",       singleton->materials[Res::MaterialType::Black],    batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/purple",      singleton->materials[Res::MaterialType::Purple],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/yellow",      singleton->materials[Res::MaterialType::Yellow],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/blue",        singleton->materials[Res::MaterialType::Blue],     batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/tenten",      singleton->materials[Res::MaterialType::Tenten],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/gray2",       singleton->materials[Res::MaterialType::Gray2],    batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/phase1",      singleton->materials[Res::MaterialType::Phase1],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/phase2",      singleton->materials[Res::MaterialType::Phase2],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/phase3",      singleton->materials[Res::MaterialType::Phase3],   batch);
            Sys::MaterialMgr::SetTextureSet(L"Resource/Texture/phase4",      singleton->materials[Res::MaterialType::Phase4],   batch);

            // バッチ終了.
            auto future = batch.End(singleton->queue.Get());
            //バッチ完了を待機
            future.wait();
        }

        // 線による正方形の作成
        {
            // 奥行の線
            singleton->lineData[0].vertices.resize(2);
            singleton->lineData[0].vertices[0] = DirectX::XMFLOAT3(-0.5f, 0, 0.5f);       // 始点
            singleton->lineData[0].vertices[1] = DirectX::XMFLOAT3(0.5f, 0, 0.5f);       // 終点

            if (!singleton->lineData[0].lineVB.Init(singleton->device.Get(), sizeof(LineData::vertices), singleton->lineData[0].vertices.data()))
            {
                ELOG("Error : Line::Init() Failed.");
            }

            // 手前の線
            singleton->lineData[1].vertices.resize(2);
            singleton->lineData[1].vertices[0] = DirectX::XMFLOAT3(-0.5f, 0, -0.5f);      // 始点
            singleton->lineData[1].vertices[1] = DirectX::XMFLOAT3(0.5f,  0, -0.5f);      // 終点

            if (!singleton->lineData[1].lineVB.Init(singleton->device.Get(), sizeof(LineData::vertices), singleton->lineData[1].vertices.data()))
            {
                ELOG("Error : Line::Init() Failed.");
            }

            // 右の線
            singleton->lineData[2].vertices.resize(2);
            singleton->lineData[2].vertices[0] = DirectX::XMFLOAT3(0.5f, 0, 0.5f);       // 始点
            singleton->lineData[2].vertices[1] = DirectX::XMFLOAT3(0.5f, 0, -0.5f);      // 終点

            if (!singleton->lineData[2].lineVB.Init(singleton->device.Get(), sizeof(LineData::vertices), singleton->lineData[2].vertices.data()))
            {
                ELOG("Error : Line::Init() Failed.");
            }

            // 左の線
            singleton->lineData[3].vertices.resize(2);
            singleton->lineData[3].vertices[0] = DirectX::XMFLOAT3(-0.5f, 0, 0.5f);      // 始点
            singleton->lineData[3].vertices[1] = DirectX::XMFLOAT3(-0.5f, 0, -0.5f);     // 終点

            if (!singleton->lineData[3].lineVB.Init(singleton->device.Get(), sizeof(LineData::vertices), singleton->lineData[3].vertices.data()))
            {
                ELOG("Error : Line::Init() Failed.");
            }
        }

        //// カメラバッファの設定.
        //{
        //    for (auto i = 0; i < FrameCount; ++i)
        //    {
        //        if (!m_CameraCB[i].Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RES], sizeof(CbCamera)))
        //        {
        //            ELOG("Error : ConstantBuffer::Init() Failed.");
        //            return false;
        //        }
        //    }
        //}

        // ディレクショナルライトの設定
        {
            auto cb = new (std::nothrow) ConstantBuffer();
            if (cb == nullptr)
            {
                ELOG("Error : Out of memory.");
                return false;
            }

            if (!cb->Init(singleton->device.Get(), singleton->pool[POOL_TYPE_RES], sizeof(DirLightBuffer)))
            {
                ELOG("Error : ConstantBuffer::Init() Failed.");
                return false;
            }

            auto ptr = cb->GetPtr<DirLightBuffer>();
            ptr->direction = Vector3(0.0f, -1.0f, 0.0f);
            ptr->color = Vector3(1.0f, 1.0f, 1.0f);
            singleton->dirLight = cb;
        }

        // ルートシグニチャの生成.
        {
            auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
            flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
            flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
            flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            // ディスクリプタレンジを設定.
            D3D12_DESCRIPTOR_RANGE range[2] = {};
            range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            range[0].BaseShaderRegister = 0;
            range[0].NumDescriptors = 1;                           // t0

            range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            range[1].BaseShaderRegister = 1;
            range[1].NumDescriptors = 1;                           // t1

            //// ルートパラメーターの設定
            // 座標系
            D3D12_ROOT_PARAMETER param[4] = {};
            param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            param[0].Descriptor.ShaderRegister = 0;
            param[0].Descriptor.RegisterSpace = 0;
            param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

            // 通常テクスチャ
            param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param[1].DescriptorTable.pDescriptorRanges = &range[0];
            param[1].DescriptorTable.NumDescriptorRanges = 1;
            param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            // ブルーム適応テクスチャ
            param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param[2].DescriptorTable.pDescriptorRanges = &range[1];
            param[2].DescriptorTable.NumDescriptorRanges = 1;
            param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            // ディレクショナルライト
            param[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            param[3].Descriptor.ShaderRegister = 1;
            param[3].Descriptor.RegisterSpace = 0;
            param[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            // スタティックサンプラーの設定.
            D3D12_STATIC_SAMPLER_DESC sampler{};
            sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
            sampler.MaxAnisotropy = 1;
            sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
            sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            sampler.MinLOD = -D3D12_FLOAT32_MAX;
            sampler.MaxLOD = +D3D12_FLOAT32_MAX;
            sampler.ShaderRegister = 0;
            sampler.RegisterSpace = 0;
            sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            // ルートシグニチャの設定.
            D3D12_ROOT_SIGNATURE_DESC desc{};
            desc.NumParameters = _countof(param);
            desc.NumStaticSamplers = 1;
            desc.pParameters = param;
            desc.pStaticSamplers = &sampler;
            desc.Flags = flag;

            ComPtr<ID3DBlob> pBlob;
            ComPtr<ID3DBlob> pErrorBlob;

            // シリアライズ
            auto hr = D3D12SerializeRootSignature(
                &desc,
                D3D_ROOT_SIGNATURE_VERSION_1,
                pBlob.GetAddressOf(),
                pErrorBlob.GetAddressOf());
            if (FAILED(hr))
            {
                return false;
            }

            // ルートシグニチャを生成
            hr = singleton->device->CreateRootSignature(
                0,
                pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(),
                IID_PPV_ARGS(singleton->BasicRS.GetAddressOf()));
            if (FAILED(hr))
            {
                ELOG("Error : Root Signature Create Failed. retcode = 0x%x", hr);
                return false;
            }
        }

        // パイプラインステートの生成.
        {
            ComPtr<ID3DBlob> vsBasicBlob;
            ComPtr<ID3DBlob> psBasicBlob;
            ComPtr<ID3DBlob> psBasic2DBlob;
            ComPtr<ID3DBlob> vsGeometryBlob;
            ComPtr<ID3DBlob> psGeometryBlob;

            // 標準用
            singleton->LoadShader(L"Shader/cso/BasicVS.cso", vsBasicBlob);
            singleton->LoadShader(L"Shader/cso/BasicPS.cso", psBasicBlob);
            singleton->LoadShader(L"Shader/cso/Basic2DPS.cso", psBasic2DBlob);
            // 線用
            singleton->LoadShader(L"Shader/cso/GeometryVS.cso", vsGeometryBlob);
            singleton->LoadShader(L"Shader/cso/GeometryPS.cso", psGeometryBlob);

            // ラスタライザの設定(アンチエイリアシング)
            D3D12_RASTERIZER_DESC rasterDesc{};
            rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
            rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
            rasterDesc.FrontCounterClockwise = false;
            rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            rasterDesc.DepthBiasClamp = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            rasterDesc.DepthClipEnable = true;
            rasterDesc.MultisampleEnable = false;
            rasterDesc.AntialiasedLineEnable = true;
            rasterDesc.ForcedSampleCount = 0;
            rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

            // 標準用GPS
            {
                D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
                // 2D
                desc.InputLayout                                    = MeshVertex::InputLayout;
                desc.pRootSignature                                 = singleton->BasicRS.Get();
                desc.VS                                             = { vsBasicBlob->GetBufferPointer(), vsBasicBlob->GetBufferSize() };
                desc.PS                                             = { psBasic2DBlob->GetBufferPointer(), psBasic2DBlob->GetBufferSize() };
                desc.BlendState.AlphaToCoverageEnable               = false;
                desc.BlendState.IndependentBlendEnable              = false;
                desc.RasterizerState                                = DirectX::CommonStates::CullNone;
                desc.BlendState.RenderTarget->BlendEnable           = TRUE;                             // 加算、乗算、α等のブレンドを行うか
                desc.BlendState.RenderTarget->LogicOpEnable         = FALSE;                            // 論理演算するかどうか ※上かどちらかのみ
                desc.BlendState.RenderTarget->SrcBlend              = D3D12_BLEND_SRC_ALPHA;            // 描画元係数
                desc.BlendState.RenderTarget->DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;        // 描画先係数
                desc.BlendState.RenderTarget->BlendOp               = D3D12_BLEND_OP_ADD;               // ブレンド方法
                desc.BlendState.RenderTarget->SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;            // 描画元α係数
                desc.BlendState.RenderTarget->DestBlendAlpha        = D3D12_BLEND_ZERO;                 // 描画先α係数
                desc.BlendState.RenderTarget->BlendOpAlpha          = D3D12_BLEND_OP_ADD;               // どう論理演算するのか
                desc.BlendState.RenderTarget->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;     // 書き込むときのマスク値
                desc.DepthStencilState                              = DirectX::CommonStates::DepthDefault;
                desc.SampleMask                                     = UINT_MAX;
                desc.PrimitiveTopologyType                          = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                desc.NumRenderTargets                               = 1;
                desc.RTVFormats[0]                                  = singleton->colorTarget[0].GetViewDesc().Format;
                desc.DSVFormat                                      = singleton->depthTarget.GetViewDesc().Format;
                desc.SampleDesc.Count                               = 1;
                desc.SampleDesc.Quality                             = 0;
                // GPS生成
                auto hr = singleton->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(singleton->pso[DrawType::UI].GetAddressOf()));
                Helper::CheckResult(hr);

                // 3D
                desc.PS = { psBasicBlob->GetBufferPointer(), psBasicBlob->GetBufferSize() };
                desc.NumRenderTargets = 3;
                desc.RTVFormats[0] = singleton->colorTarget[0].GetViewDesc().Format;
                desc.RTVFormats[1] = singleton->colorTarget[0].GetViewDesc().Format;
                desc.RTVFormats[2] = singleton->colorTarget[0].GetViewDesc().Format;
                // GPS生成
                hr = singleton->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(singleton->pso[DrawType::Basic].GetAddressOf()));
                Helper::CheckResult(hr);
            }

            // 線用
            {
                D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
                desc.InputLayout = Res::LineVertex::InputLineLayout;
                desc.pRootSignature = singleton->BasicRS.Get();
                desc.VS = { vsGeometryBlob->GetBufferPointer(), vsGeometryBlob->GetBufferSize() };
                desc.PS = { psGeometryBlob->GetBufferPointer(), psGeometryBlob->GetBufferSize() };
                desc.RasterizerState = rasterDesc;
                desc.BlendState.RenderTarget->BlendEnable = TRUE;                                       // 加算、乗算、α等のブレンドを行うか
                desc.BlendState.RenderTarget->LogicOpEnable = FALSE;                                    // 論理演算するかどうか ※上かどちらかのみ
                desc.BlendState.RenderTarget->SrcBlend = D3D12_BLEND_SRC_ALPHA;                         // 描画元係数
                desc.BlendState.RenderTarget->DestBlend = D3D12_BLEND_INV_SRC_ALPHA;                    // 描画先係数
                desc.BlendState.RenderTarget->BlendOp = D3D12_BLEND_OP_ADD;                             // ブレンド方法
                desc.BlendState.RenderTarget->SrcBlendAlpha = D3D12_BLEND_ONE;                          // 描画元α係数
                desc.BlendState.RenderTarget->DestBlendAlpha = D3D12_BLEND_ZERO;                        // 描画先α係数
                desc.BlendState.RenderTarget->BlendOpAlpha = D3D12_BLEND_OP_ADD;                        // どう論理演算するのか
                desc.BlendState.RenderTarget->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;     // 書き込むときのマスク値
                desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
                desc.DepthStencilState = DirectX::CommonStates::DepthDefault;
                desc.SampleMask = UINT_MAX;
                desc.NumRenderTargets = 1;
                desc.RTVFormats[0] = singleton->colorTarget[0].GetViewDesc().Format;
                desc.DSVFormat = singleton->depthTarget.GetViewDesc().Format;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;

                auto hr = singleton->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(singleton->pso[DrawType::Line].GetAddressOf()));
                if (FAILED(hr))
                {
                    ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
                    return false;
                }
            }

        }

        // ガウシアンぼかし
        //singleton->CreateBokeParamResourcec();
        // ブルーム
        singleton->CreateBloomBuffer();
        // ペラ作成
        singleton->CreatePeraResource();
        singleton->CreatePeraVertex();
        singleton->CreatePeraPipeline();

        // 変換行列用の定数バッファの生成
        {
            // 垂直画角とアスペクト比の設定.
            float aspect = static_cast<float>(singleton->width) / static_cast<float>(singleton->height);
            singleton->proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(37.5f), aspect, 1.0f, 500.0f);

            // メモリ予約
            singleton->transform[DrawType::Basic].reserve(FrameCount* MaxBasicMeshCB);
            for (auto i = 0u; i < FrameCount * MaxBasicMeshCB; ++i)
            {
                auto pCB = new (std::nothrow) ConstantBuffer();
                if (pCB == nullptr)
                {
                    ELOG("Error : Out of memory.");
                    return false;
                }

                // 定数バッファ初期化
                if (!pCB->Init(singleton->device.Get(), singleton->pool[POOL_TYPE_RES], sizeof(CTransform) * 2))
                {
                    ELOG("Error : ConstantBuffer::Init() Failed.");
                    return false;
                }

                // カメラ設定
                // 0,0,0にしたらエラーが出る
                singleton->eyePos = Vector3(0.0f, 60.0f, 0.0f);
                singleton->targetPos = Vector3(0.0f, 0.0f, 0.0f);
                singleton->upward = Vector3(0.0f, 0.0f, 1.0f);

                // 変換行列を設定
                auto ptr = pCB->GetPtr<CTransform>();
                ptr->View = DirectX::XMMatrixLookAtLH(singleton->eyePos, singleton->targetPos, singleton->upward);
                ptr->Proj = singleton->proj;
                ptr->Alpha = 1;

                singleton->transform[DrawType::Basic].push_back(pCB);
            }
            singleton->transform[DrawType::Basic].shrink_to_fit();

            singleton->transform[DrawType::Line].reserve(FrameCount* MaxLineRectCB);
            for (auto i = 0u; i < FrameCount * MaxLineRectCB; ++i)
            {
                auto pCB = new (std::nothrow) ConstantBuffer();

                if (pCB == nullptr)
                {
                    ELOG("Error : Out of memory.");
                    return false;
                }

                // 定数バッファ初期化
                if (!pCB->Init(singleton->device.Get(), singleton->pool[POOL_TYPE_RES], sizeof(CbLine) * 2))
                {
                    ELOG("Error : ConstantBuffer::Init() Failed.");
                    return false;
                }

                // 変換行列を設定
                auto ptr = pCB->GetPtr<CbLine>();
                //ptr->View = Matrix::CreateLookAt(singleton->eyePos, singleton->targetPos, singleton->upward);
                ptr->Proj = singleton->proj;
                ptr->Color = { 1,1,1,1 };

                singleton->transform[DrawType::Line].push_back(pCB);
            }
            singleton->transform[DrawType::Line].shrink_to_fit();


            singleton->transform[DrawType::UI].reserve(FrameCount * MaxUICB);
            for (auto i = 0u; i < FrameCount * MaxUICB; ++i)
            {
                auto pCB = new (std::nothrow) ConstantBuffer();
                if (pCB == nullptr)
                {
                    ELOG("Error : Out of memory.");
                    return false;
                }

                // 定数バッファ初期化
                if (!pCB->Init(singleton->device.Get(), singleton->pool[POOL_TYPE_RES], sizeof(CTransform) * 2))
                {
                    ELOG("Error : ConstantBuffer::Init() Failed.");
                    return false;
                }

                // カメラ設定.
                auto eyePos = Vector3(0, 1, 0);
                auto targetPos = Vector3::Zero;
                auto upward = Vector3::UnitZ;

                // 変換行列を設定
                auto ptr = pCB->GetPtr<CTransform>();
                ptr->World = Matrix::Identity;
                ptr->View = DirectX::XMMatrixLookAtLH(eyePos, targetPos, upward);
                ptr->Proj = singleton->proj;
                ptr->Alpha = 1;

                singleton->transform[DrawType::UI].push_back(pCB);
            }
            singleton->transform[DrawType::UI].shrink_to_fit();
        }
        return true;
    }

    void Dx12Wrapper::RenderBefore()
    {
        // ここはUpdate処理かな？
        auto pos = singleton->playerPos;
        // 更新処理
        {
            // 左手座標系でviewを作成
            singleton->view = DirectX::XMMatrixLookAtLH(singleton->eyePos, singleton->targetPos, singleton->upward);

            // 線で作られた四角データ
            for (int i = 0; i < singleton->drawRectData.size(); ++i)
            {
                auto line = singleton->transform[DrawType::Line][singleton->frameIndex * 2 + i]->GetPtr<CbLine>();
                line->View = singleton->view;
                // 線の座標系
                line->World = singleton->drawRectData[i].matrix;
                // 色設定
                line->Color = singleton->drawRectData[i].color;
            }

            // メッシュデータ
            for (int i = 0; i < singleton->drawMeshData.size(); ++i)
            {
                auto mesh = singleton->transform[DrawType::Basic][singleton->frameIndex * 2 + i]->GetPtr<CTransform>();
                mesh->View = singleton->view;
                mesh->World = singleton->drawMeshData[i].matrix;
            }

            // UIのメッシュデータ(view行列なし)
            for (int i = 0; i < singleton->drawMesh2DData.size(); ++i)
            {
                auto uiMesh = singleton->transform[DrawType::UI][singleton->frameIndex * 2 + i]->GetPtr<CTransform>();
                uiMesh->World = singleton->drawMesh2DData[i].matrix;
                uiMesh->Alpha = singleton->drawMesh2DData[i].alpha;
            }
        }
    }

    void Dx12Wrapper::RenderBegin()
    {
        // コマンドの記録を開始
        auto pCmd = singleton->gcList = singleton->cmdList.Reset();

        // ペラテスト用(マルチパスレンダリング)
        singleton->PreDrawToPera1();
    }

    void Dx12Wrapper::Render()
    {
        auto pCmd = singleton->gcList;

        // ここが描画処理
        {
            ID3D12DescriptorHeap* const heaps[] = {
            singleton->pool[POOL_TYPE_RES]->GetHeap()
            };

            pCmd->SetDescriptorHeaps(1, heaps);
            pCmd->RSSetViewports(1, &singleton->viewport);
            pCmd->RSSetScissorRects(1, &singleton->scissor);

            // 線描画用に設定
            pCmd->SetGraphicsRootSignature(singleton->BasicRS.Get());
            pCmd->SetPipelineState(singleton->pso[DrawType::Line].Get());
            // 線でできた四角を描画
            {
                pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP); // インデックスを使用せずにライン描画
                for (int i = 0; i < singleton->drawRectData.size(); ++i)
                {
                    pCmd->SetGraphicsRootConstantBufferView(0, singleton->transform[DrawType::Line][singleton->frameIndex * 2 + i]->GetAddress());

                    for (int j = 0; j < 4; ++j)
                    {
                        auto vbv = singleton->lineData[j].lineVB.GetView();
                        pCmd->IASetVertexBuffers(0, 1, &vbv);
                        pCmd->DrawInstanced(singleton->lineData[j].vertices.size(), 1, 0, 0);
                    }
                }
                // 描画し終わったらクリアする
                singleton->drawRectData.clear();
            }

            // メッシュ描画用に設定
            pCmd->SetPipelineState(singleton->pso[DrawType::Basic].Get());
            pCmd->SetGraphicsRootSignature(singleton->BasicRS.Get());
            pCmd->SetGraphicsRootConstantBufferView(3, singleton->dirLight->GetAddress());
            for (int i = 0; i < singleton->drawMeshData.size(); ++i)
            {
                pCmd->SetGraphicsRootConstantBufferView(0, singleton->transform[DrawType::Basic][singleton->frameIndex * 2 + i]->GetAddress());
                pCmd->SetGraphicsRootDescriptorTable(1, singleton->materials[singleton->drawMeshData[i].matType].GetTextureHandle(0, TU_BASE_COLOR));
                pCmd->SetGraphicsRootDescriptorTable(2, singleton->materials[Res::MaterialType::Gray2].GetTextureHandle(0, TU_BASE_COLOR));
                MyDX::MeshMgr::GetMesh(singleton->drawMeshData[i].mesh).Draw(pCmd);
            }
            // メッシュ描画リストのクリア
            singleton->drawMeshData.clear();

            // UIメッシュ
            pCmd->SetPipelineState(singleton->pso[DrawType::UI].Get());
            pCmd->SetGraphicsRootSignature(singleton->BasicRS.Get());
            for (int i = 0; i < singleton->drawMesh2DData.size(); ++i)
            {
                pCmd->SetGraphicsRootConstantBufferView(0, singleton->transform[DrawType::UI][singleton->frameIndex * 2 + i]->GetAddress());
                pCmd->SetGraphicsRootDescriptorTable(1, singleton->materials[singleton->drawMesh2DData[i].matType].GetTextureHandle(0, TU_BASE_COLOR));
                pCmd->SetGraphicsRootDescriptorTable(2, singleton->materials[Res::MaterialType::Black].GetTextureHandle(0, TU_BASE_COLOR));
                MyDX::MeshMgr::GetMesh(singleton->drawMesh2DData[i].mesh).Draw(pCmd);
            }
            singleton->drawMesh2DData.clear();
        }
    }

    void Dx12Wrapper::RenderEnd()
    {
        auto pCmd = singleton->gcList;

        DXTK12Font::RenderingFonts(pCmd);

        singleton->PostDrawToPera1();
        //singleton->DrawHorizontalBokeh();

        // 書き込み用リソースバリア設定.
        DirectX::TransitionResource(pCmd,
            singleton->colorTarget[singleton->frameIndex].GetResource(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

        // ディスクリプタ取得
        auto handleRTV = singleton->colorTarget[singleton->frameIndex].GetHandleRTV();
        auto handleDSV = singleton->depthTarget.GetHandleDSV();

        // レンダーターゲットを設定
        pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

        // レンダーターゲットをクリア
        pCmd->ClearRenderTargetView(handleRTV->HandleCPU, ClsClr, 0, nullptr);

        // 深度ステンシルビューをクリア
        pCmd->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        pCmd->SetGraphicsRootSignature(singleton->peraRS.Get());
        pCmd->SetPipelineState(singleton->peraPSO1.Get());
        pCmd->SetDescriptorHeaps(1, singleton->peraSRVHeap.GetAddressOf());
        auto handle = singleton->peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
        pCmd->SetGraphicsRootDescriptorTable(1, handle);
        //handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //pCmd->SetGraphicsRootDescriptorTable(1, handle);
        //handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //pCmd->SetGraphicsRootDescriptorTable(1, handle);
        //handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pCmd->IASetVertexBuffers(0, 1, &singleton->peraVBV);
        pCmd->DrawInstanced(4, 1, 0, 0);


        // 表示用リソースバリア設定.
        DirectX::TransitionResource(pCmd,
            singleton->colorTarget[singleton->frameIndex].GetResource(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);

        singleton->DrawShrinkTextureForBlur();

        // コマンドリストの記録を終了.
        pCmd->Close();

        // コマンドリストを実行.
        ID3D12CommandList* pLists[] = { pCmd };
        singleton->queue->ExecuteCommandLists(1, pLists);
        // 画面に表示.
        Present(1);

        // Fontのメモリリークを防止
        DXTK12Font::Commit(singleton->queue.Get());
    }

    void Dx12Wrapper::Term()
    {
        // マテリアル破棄
        {
            typedef EnumIterator < Res::MaterialType, Res::MaterialType::Begin, Res::MaterialType::End > typeItr;
            for (auto itr = typeItr(); itr != typeItr(Res::MaterialType::End); ++itr)
            {
                singleton->materials[*itr].Term();
            }
        }

        // 変換バッファ破棄
        {
            typedef EnumIterator < DrawType, DrawType::Begin, DrawType::End > typeItr;
            for (auto itr = typeItr(); itr != typeItr(DrawType::End); ++itr)
            {
                for (auto tf : singleton->transform[*itr])
                {
                    SafeTerm(tf);
                }
            }
            singleton->transform.clear();
        }

        DXTK12Font::OnTerm();       // フォント終了
    }

    void Dx12Wrapper::CreatePeraResource()
    {
        // 作成済みのヒープから情報を使いもう１枚作成
        auto heapDesc = singleton->pool[POOL_TYPE_RTV]->GetHeap()->GetDesc();

        // 使っているバックバッファの情報を利用
        auto* bbuff = singleton->colorTarget[0].GetResource();
        auto resDesc = bbuff->GetDesc();

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        // レンダリング時のクリア値と同じ値
        D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, ClsClr);

        // 1・2枚目
        for (auto& res : singleton->peraReses)
        {
            auto result = singleton->device->CreateCommittedResource(
                &heapProp,D3D12_HEAP_FLAG_NONE,
                &resDesc,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                &clearValue,IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
            );
            Helper::CheckResult(result);
        }

        // RTV用ヒープ作成
        heapDesc.NumDescriptors = 4;    // ペラ2枚分 // ブルーム用
        auto result = singleton->device->CreateDescriptorHeap(
            &heapDesc,
            IID_PPV_ARGS(singleton->peraRTVHeap.ReleaseAndGetAddressOf())
        );
        Helper::CheckResult(result);
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        // 1・2枚目
        auto handle = singleton->peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
        for (auto& res : singleton->peraReses)
        {
            singleton->device->CreateRenderTargetView(res.Get(),&rtvDesc,handle);
            handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
        // ブルーム用＆ブルーム用縮小バッファ用
        for (auto& res : singleton->bloomBuff)
        {
            singleton->device->CreateRenderTargetView(res.Get(), &rtvDesc, handle);
            handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // SRV用ヒープ作成
        heapDesc.NumDescriptors = 4;    // ペラ2枚、ブルーム2枚
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        result = singleton->device->CreateDescriptorHeap(&heapDesc,
            IID_PPV_ARGS(singleton->peraSRVHeap.ReleaseAndGetAddressOf()));
        Helper::CheckResult(result);


        // 本だと先にインクリメントしているが、多分SRVを作成後に書く場合の書き方
        handle = singleton->peraSRVHeap->GetCPUDescriptorHandleForHeapStart();
        //// ぼけ定数バッファービュー設定
        //D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        //cbvDesc.BufferLocation = singleton->bokehParamResource->GetGPUVirtualAddress();
        //cbvDesc.SizeInBytes = singleton->bokehParamResource->GetDesc().Width;
        //// 1枚目
        //singleton->device->CreateConstantBufferView(&cbvDesc, handle);
        //handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


        // SRVを作成
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Format = rtvDesc.Format;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


        // 1・2枚目
        for (auto& res : singleton->peraReses)
        {
            singleton->device->CreateShaderResourceView(res.Get(), &srvDesc, handle);
            handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
        // ブルームテクスチャ、高輝度縮小テクスチャ
        for (auto& res : singleton->bloomBuff)
        {
            singleton->device->CreateShaderResourceView(res.Get(), &srvDesc, handle);
            handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
    }

    void Dx12Wrapper::PreDrawToPera1()
    {
        auto pCmd = singleton->gcList;
        auto handleDSV = singleton->depthTarget.GetHandleDSV();

        // レンダーターゲットを切り替える前にリソースのステートをレンダーターゲットに切り替える
        for (auto& res : singleton->peraReses)
        {
            Barrier(res.Get(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            );
        }
        Barrier(singleton->bloomBuff[0].Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        CD3DX12_CPU_DESCRIPTOR_HANDLE handles[3]{};
        D3D12_CPU_DESCRIPTOR_HANDLE baseH = singleton->peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
        auto incSize = singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        uint32_t offset = 0;
        for (auto& handle : handles)
        {
            handle.InitOffsetted(baseH, offset);
            offset += incSize;
        }
        pCmd->OMSetRenderTargets(_countof(handles), handles, false, &handleDSV->HandleCPU);
        for (int i = 0; i < _countof(handles); ++i)
        {
            pCmd->ClearRenderTargetView(handles[i], ClsClr, 0, nullptr);
        }

        pCmd->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    void Dx12Wrapper::PostDrawToPera1()
    {
        for (auto& res : singleton->peraReses)
        {
            Barrier(res.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        }
        //Barrier(singleton->bloomBuff[0].Get(),
        //    D3D12_RESOURCE_STATE_RENDER_TARGET,
        //    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        //);
    }

    void Dx12Wrapper::CreatePeraPipeline()
    {
        D3D12_DESCRIPTOR_RANGE range[2] = {};
        range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;   // c
        range[0].BaseShaderRegister = 0;
        range[0].NumDescriptors = 1;

        // 通常、法線、高輝度、縮小高輝度
        range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;   // t
        range[1].BaseShaderRegister = 0;
        range[1].NumDescriptors = 4;                            // t0、t1、t2、t3

        D3D12_ROOT_PARAMETER rp[2] = {};
        rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rp[0].DescriptorTable.pDescriptorRanges = &range[0];
        rp[0].DescriptorTable.NumDescriptorRanges = 1;

        rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rp[1].DescriptorTable.pDescriptorRanges = &range[1];
        rp[1].DescriptorTable.NumDescriptorRanges = 1;

        D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);

        // ルートシグネチャを作成
        D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
        rsDesc.NumParameters = _countof(rp);
        rsDesc.pParameters = rp;
        rsDesc.NumStaticSamplers = 1;
        rsDesc.pStaticSamplers = &sampler;
        rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> rsBlob;
        ComPtr<ID3DBlob> erroBlob;
        auto result = D3D12SerializeRootSignature(
            &rsDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            rsBlob.ReleaseAndGetAddressOf(),
            erroBlob.ReleaseAndGetAddressOf()
        );
        Helper::CheckResult(result);
        result = singleton->device->CreateRootSignature(
            0,
            rsBlob->GetBufferPointer(),
            rsBlob->GetBufferSize(),
            IID_PPV_ARGS(singleton->peraRS.ReleaseAndGetAddressOf())
        );
        Helper::CheckResult(result);

        // ペラ用シェーダー読み込み
        ComPtr<ID3DBlob> vsPeraBlob;
        ComPtr<ID3DBlob> psPeraBlob;
        singleton->LoadShader(L"Shader/cso/PeraVS.cso", vsPeraBlob);
        singleton->LoadShader(L"Shader/cso/PeraPS.cso", psPeraBlob);
        //result = D3DCompileFromFile(
        //    L"Shader/PeraPS.hlsl", nullptr,
        //    D3D_COMPILE_STANDARD_FILE_INCLUDE,
        //    "HorizontalBokehPS", "ps_5_0", 0, 0,
        //    psPeraBlob.ReleaseAndGetAddressOf(),
        //    erroBlob.ReleaseAndGetAddressOf()
        //);
        //CheckResult(result, erroBlob.Get());

        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
        gpsDesc.VS = { vsPeraBlob->GetBufferPointer(),vsPeraBlob->GetBufferSize() };
        gpsDesc.PS = { psPeraBlob->GetBufferPointer(),psPeraBlob->GetBufferSize() };
        gpsDesc.DepthStencilState.StencilEnable = false;
        gpsDesc.DepthStencilState.DepthEnable = false;
        D3D12_INPUT_ELEMENT_DESC layout[2] = {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
        };
        gpsDesc.InputLayout.NumElements = _countof(layout);
        gpsDesc.InputLayout.pInputElementDescs = layout;
        gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        gpsDesc.NumRenderTargets = 1;
        gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        gpsDesc.SampleDesc.Count = 1;
        gpsDesc.SampleDesc.Quality = 0;
        gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        gpsDesc.pRootSignature = singleton->peraRS.Get();

        // 1枚目用パイプライン生成
        result = singleton->device->CreateGraphicsPipelineState(
            &gpsDesc,
            IID_PPV_ARGS(singleton->peraPSO1.ReleaseAndGetAddressOf())
        );
        Helper::CheckResult(result);

        //// 2枚目用ピクセルシェーダー
        //result = D3DCompileFromFile(
        //    L"Shader/PeraPS.hlsl", nullptr,
        //    D3D_COMPILE_STANDARD_FILE_INCLUDE,
        //    "VerticalBokehPS", "ps_5_0", 0, 0,
        //    psPeraBlob.ReleaseAndGetAddressOf(),
        //    erroBlob.ReleaseAndGetAddressOf()
        //);
        //CheckResult(result, erroBlob.Get());
        //// 2枚目用パイプライン
        //gpsDesc.PS = CD3DX12_SHADER_BYTECODE(psPeraBlob.Get());
        //result = singleton->device->CreateGraphicsPipelineState(
        //    &gpsDesc,
        //    IID_PPV_ARGS(singleton->peraPSO2.ReleaseAndGetAddressOf())
        //);
        //CheckResult(result);


        // ブルーム用
        result = D3DCompileFromFile(
            L"Shader/PeraPS.hlsl", nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "BlurPS", "ps_5_0", 0, 0,
            psPeraBlob.ReleaseAndGetAddressOf(),
            erroBlob.ReleaseAndGetAddressOf()
        );
        Helper::CheckResult(result, erroBlob.Get());
        // 2枚目用パイプライン
        gpsDesc.PS = CD3DX12_SHADER_BYTECODE(psPeraBlob.Get());
        gpsDesc.NumRenderTargets = 2;
        gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        gpsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
        result = singleton->device->CreateGraphicsPipelineState(
            &gpsDesc,
            IID_PPV_ARGS(singleton->blurPSO.ReleaseAndGetAddressOf())
        );
        Helper::CheckResult(result);
    }

    void Dx12Wrapper::CreatePeraVertex()
    {
        struct PeraVertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
        };

        PeraVertex pv[4] =
        { {{-1,-1,0.1},{0,1}},  // 左上
        { {-1,1,0.1},{0,0} },   // 右上
        { {1,-1,0.1},{1,1} },   // 右下
        { {1,1,0.1},{1,0} } };  // 左下

        // 頂点バッファの作成
        auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));
        auto result = singleton->device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(singleton->peraVB.ReleaseAndGetAddressOf()));
        Helper::CheckResult(result);
 
        // マップ
        PeraVertex* mappedPera = nullptr;
        singleton->peraVB->Map(0, nullptr, (void**)&mappedPera);
        std::copy(std::begin(pv), std::end(pv), mappedPera);
        singleton->peraVB->Unmap(0, nullptr);

        singleton->peraVBV.BufferLocation = singleton->peraVB->GetGPUVirtualAddress();
        singleton->peraVBV.SizeInBytes = sizeof(pv);
        singleton->peraVBV.StrideInBytes = sizeof(PeraVertex);
    }

    void Dx12Wrapper::CreateBokeParamResourcec()
    {
        auto weights = Helper::GetGaussianWeight(8, 5.0f);
        auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(Helper::AligmentedValue(sizeof(weights[0]) * weights.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
        auto result = singleton->device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(singleton->bokehParamResource.ReleaseAndGetAddressOf())
        );
        Helper::CheckResult(result);

        float* mappedWeight = nullptr;
        result = singleton->bokehParamResource->Map(0, nullptr, (void**)&mappedWeight);
        copy(weights.begin(), weights.end(), mappedWeight);
        singleton->bokehParamResource->Unmap(0, nullptr);
    }

    void Dx12Wrapper::DrawHorizontalBokeh()
    {
        auto pCmd = singleton->gcList;
        Barrier(singleton->peraReses[1].Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        auto rtvHeapPointer = singleton->peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
        // 2枚目RTVなので進ませてレンタゲを割り当てる
        rtvHeapPointer.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        pCmd->OMSetRenderTargets(1, &rtvHeapPointer, false, nullptr);
        // クリアカラー
        pCmd->ClearRenderTargetView(rtvHeapPointer, ClsClr, 0, nullptr);

        D3D12_VIEWPORT vp = CD3DX12_VIEWPORT(0.0f, 0.0f, singleton->width, singleton->height);
        pCmd->RSSetViewports(1, &vp);//ビューポート

        CD3DX12_RECT rc(0, 0, singleton->width, singleton->height);
        pCmd->RSSetScissorRects(1, &rc);//シザー(切り抜き)矩形

        pCmd->SetGraphicsRootSignature(singleton->peraRS.Get());
        pCmd->SetDescriptorHeaps(1, singleton->peraSRVHeap.GetAddressOf());

        auto handle = singleton->peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
        pCmd->SetGraphicsRootDescriptorTable(0, handle);

        handle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        pCmd->SetGraphicsRootDescriptorTable(1, handle); 

        pCmd->SetPipelineState(singleton->peraPSO2.Get());
        pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pCmd->IASetVertexBuffers(0, 1, &singleton->peraVBV);
        pCmd->DrawInstanced(4, 1, 0, 0);

        Barrier(singleton->peraReses[1].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    }

    void Dx12Wrapper::CreateBloomBuffer()
    {
        auto& buff = singleton->colorTarget[0];
        auto resDesc = buff.GetDesc();
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Color[0] = clearValue.Color[1] = clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;
        clearValue.Format = resDesc.Format; 
        for (auto& res : singleton->bloomBuff) {
            auto result = singleton->device->CreateCommittedResource(
                &heapProp,D3D12_HEAP_FLAG_NONE,
                &resDesc,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                &clearValue,IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
            );
            resDesc.Width >>= 1;
            Helper::CheckResult(result);
        }
    }

    void Dx12Wrapper::DrawShrinkTextureForBlur()
    {
        auto pCmd = singleton->gcList;
        pCmd->SetPipelineState(singleton->blurPSO.Get());
        pCmd->SetGraphicsRootSignature(singleton->peraRS.Get());
        // 頂点バッファ
        pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pCmd->IASetVertexBuffers(0, 1, &singleton->peraVBV);
        // 高輝度成分バッファはシェーダリソースに
        Barrier(
            singleton->bloomBuff[0].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        // 縮小バッファはレンタゲに
        Barrier(
            singleton->bloomBuff[1].Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        auto rtvHandle = singleton->peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
        auto srvHandle = singleton->peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
        // 4つ目に移動
        rtvHandle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 3;
        // レンタゲセット
        pCmd->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
        // テクスチャは1枚目の3つ目のレンタゲ
        srvHandle.ptr += singleton->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;
        // 1パス目高輝度テクスチャとして使用
        pCmd->SetDescriptorHeaps(1, singleton->peraSRVHeap.GetAddressOf());
        pCmd->SetGraphicsRootDescriptorTable(0, srvHandle);

        auto desc = singleton->bloomBuff[0]->GetDesc();
        D3D12_VIEWPORT vp = {};
        D3D12_RECT sr = {};
        vp.MaxDepth = 1.0f;
        vp.MinDepth = 0.0f;
        vp.Height = desc.Height / 2.0f;
        vp.Width = desc.Width / 2.0f;
        sr.top = 0;
        sr.left = 0;
        sr.right = vp.Width;
        sr.bottom = vp.Height;

        auto weights = Helper::GetGaussianWeight(8, 5.0f);

        for (int i = 0; i < 8; ++i) 
        {
            pCmd->RSSetViewports(1, &vp);
            pCmd->RSSetScissorRects(1, &sr);
            pCmd->DrawInstanced(4, 1, 0, 0);
            //書いたら下にずらして次を書く準備
            sr.top += vp.Height;
            vp.TopLeftX = 0;
            vp.TopLeftY = sr.top;
            //幅も高さも半分に
            vp.Width /= 2;
            vp.Height /= 2;
            sr.bottom = sr.top + vp.Height;
        }

        //for (auto weight : weights)
        //{
        //    pCmd->RSSetViewports(1, &vp);
        //    pCmd->RSSetScissorRects(1, &sr);
        //    pCmd->DrawInstanced(4, 1, 0, 0);
        //}

        //縮小バッファをシェーダリソースに
        Barrier(singleton->bloomBuff[1].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    void Dx12Wrapper::LoadShader(LPCWSTR _Path, ComPtr<ID3DBlob>& _Blob)
    {
        auto hr = D3DReadFileToBlob(_Path, _Blob.GetAddressOf());
        Helper::CheckResult(hr);
    }

    void Dx12Wrapper::Barrier(ID3D12Resource* _P, D3D12_RESOURCE_STATES _Before, D3D12_RESOURCE_STATES _After)
    {
        auto pCmd = singleton->gcList;
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_P, _Before, _After, 0);
        pCmd->ResourceBarrier(1, &barrier);
    }
}