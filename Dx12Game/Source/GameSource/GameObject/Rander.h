#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CRestrictRect;
	class CSphColl;
}

namespace GameObject
{
	/// <summary> タイトルのみで使用、旧型式のオブジェクト </summary>
	class Rander : public Base
	{
	public:
		Rander();
		~Rander();

		// Public Override Method
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
		// 通常行動のみ
		const float MoveSpeed;		// 移動速度
		const float IdolDuration;	// 待ち時間
		const float ActionTime;		// 行動にかける時間
		bool isAction;				// ?行動中か
		// 死亡時のみ

		// Private Method
		/// <summary>
		/// 分割数にあった度数をランダムに作成して返却する
		/// </summary>
		/// <param name="_SplitNum">分割数(例:4なら360を4分割した、0,90,180,270が出る)</param>
		const float CreateRandomDir(int _SplitNum);
		void Move();				// 移動
		void Spawn();

		// Component Variable

		Component::CSphColl* sphColl;			// 球の当たり判定
		Component::CTransform* subTrans;		// 矢印用の座標系
		Component::CRestrictRect* cRestRect;

		// Componet Method
		void OnTriggerEnter(Base* _Other)override;
	};
}