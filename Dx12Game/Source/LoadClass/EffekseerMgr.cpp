#include "EffekseerMgr.h"
#include "Dx12Wrapper.h"
#include "EfkData.h"
#include "Logger.h"

#include "Timer.h"

// Link Effekseer
#if defined(DEBUG) || (_DEBUG)
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#pragma comment(lib,"LLGId.lib")
# else
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"LLGI.lib")
#endif

static const int MaxEffectDisplay = 500;		// ��x�ɉ�ʏ�ɑ��݂ł���G�t�F�N�g��
static const int MaxParticleDisplay = 65536;	// ��x�ɉ�ʏ�ɑ��݂ł���p�[�e�B�N����


namespace Effect
{
	EffekseerMgr* EffekseerMgr::singleton = nullptr;

	EffekseerMgr::EffekseerMgr() :
		efkRenderer(nullptr),
		efkMgr(nullptr),
		efkMemPool(nullptr),
		efkCmdList(nullptr),
		effect(),
		loopPlayData()
	{
		singleton = nullptr;
	}

	EffekseerMgr::~EffekseerMgr()
	{
	}

	void EffekseerMgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new EffekseerMgr;
			Init();
		}
	}

	void EffekseerMgr::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void EffekseerMgr::Init()
	{
		DXGI_FORMAT bbFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
		singleton->efkRenderer = EffekseerRendererDX12::Create(
			MyDX::Dx12Wrapper::GetDevice(),		// DirectX12�̃f�o�C�X
			MyDX::Dx12Wrapper::GetQue(),        // DirectX12�̃R�}���h�L���[
			2,                                  // �o�b�N�o�b�t�@�[�̐�
			bbFormats,                          // �����_�[�^�[�Q�b�g�t�H�[�}�b�g
			1,                                  // �����_�[�^�[�Q�b�g��
			DXGI_FORMAT_D32_FLOAT,				// �f�v�X�͂Ȃ�
			false,                              // ���΃f�v�X�͂Ȃ�
			MaxParticleDisplay);              // �ő�p�[�e�B�N���̐�

		singleton->efkMgr = Effekseer::Manager::Create(MaxEffectDisplay);  // �ő�C���X�^���X��
		// �G�t�F�N�V�A�ݒ�
		SetUpEfkMgr();
		// DX12�p�̏�����
		InitDX12Renderer();
		// �G�t�F�N�g�ǂݍ���
					// �S�Ẵ��b�V���f�[�^�����[�h
		for (auto type = MyRes::EfkType::Begin; type != MyRes::EfkType::End; ++type)
		{
			// ?�������̂�ǂݍ���ł��Ȃ�����r����
			for (auto comparison = MyRes::EfkType::Begin; comparison != MyRes::EfkType::End; ++comparison)
			{
				if ((type != comparison) && (MyRes::EfkData::pathMap[type] == MyRes::EfkData::pathMap[comparison]))
				{
					ELOG("Error : Efk File Double Load");
				}
			}
			LoadEffect(type);
		}
	}

	int EffekseerMgr::PlayEffect(const MyRes::EfkType _Type, const DirectX::XMFLOAT3& _Pos, bool _IsLoop, const DirectX::XMFLOAT3 _Angle)
	{
		if (_IsLoop)
		{
			LoopPlayData data;
			data.type = _Type;
			data.handle = singleton->efkMgr->Play(singleton->effect[_Type], _Pos.x, _Pos.y, _Pos.z);
			singleton->loopPlayData.emplace_back(data);
			// �p�x�̐ݒ�
			singleton->efkMgr->SetRotation(data.handle, _Angle.x, _Angle.y, _Angle.z);

			//  ���x���f���^�^�C���ɍ��킹��
			singleton->efkMgr->SetSpeed(data.handle, MySys::Timer::GetDeltaTime() * 100);

			return data.handle;
		}
		else
		{
			int temp = singleton->efkMgr->Play(singleton->effect[_Type], _Pos.x, _Pos.y, _Pos.z);
			// �p�x�̕ύX
			singleton->efkMgr->SetRotation(temp, _Angle.x, _Angle.y, _Angle.z);

			//  ���x���f���^�^�C���ɍ��킹��
			singleton->efkMgr->SetSpeed(temp, MySys::Timer::GetHitStopTime() * 100);

			return temp;
		}
	}

	void EffekseerMgr::StopEffect(int _Handle)
	{
		// �w�肳�ꂽ�G�t�F�N�g���~
		singleton->efkMgr->StopEffect(_Handle);
		// ���[�v�Đ����̃G�t�F�N�g�f�[�^����폜
		for (auto itr = singleton->loopPlayData.begin(); itr != singleton->loopPlayData.end(); ++itr)
		{
			if (itr->handle == _Handle)
			{
				singleton->loopPlayData.erase(itr);
				return;
			}
		}
	}

	void EffekseerMgr::Draw()
	{
		// �G�t�F�N�g�`��
		singleton->efkMgr->Update();		// �}�l�[�W���̍X�V(�X�V����)
		singleton->efkMemPool->NewFrame();	// �`�悷�郌���_�[�^�[�Q�b�g��I��
		// �`�揈���̊J�n
		EffekseerRendererDX12::BeginCommandList(singleton->efkCmdList, MyDX::Dx12Wrapper::GetCmdList());
		// �`��O����
		singleton->efkRenderer->BeginRendering();
		// �G�t�F�N�g�`��
		singleton->efkMgr->Draw();
		// �`��㏈��
		singleton->efkRenderer->EndRendering();
		// �`�揈���I��
		EffekseerRendererDX12::EndCommandList(singleton->efkCmdList);
	}

	int EffekseerMgr::UpdateLoopPlayEffect(const DirectX::XMFLOAT3& _Pos, const int _Handle, const float _DeltaTime, const DirectX::XMFLOAT3& _Angle)
	{
		//  �n���h���ɂ���Ďw�肳�ꂽ�G�t�F�N�g�����[�v�Đ���������
		for (auto itr = singleton->loopPlayData.begin(); itr != singleton->loopPlayData.end(); ++itr)
		{
			//  ���[�v�Đ������G�t�F�N�g�̍Đ����I����Ă�����A�ŏ�����Đ����Ȃ����B
			if (itr->handle == _Handle && !singleton->efkMgr->Exists(_Handle))
			{
				//  �Đ�
				itr->handle = singleton->efkMgr->Play(singleton->effect[itr->type], _Pos.x, _Pos.y, _Pos.z);
				//  �p�x�̕ύX
				singleton->efkMgr->SetRotation(itr->handle, _Angle.x, _Angle.y, _Angle.z);

				return itr->handle;
			}
		}
		//  �ʒu�̕ύX
		singleton->efkMgr->SetLocation(_Handle, _Pos.x, _Pos.y, _Pos.z);
		//  �p�x�̕ύX
		singleton->efkMgr->SetRotation(_Handle, _Angle.x, _Angle.y, _Angle.z);
		//  ���x���f���^�^�C���ɍ��킹��
		singleton->efkMgr->SetSpeed(_Handle, _DeltaTime);
		return _Handle;
	}

	void EffekseerMgr::SetPlaySpeed(const int _Handle, const float _PlaySpeed)
	{
		singleton->efkMgr->SetSpeed(_Handle, _PlaySpeed);
	}

	void EffekseerMgr::SyncronizeEffekseerCamera()
	{
		// Effekseer�p�r���[�s��
		Effekseer::Matrix44 fkViewMat;
		// Effekseer�p�v���W�F�N�V�����s��
		Effekseer::Matrix44 fkProjMat;

		// �f�o�C�X�̃r���[�s��ƃv���W�F�N�V�����s����R�s�[
		DirectX::XMMATRIX view = MyDX::Dx12Wrapper::GetViewMatrix();
		DirectX::XMMATRIX proj = MyDX::Dx12Wrapper::GetProjMatrix();

		//  Effekseer�p�ƃf�o�C�X��̂��ꂼ��̍s��𓯊�
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fkViewMat.Values[i][j] = view.r[i].m128_f32[j];
				fkProjMat.Values[i][j] = proj.r[i].m128_f32[j];
			}
		}

		// ���ꂼ��̍s��ɏ]���ăJ������ݒ�
		singleton->efkRenderer->SetCameraMatrix(fkViewMat);
		singleton->efkRenderer->SetProjectionMatrix(fkProjMat);
	}

	void EffekseerMgr::SetUpEfkMgr()
	{
		// ���W�n������n��(�N���C�A���g���ɍ��킹��)
		singleton->efkMgr->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
		// �`��p�C���X�^���X����`��@�\��ݒ�
		singleton->efkMgr->SetSpriteRenderer(singleton->efkRenderer->CreateSpriteRenderer());
		singleton->efkMgr->SetRibbonRenderer(singleton->efkRenderer->CreateRibbonRenderer());
		singleton->efkMgr->SetRingRenderer(singleton->efkRenderer->CreateRingRenderer());
		singleton->efkMgr->SetTrackRenderer(singleton->efkRenderer->CreateTrackRenderer());
		singleton->efkMgr->SetModelRenderer(singleton->efkRenderer->CreateModelRenderer());
		// �`��p�C���X�^���X����e�N�X�`���̓ǂݍ��݋@�\��ݒ�(�Ǝ��g�����\)
		singleton->efkMgr->SetTextureLoader(singleton->efkRenderer->CreateTextureLoader());
		singleton->efkMgr->SetModelLoader(singleton->efkRenderer->CreateModelLoader());
		singleton->efkMgr->SetMaterialLoader(singleton->efkRenderer->CreateMaterialLoader());
	}

	void EffekseerMgr::InitDX12Renderer()
	{
		// DX12���L�̏���
		singleton->efkMemPool = EffekseerRenderer::CreateSingleFrameMemoryPool(singleton->efkRenderer->GetGraphicsDevice());
		singleton->efkCmdList = EffekseerRenderer::CreateCommandList(singleton->efkRenderer->GetGraphicsDevice(), singleton->efkMemPool);
		singleton->efkRenderer->SetCommandList(singleton->efkCmdList);
	}

	void EffekseerMgr::LoadEffect(const MyRes::EfkType _Type)
	{
		const float Magnification = 1.0f;

		singleton->effect[_Type] = Effekseer::Effect::Create(
			singleton->efkMgr,
			(const EFK_CHAR*)MyRes::EfkData::pathMap[_Type],
			Magnification,
			(const EFK_CHAR*)L"Resource/Effekseer/Texture"
		);

		// �G���[�`�F�b�N
		if (singleton->effect[_Type] == nullptr)
		{
			ELOG("Error : Missing Efk Load");
		}
	}
}