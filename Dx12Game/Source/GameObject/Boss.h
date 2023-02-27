#pragma once
// ------------------------------
// ステージ1~3で使用しているボス
// ------------------------------
#include "GameObjectBase.h"

namespace Component
{
	class SphColl;
	class CFixPos;
	class CMoveForwardY;
	class CRotYAimPos;
	class CMoveAimPos;
	class CTransformRingBuff;
}

namespace State::Boss
{
	class Base;
	enum class StateList;
}

namespace Res
{
	enum class MaterialType : unsigned char;
}

namespace GameObject
{
	class Laser;
	class Shield;
	class EnemyMissile;

	class Boss : public Base
	{
	public:
		Boss();
		~Boss();

		// Public Method
		void Init()override;
		void Update() override;			// 更新
		void Draw()const override;


		// ステートから呼ぶ処理一覧
	
		void FireNormal(const uint8_t _BulletNum);		// 通常弾幕
		void FireMissile();								// ミサイル発射
		void ClearRotShields();							// 回転シールドを無効化する
		void ClearTrackShields();						// 追跡シールドを無効化する
		void TrackingShields();							// シールドの追跡処理
		void ReflectionMoveDir();						// 反射追跡
		void RotPlayerPos();							// 追跡移動
		bool PutLaserPoint();							// レーザーを設置する処理、レーザーが完成したらtrueを返却
		void ClearLaser();								// 行動変更時などにレーザーを無効化する
		void NumOrLessTrackingShield(uint8_t _Num);		// シールドが_Num以下になるようにする

		/// <summary>
		/// 目標座標へ移動する。完了後trueを返却
		/// </summary>
		bool Move(const Vector3 _Target, const float _Speed);
		/// <summary>
		/// ボスを中心に回転するシールドを指定数作成
		/// </summary>
		void CreateRotShield(const uint8_t _Num, const float _Speed);
		/// <summary>
		/// 回転シールドを回転させる処理
		/// </summary>
		void RotShields(const float& _Speed);
		/// <summary>
		/// ボスを追跡するシールドを指定数作成
		/// </summary>
		void CreateTrackShield(const uint8_t _Num, const float _Speed);
		/// <summary>
		/// 向いている方向に進む
		/// </summary>
		void MoveForward();
		/// <summary>
		/// ステージの外に出ているか？
		/// </summary>
		bool IsOutsideStage();

		void ClearWaitTime() { this->waitTime = 0.0f; }

		const uint16_t& GetHpPercentage();						// 現在の体力の割合を取得(0~100)
		void SetSpeed(const float _Speed);						// 移動速度を設定
		void SetInvisible(const bool& _IsInvisible) { this->isInvisible = _IsInvisible; }	// 無敵状態を設定
		void SetLevel(const uint8_t _Lv) { this->level = _Lv; }	// レベル設定
		uint8_t GetLevel()const { return this->level; }
	private:
		//// Private Variable
		
		// 自身の状態
		bool isInvisible;
		uint8_t level;
		
		// ステータス
		float speed;						// 移動速度
		const uint16_t FirstHp;				// 初期体力
		uint16_t hp;						// 体力

		// 回転シールド
		const float ScaleRotShield;
		std::vector<GameObject::Shield*> rotShields;		// 使用中の回転シールド
		float rotShieldRot;									// 回転シールドの角度
		
		// 追跡シールド
		const float DistMagnification;						// 追跡間隔の倍率
		const float ScaleTrackingShield;
		const float DurationShieldCrack;					// シールドにひびが入った状態の継続時間
		std::vector<GameObject::Shield*> trackingShields;	// 使用中の追跡シールド
		float crackTimeCounter;

		// 攻撃
		GameObject::Base* aimPos = nullptr;			// 目標
		GameObject::Laser* laser;					// 現在作成中のレーザーオブジェクト
		// 被弾
		float waitTime;								// 停止時間カウンター

		// 色関連
		const float DurationDamageColor;		// ダメージカラーの持続時間
		float damageTimeCounter;
		Res::MaterialType matType;


		// ステート
		State::Boss::StateList nowState;				// 現在のステート
		State::Boss::StateList nextState;				// 次のステート
		std::vector<State::Boss::Base*> statePools;		// ステートクラスプール
		
		float rotCore;									// コア部分回転用
		float rotSpeed;									// 反射時などの正面を向くための回転速度

		// Private Method
		void Damage();

		// Component Variable
		Component::SphColl* sphColl;				// 球のあたり判定コンポーネント用
		Component::CFixPos* cFixPos;
		Component::CMoveForwardY* cMoveForwardY;
		Component::CRotYAimPos* cRotYAimPos;
		Component::CMoveAimPos* cMoveAimPos;
		Component::CTransformRingBuff* cRingBuff;
		// Component Method
		void OnTriggerEnter(Base* _Other)override;
	};
}