#include "BackgroundCube.h"
#include "Dx12Wrapper.h"

namespace GameObject
{
	BackgroundCube::BackgroundCube() :
		Base(Tag::BackGround, "BackgroundCube")
	{
	}

	BackgroundCube::~BackgroundCube()
	{
	}

	void BackgroundCube::Init()
	{
		this->speed = 30;
		this->dir = Vector3(0, 1, 0);
		this->dir.Normalize();
		this->duration = 20;
		this->timeCounter = 0;
		this->transform->scale = {10.0f};
	}

	void BackgroundCube::Update()
	{
		// 持続時間を終了したら、非表示にする
		if (this->timeCounter += MySys::Timer::GetDeltaTime(); this->timeCounter >= this->duration)
		{
			this->SetActive(false);
		}
		// 移動
		transform->position += dir * MySys::Timer::GetDeltaTime() * speed;

		Draw();
	}

	void BackgroundCube::Draw()
	{
		transform->matrix
			= Matrix::CreateScale(this->transform->scale)
			* Matrix::CreateTranslation(this->transform->position);
		MyDX::Dx12Wrapper::DrawBasicMesh({ transform->matrix, MyRes::MeshType::Cube, 6 });
	}
}