#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"

namespace Component
{
	/// <summary>
	/// p1、p2間を所要時間をかけて移動する
	/// </summary>
	class CLineMove : public Base
	{
	public:
		// 移動方法
		enum class MoveType
		{
			OneWay,	// 片道処理
			Round,	// 往復処理
			Random	// ランダム
		};
		// Public Variable

		float duration;		// 片道の所要時間
		MoveType type;		// 処理タイプ
		uint16_t maxDiv;	// ランダム時分割の最大値
		uint16_t minDiv;	// ランダム時分割の最小値

		// Public Method

		/// <summary> 線、二点の情報を設定する 線移動を行うために、二点の情報を設定する </summary>
		void SetLine(Vector3 _P1, Vector3 _P2)
		{
			this->p1 = _P1;
			this->p2 = _P2;
		}

		void Start()override 
		{
		};

		void Init()
		{
			// 未設定時用 テスト値
			this->p1 = Vector3(-5, 0, 0);
			this->p2 = Vector3(5, 0, 0);
			this->duration = 1.0f;
			this->type = MoveType::Round;
			this->timeCounter = 0;
			this->maxDiv = 0;
			this->minDiv = 0;
		}

		void Update()
		{
			// 片道、往復処理の場合
			if (this->type == MoveType::OneWay || this->type == MoveType::Round)
			{
				// 移動処理、移動が完了した場合
				if (this->MoveLineLerp())
				{
					// 移動タイプが往復の場合、開始位置と目標位置を反転
					if (this->type == MoveType::Round)
					{
						Vector3 temp = this->p1;
						this->p1 = this->p2;
						this->p2 = temp;
					}
					this->timeCounter = 0.0f;
				}
			}
			// ランダムの場合
			else if (this->type == MoveType::Random)
			{
				// p1~p2間の分割量を渡し、乱数を生成
				uint32_t rand = MyMath::CreateRandom(this->minDiv, this->maxDiv);
				this->parent->transform->position = Vector3::Lerp(this->p1, this->p2, rand / (float)(maxDiv - minDiv));
			}
		}
	private:
		// Private Variable
		Vector3 p1;			// 開始位置
		Vector3 p2;			// 目標位置
		float timeCounter;	// 時間計測カウンタ

		// Private Method

		/// <summary>
		/// 線形補間移動を行い、移動がかんりょうしたらtrueを返却
		/// </summary>
		/// <returns></returns>
		bool MoveLineLerp()
		{
			timeCounter += Sys::Timer::GetDeltaTime();
			// 線形補間移動を行う
			this->parent->transform->position = Vector3::Lerp(this->p1, this->p2, this->timeCounter / this->duration);
			// 2点間の移動にかかる時間を超えた場合
			if (this->timeCounter / this->duration >= 1.0f)
			{
				// 現在位置を目標座標にする
				this->parent->transform->position = this->p2;
				return true;
			}
			return false;
		}
	};
}