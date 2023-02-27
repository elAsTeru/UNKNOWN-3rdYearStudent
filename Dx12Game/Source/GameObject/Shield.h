#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
	class CMoveAimPos;
	class CRotYAimPos;
};

namespace GameObject
{
	class Shield : public Base
	{
	public:
		Shield();
		~Shield();

		// Public Method
		void Init()override;
		void Update() override;			// 更新
		void Draw()const override;

		/// <summary>
		/// 移動の目標座標を設定
		/// </summary>
		void SetTargetPos(const Vector3 _Pos);
		/// <summary>
		/// 移動速度を設定
		/// </summary>
		void SetSpeed(const float _Speed);
		Vector3 GetForward() { return this->forward; }

		/// <summary>
		/// シールドがひび割れた表現状態にする
		/// </summary>
		void Crack();
		/// <summary>
		/// シールドが修復、通常に戻る
		/// </summary>
		void Repair();

		/// <summary>
		/// シールドの状態を取得
		/// </summary>
		const bool& IsCrack() { return this->isCrack; }

	private:
		// Component Variable
		float timeCounter;
		Vector3 forward;			// 向いている方向
		bool isCrack;				// シールドの状態
		bool isDraw;				// 描画を行うか?
		const float DurationAnim;	// アニメーションに使用する時間
		float scaleMag;				// 拡大率


		Component::SphColl* sphColl;			// 球のあたり判定コンポーネント用
		Component::CMoveAimPos* cMoveAimPos;
		Component::CRotYAimPos* cRotYAimPos;

		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}