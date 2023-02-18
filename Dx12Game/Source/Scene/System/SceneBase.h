#pragma once
#include "SceneMgr.h"
#include "DX12Wrapper.h"
#include "Score.h"

namespace Scene
{
	class Base
	{
	protected:
		Base(std::string _Name):
			name(_Name)
		{}
	public:
		Base(const Base&) = default;
		Base& operator = (const Base&) = delete;
		virtual ~Base() = default;

		//Public Method
		virtual void Update() = 0;

	protected:
		std::string name;
	};
}