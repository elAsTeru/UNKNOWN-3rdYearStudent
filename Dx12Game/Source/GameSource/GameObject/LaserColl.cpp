#include "LaserColl.h"
#include "Dx12Wrapper.h"
#include <cmath>
#include "CSphColl.h"

namespace GameObject
{
	LaserColl::LaserColl():
		Base(Tag::Enemy, "LaserColl"),
		MaxTime(1.0f),
		MaxRadius(3.0f)
	{
		this->sphColl = AddComponent<Component::CSphColl>();
	}
	LaserColl::~LaserColl()
	{
	}
	void LaserColl::Update()
	{
		// 指定された時間で最大サイズになる
		if (this->timeCounter += Sys::Timer::GetDeltaTime(); timeCounter >= MaxTime)
		{
			this->timeCounter = MaxTime;
		}
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