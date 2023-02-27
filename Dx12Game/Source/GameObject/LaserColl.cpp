#include "LaserColl.h"

#include <cmath>
#include "SphereCollider.h"

namespace GameObject
{
	LaserColl::LaserColl():
		Base(Tag::Enemy, "LaserColl")
	{
		this->sphColl = AddComponent<Component::SphColl>();
	}
	LaserColl::~LaserColl()
	{
	}
	void LaserColl::Update()
	{
		// 指定された時間で最大サイズになる
		const float MaxTime = 1;
		const float MaxRadius = 3;
		if(this->timeCounter += Sys::Timer::GetDeltaTime(); timeCounter >= MaxTime)
		{ this->timeCounter = MaxTime; }
		this->sphColl->radius = std::lerp(0, MaxRadius, this->timeCounter);
	}

	void LaserColl::Init()
	{
		this->sphColl->radius = 0;
		this->timeCounter = 0;
	}

	void LaserColl::OnTriggerEnter(Base* _Other)
	{
	}
}