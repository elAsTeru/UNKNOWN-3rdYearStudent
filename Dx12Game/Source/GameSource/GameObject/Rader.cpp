#include "Rader.h"
#include "CSphColl.h"
#include "AvoidTracker.h"

namespace GameObject
{
	Rader::Rader() :
		Base(Tag::Enemy, "Rader"),
		sphColl(AddComponent<Component::CSphColl>()),
		dist(2.0f)
	{
		// Init Component Variable
		this->transform->scale = { 1.5f };
		this->sphColl->radius = 2.0f;
	}

	void Rader::Update()
	{
		// 避け敵の移動方向に設置
		this->transform->position = DirVec * this->dist + this->parentObj->transform->position;
	}

	void Rader::Init()
	{
		this->sphColl->isTrigger = false;
		this->sphColl->isEnable = false;
	}

	void Rader::OnTriggerEnter(Base* _Other)
	{
		if (_Other->tag == Tag::Bullet)
		{
			// 2Dの外積を使用することで左右のチェックに使用することが出来る
			// v1×v2= x1*y2-x2*y1 = |v1||v2|sin(θ)
			// 符号を見ることで
			// v1 の 左右どちらにあるのかを判定できる、0の場合は平行

			// thisから見て弾は右にあるか左にあるか外積を利用して求める
			Vector2 forward = { this->DirVec.x,this->DirVec.z };																				// thisが見ている方向(正規化済)
			Vector2 bullet = { _Other->transform->position.x - this->transform->position.x , _Other->transform->position.z - this->transform->position.z };	// 弾の方向

			// 2Dの外積を行う
			float cross = static_cast<float>((forward.x * bullet.y) - (forward.y * bullet.x));	// ①
			
			// それぞれのベクトルの大きさだけを求める
			float vecALength = Vector2(forward).Length();		// ②
			float vecBLength = Vector2(bullet).Length();		// ③

			// ①②③を使用して、sinθの値を求める
			float dir = std::sin(cross / (vecALength * vecBLength));

			// 左に避ける
			if (dir <= 0.0f)
			{
				parentObj->Avoid(true);
			}
			// 右に避ける
			else
			{
				parentObj->Avoid(false);
			}
		}
	}
}