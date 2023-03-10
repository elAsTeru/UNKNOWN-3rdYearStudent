#include "CubeGenerator.h"
#include "Dx12Wrapper.h"
#include "CLineMove.h"
#include "CGenerator.h"

namespace GameObject
{
	CubeGenerator::CubeGenerator() :
		Base(Tag::Untagged, "CubeGenerator"),
		cLineMove(AddComponent<Component::CLineMove>()),
		cGenerator(AddComponent<Component::CGenerator>())
	{
		this->Init();
	}

	CubeGenerator::~CubeGenerator()
	{

	}

	void CubeGenerator::Init()
	{
		// 線上移動コンポーネントの初期化
		// 外で設定
		// 発生コンポーネントの初期化
		this->cGenerator->Init();
		this->cGenerator->objName = "BackgroundCube";
		this->cGenerator->interval = 0.2f;
	}

	void CubeGenerator::Update()
	{
		this->cLineMove->Update();
		this->cGenerator->Update();
	}

	void CubeGenerator::Draw() const
	{
//		// コメントアウト化を忘れた場合にデバッグ時のみ可視化
//#if defined(DEBUG) || (_DEBUG)
//		this->transform->matrix
//			= Matrix::CreateScale(this->transform->scale)
//			* Matrix::CreateTranslation(this->transform->position);
//		MyDX::Dx12Wrapper::DrawBasicMesh({ this->transform->matrix, Res::MeshType::Sphere , Res::MaterialType::White });
//#endif
	}

	void CubeGenerator::SetInterval(const float _Time)
	{
		this->cGenerator->interval = _Time;
	}
} 