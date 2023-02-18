#pragma once

// Effekseer
#include <Effekseer/Effekseer.h>
#include <Effekseer/EffekseerRendererDX12.h>
#include <unordered_map>
#include <list>
#include <DirectXMath.h>
#include "EfkType.h"

namespace Effect
{
	class EffekseerMgr
	{
		EffekseerMgr();
		~EffekseerMgr();
		static EffekseerMgr* singleton;
	public:
		// Public Method
		static void OnInit();
		static void OnTerm();

		static void Init();
		static int PlayEffect(const MyRes::EfkType _Type, const DirectX::XMFLOAT3& _Pos, bool _IsLoop, const DirectX::XMFLOAT3 _Angle = { 0.0f,0.0f,0.0f });
		static void StopEffect(int _Handle);
		static void Draw();
		static int UpdateLoopPlayEffect(const DirectX::XMFLOAT3& _Pos, const int _Handle, const float _DeltaTime, const DirectX::XMFLOAT3& _Angle = { 0.0f,0.0f,0.0f });
		static void SetPlaySpeed(const int _Handle, const float _PlaySpeed);
		/// <summary>
		/// �J�����ƃG�t�F�N�g�̈ʒu�𓯊�
		/// </summary>
		static void SyncronizeEffekseerCamera();

	private:
		// Private Struct
		struct LoopPlayData
		{
			Effekseer::Handle handle;
			MyRes::EfkType type;
		};

		// Private Variable
		// �K�v���ڂ̊�{
		Effekseer::RefPtr <EffekseerRenderer::Renderer>					efkRenderer;	// �����_��
		Effekseer::RefPtr <Effekseer::Manager>							efkMgr;			// �}�l�W��
		// DX12�EVulkan�Emetal�Ȃǂ̃R�}���h���X�g�n�ւ̑Ή��ɕK�{
		Effekseer::RefPtr <EffekseerRenderer::SingleFrameMemoryPool>	efkMemPool;		// �������v�[��
		Effekseer::RefPtr <EffekseerRenderer::CommandList>				efkCmdList;		// �R�}���h���X�g
		// �G�t�F�N�g�Đ��ɕK�v
		std::unordered_map<MyRes::EfkType, Effekseer::RefPtr <Effekseer::Effect>> effect;// �G�t�F�N�g�{��
		std::list<LoopPlayData> loopPlayData;


		// Private Method
		static void SetUpEfkMgr();
		static void InitDX12Renderer();
		static void LoadEffect(const MyRes::EfkType _Type);
	};
}