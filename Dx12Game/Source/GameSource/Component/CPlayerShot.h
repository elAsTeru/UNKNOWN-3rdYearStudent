#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include "Tool/InputMgr.h"
#include "Tool/Sound.h"

namespace Component
{
	/// <summary>
	/// プレイヤーのショット処理
	/// </summary>
	class PlayerShot : public Base
	{
	public:
		// Public Variable
		float fireRate;				// 連射速度
		float coolTime;				// 次撃てるまでの時間
		float timeCounter;
		int outerDir;
		float nowDirY;				// 現在弾を飛ばしている方向
		float rotSpeed;
		bool isFreeDir;				// 傾けた方向に直接弾を撃てる状態か？

		// 用語記録用
		// damage			// ダメージ
		// fireRate			// 連射速度
		// range			// 最大射程
		// heatCapacity		// オーバーヒートする熱量
		// heatGeneration	// 1発でどれくらいの熱量が発生するか
		// coolingSpeed		// 射撃してないとき毎秒取り除かれる熱量
		// overheatCoolDown	// オーバーヒート時再射撃可能になるまでの時間


		// Public Method
		void Start()override
		{
			this->fireRate = 12.0f;
			this->coolTime = 1.0f / fireRate;
			this->outerDir = -1;
			this->rotSpeed = 4000.0f;
			this->isFreeDir = true;
		};

		// Public Method
		/// <summary>
		/// 移動処理、移動処理が入った場合trueを返却
		/// </summary>
		void Update()
		{
			// クールタイムチェック
			if (timeCounter += Sys::Timer::GetHitStopTime(); timeCounter < coolTime)
			{ return; }

			timeCounter = 0;

			// 射撃に使用するAxisの取得
			Vector2 axis = Input::Mgr::GetAxisR();
			// 入力が無ければ処理を終了
			if (MyMath::IsNearZero(axis.x) && MyMath::IsNearZero(axis.y))
			{
				this->isFreeDir = true;
				return;
			}

			// 入力量と現在の向きから、線形補間で弾が飛ぶ角度を求める
			// 現在の向きからベクトルを求める
			Vector3 nowDir;
			{
				Vector2 temp = MyMath::AngleToVecs2LH(this->nowDirY);
				nowDir = Vector3(temp.x, 0, temp.y);
			}

			// 目標の向きベクトル
			Vector3 aimDir = Vector3(axis.x, 0, axis.y);

			// aimDirと現在の向きがほぼ同じなら適応して終了する
			if (this->isFreeDir || MyMath::IsNearAngle(nowDir, aimDir))
			{
				this->isFreeDir = false;
				nowDir = aimDir;
				this->nowDirY = MyMath::Vec2ToAnglesLH(Vector2(nowDir.x, nowDir.z));
			}
			else
			{
				// 左右回転どちらが近いか求める
				float rotDir = MyMath::CalcRotateYDirection(nowDir, aimDir);

				// aimDirに徐々に近づける処理
				Vector3 rotVec;
				{
					// 回転速度を求める
					float radSpeed = MyMath::ToRadians(rotSpeed) * Sys::Timer::GetHitStopTime();
					radSpeed *= rotDir;
					// 回転行列を作成
					Matrix yRotMat = Matrix::CreateRotationY(radSpeed);
					// 回転処理
					rotVec = Vector3::Transform(nowDir, yRotMat);
				}

				// 回転しすぎていたら修正
				Vector3 cross1, cross2;
				nowDir.Cross(aimDir, cross1);
				rotVec.Cross(aimDir, cross2);

				if (cross1.y * cross2.y < 0.0f)
				{
					rotVec = aimDir;
				}
				this->nowDirY = MyMath::Vec2ToAnglesLH(Vector2(rotVec.x, rotVec.z));
			}
			
			// 弾を左右交互に設置する処理
			Vector3 cross;		// 外積で出た値を格納
			// 外積で弾を横に並べられるようにする
			{
				// 弾の向きベクトル
				Vector3 dir(nowDir.x, 0, nowDir.z);
				// 上向きのベクトル
				Vector3 upward(0, 1, 0);
				// 外積を求める
				cross = dir.Cross(upward);
				cross.Normalize();
			}
			// 左右交互にアクティブ化
			{
				float distAround = 0.2f;	// 左右間の距離
				float dist = 0.5f;			// 中心からの距離
				Vector3 setPos = (cross * float(outerDir) * distAround) + (nowDir * dist) + parent->transform->position;	// 弾を設置する位置
				// 無効な弾を探索
				if (auto bullet = GameObject::Mgr::FindDeactiveObj("Bullet"); bullet)
				{
					bullet->Init();
					// プレイヤーの位置と弾の単位ベクトルを足して、プレイヤーの位置＋指定した距離離れた場所に弾を設置
					bullet->transform->position = setPos;
					bullet->transform->rotation.y = this->nowDirY;
					bullet->SetActive(true);
					// 射撃SEを再生
					MyDX::Tool::Sound::PlaySE(Res::SEType::Shot);
				}

				// 弾の左右出る方を交代
				outerDir *= -1;
			}
		}
	};
}