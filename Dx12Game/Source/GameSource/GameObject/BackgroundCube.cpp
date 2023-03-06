#include "BackgroundCube.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	BackgroundCube::BackgroundCube() :
		Base(Tag::BackGround, "BackgroundCube"),
		LifeTime(20.0f),
		Speed(30.0f),
		MoveDir(Vector3(0,1,0))
	{
		this->transform->scale = Vector3(10.0f);
	}

	BackgroundCube::~BackgroundCube()
	{
	}

	void BackgroundCube::Init()
	{
		this->lifeTime = LifeTime;
	}

	void BackgroundCube::Update()
	{
		// 生存時間を経過したら無効化して処理終了
		if (lifeTime <= 0)
		{
			this->SetActive(false);
			return;
		}
		else
		{
			this->lifeTime -= Sys::Timer::GetDeltaTime();
		}

		// 移動処理
		this->transform->position += this->MoveDir * Sys::Timer::GetDeltaTime() * this->Speed;

		this->transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateTranslation(this->transform->position);
	}

	void BackgroundCube::Draw()const
	{
		MyDX::Dx12Wrapper::DrawMesh(this->transform->matrix, Res::MeshType::Cube, Res::MaterialType::Gray2);
	}
}