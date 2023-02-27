#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"

namespace Component
{
	/// <summary>
	/// 設定したオブジェクトを設定間隔で生成
	/// </summary>
	class CGenerator : public Base
	{
	public:
		// Public Variable
		std::string ObjName;	// 生成するオブジェクトの名前
		float interval;			// 片道の所要時間

		// Public Method
		void Start()override 
		{
		};

		void Init()
		{
			// 未設定時用 テスト値
			this->ObjName = "Empty";
			this->interval = 1.0f;
		}

		void Update()
		{
			this->timeCounter += Sys::Timer::GetDeltaTime();
			// 時間カウンタが間隔時間を超えたら
			if (this->timeCounter >= this->interval)
			{
				this->timeCounter = 0;
				// 対象のオブジェクトを探索し、初期化後、親の座標をセットする
				if (GameObject::Base* obj = GameObject::Mgr::FindDeactiveObj(this->ObjName); obj)
				{
					obj->Init();
					obj->transform->position = this->parent->transform->position;
					obj->SetActive(true);
				}
			}
		}
	private:
		// Private Variable
		float timeCounter;	// 時間カウンタ
	};
}