#pragma once
#include <list>
#include <string>
#include "GameObjectMgr.h"
#include "ComponentBase.h"
#include "Tag.h"
#include "Transform.h"
#include "Dx12Wrapper.h"
#include "Timer.h"
#include "SceneMgr.h"
#include "Score.h"
#include "Sound.h"

//// 可動性
//enum class Mobility
//{
//	Moveble,	// 移動〇更新〇
//	Stationary,	// 移動×更新〇
//	Static,		// 移動×更新×
//};

// class GameObjectBase
namespace GameObject
{
	class Base
	{
		Base() = delete;		//引数なしコンストラクタを削除
	public:
		Base(Tag _Tag, std::string _Name);
		virtual ~Base();
		// Public Basic Method
		virtual void Start(){}							// 開始(生成時に呼ばれる)
		virtual void Init() = 0;						// 初期化
		virtual void Update() = 0;						// 更新
		virtual void Draw()const = 0;					// 描画
		virtual void OnTriggerEnter(Base* _Other) {}	// 当たり判定を追加していたら実行される
		template<class T> T* GetComponent()				// コンポーネントを取得
		{
			for (auto com : componentList)
			{
				T* buff = dynamic_cast<T*>(com);
				if (buff != nullptr)
					return buff;
			}
			return nullptr;
		}
	protected:
		// Protected Basic Variable
		std::list<Component::Base*> componentList;			// 所有するコンポーネントのリスト
		// Protected Basec Method
		template<class T> T* AddComponent()							// コンポーネントを追加する
		{
			T* buff = new T();
			buff->parent = this;
			componentList.push_back(buff);
			buff->Start();
			return buff;
		}
	public:
		// Public Variable
		Component::Transform* transform;							// 座標系コンポーネント// Getter / Setter
		// Public Method
		const Tag& tag = gameObjTag;									// タグを取得
		const std::string& name = gameObjName;							// 名前取得
		void SetActive(const bool& _Activity) { isActive = _Activity; }	// 活動性のセット
		const bool GetActive() const { return this->isActive; }		// 活動性の取得
	private:
		// Private Variable
		Tag gameObjTag;				// タグ
		std::string gameObjName;	// 名前
		bool isActive;				// 活動性
	};
}