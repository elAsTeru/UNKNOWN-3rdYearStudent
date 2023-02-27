#pragma once
#include "GameObjectBase.h"
#include "SphereCollider.h"

namespace Component
{
	class CFixPos;
}

/// <summary>
/// 旧型式、タイトルのみで使用
/// </summary>
namespace GameObject
{
	class Reverser : public Base
	{
	public:
		Reverser();
		~Reverser();

		void Init()override;
		void Update()override;
		void Draw()const override;
		void Eliminate();
	private:
		enum class Mode : unsigned char
		{
			Spawn,
			Normal,
			Dead
		};
		Mode mode;
		// Private Variable
		// 共有
		float timeCounter;			// 時間計測計
		// 生成のみ
		const float MaxSpawnScale;	// 生成用の最大拡大率
		const float SpawnDuration;	// 生成継続時間
		const float ExpanRunNum;	// 拡大実行回数
		float spawnScale;			// 生成用の拡大率

		float speed;

		bool isReverse;		// 反転している

		// Private Method
		void Move();
		void Reverse();
		void Spawn();


		/// <summary>
		/// 向きと設定から、移動した場合の座標を取得する
		/// </summary>
		/// <param name="_Rotation"></param>
		/// <param name="_Speed"></param>
		/// <returns></returns>
		Vector3 MoveForward(const Vector3 _Rotation, const float _Speed);		// 向いている方向に移動する処理 // 未実装

		// Component Variable

		Component::SphColl* sphColl;
		Component::Transform* subTrans;		// ジェット部分用の座標系
		Component::CFixPos* cFixPos;

		// Component Method

		void OnTriggerEnter(Base* _Other) override;
	};
}