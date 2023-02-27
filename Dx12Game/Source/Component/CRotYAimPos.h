#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include <cmath>

namespace Component
{
	/// <summary>
	/// 現在の向きを指定された座標(target)を向くようにrotSpeedの速度でY回転する
	/// </summary>
	class CRotYAimPos : public Base
	{
	public:
		// Private Variable
		Vector3 aimPos;		// 目標座標
		float rotSpeed;		// 回転速度
		bool isRunBeforeFix;// 前、補正実行(滑らかに追尾する敵などはフラグをoffにするとよい)

		// Public Method
		void Start()override 
		{
			this->rotSpeed = 0.0f;
			isRunBeforeFix = true;
		};

		void Init()
		{
			this->aimPos = Vector3(0.0f);
			this->rotSpeed = 0.0f;
		}

		void Update()
		{
			// 現在の向きからベクトルを求める
			Vector3 nowDir;
			{
				Vector2 temp = MyMath::AngleToVecs2LH(this->parent->transform->rotation.y);
				nowDir = Vector3(temp.x, 0, temp.y);
			}

			// 目標の向きベクトル
			Vector3 aimDir = aimPos - this->parent->transform->position;
			aimDir.Normalize();

			if (this->isRunBeforeFix)
			{
				// aimDirと現在の向きがほぼ同じなら適応して終了する
				if (MyMath::IsNearAngle(nowDir, aimDir))
				{
					nowDir = aimDir;
					this->parent->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(nowDir.x, nowDir.z));
					return;
				}
			}

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

			this->parent->transform->rotation.y = MyMath::Vec2ToAnglesLH(Vector2(rotVec.x, rotVec.z));
		}
	};
}