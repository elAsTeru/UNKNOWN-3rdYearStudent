#pragma once
#include "GameObjectBase.h"

namespace Component
{
	class CSphColl;
	class CRotYAimPos;
	class CMoveForwardY;
}

namespace Res
{
	enum class EfkType : unsigned char;
	enum class SEType : unsigned char;
	enum class ScoreType : unsigned char;
}

namespace GameObject
{
	/// <summary> 指定した＜位置＞に指定した＜速度＞で向かうミサイル </summary>
	class EnemyMissile : public Base
	{
	public:
		EnemyMissile();
		~EnemyMissile();

		void Start() {}
		void Init()override;
		void Update()override;
		void Draw()const override;


	private:
		struct OnTriDesc
		{
			std::string otherName;							// 衝突相手のオブジェクト名
			Res::EfkType efkType;							// 衝突時に出すエフェクトのタイプ
			Res::SEType seType;								// 衝突時に鳴らすSEのタイプ
			Res::ScoreType scoreType = Res::ScoreType::End;	// 衝突時に加算するスコアタイプ、加算されない場合はEndを設定
		};

		// Private Variable
		
		const float RotSpeed;			// ミサイルの回転速度
		const float InvalidCollDuration;	// 初期化後、有効になってから当たり判定無効時間

		Vector3& targetPos;
		float timeCounter;
		float rot;
		XMMATRIX subMatrix;					// ミサイルの羽部分の行列
		std::vector<OnTriDesc> onTriDescs;	// 当たり判定時に判定と処理を行うリスト

		// Component Variable

		Component::CSphColl* sphColl;
		Component::CRotYAimPos* cRotYAimPos;
		Component::CMoveForwardY* cMoveForwardY;
		
		// Component Method
		
		void OnTriggerEnter(Base* _Other) override;
	};
}