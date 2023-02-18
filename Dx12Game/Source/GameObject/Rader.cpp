#include "Rader.h"

#include <cmath>

#include "CTracking.h"
#include "CFixPos.h"
#include "SphereCollider.h"
#include "AvoidTracker.h"

#include "Logger.h"

namespace GameObject
{
	Rader::Rader():
		Base(Tag::Enemy, "Rader")
	{
		this->transform->scale = { 1.5f };

		this->sphColl = AddComponent<Component::SphColl>();
		this->sphColl->radius = 2.0f;
	}

	Rader::~Rader()
	{

	}

	void Rader::Update()
	{
		//sphColl->DebugUpdate();

		// 除け敵の移動方向に設置
		this->transform->position = forceNorVec3 * 2 + parentObj->transform->position;

	/*	this->transform->position
			= parentObj->transform->rotation
			* parentObj->transform->position;*/

		//parentObj->transform->position;
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
			// v1 の 左右どちらにあるのかを判定できる、0の場合は平行である

			// thisから見て弾は右にあるか左にあるか外積を利用して求める
			Vector2 forward = { this->forceNorVec3.x,this->forceNorVec3.z };																				// thisが見ている方向(正規化済)
			Vector2 bullet = { _Other->transform->position.x - this->transform->position.x , _Other->transform->position.z - this->transform->position.z };	// 弾の方向

			// 2Dの外積を行う
			float cross = static_cast<float>((forward.x * bullet.y) - (forward.y * bullet.x));	// ①
			
			// それぞれのベクトルの大きさだけを求める
			float vecALength = Vector2(forward).Length();		// ②
			float vecBLength = Vector2(bullet).Length();		// ③

			// ①②③を使用して、sinθの値を求める
			float dir = std::sin(cross / (vecALength * vecBLength));

			if (dir > 0.0f)
			{
				parentObj->Avoid(1);
				//OutputLog("左 : ");
			}
			else if (dir < 0.0f)
			{
				parentObj->Avoid(-1);
				//OutputLog("右 : ");
			}
			else
			{
				parentObj->Avoid(0);
				//OutputLog("平行 : ");
			}
			//OutputLog("%f\n", dir);
		}
	}
}