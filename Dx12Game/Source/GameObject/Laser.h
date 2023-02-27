#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	class Laser : public Base
	{
	public:
		Laser();
		~Laser();

		// Public Method
		void Init()override;
		void Update() override;		// 更新
		void Draw()const override;	// 描画


		/// <summary>
		/// レーザーの始点・終点を記録し、完了したらtrueを返却
		/// </summary>
		bool SetPos(Vector3 _Pos);
		void SetParentPos(Vector3* _Pos) { parentPos = _Pos; }
	private:
		// Component Variable
		Vector3* parentPos;
		Vector3 startPos;						// 始点
		Vector3 endPos;							// 終点
		bool isActivate;						// レーザーが有効化されたか?
		std::vector<GameObject::Base*> colls;	// レーザーで使用している当たり判定を格納

		const float Delay;						// レーザー発生遅延
		const float Duration;					// レーザー持続時間
		const float Width;						// レーザーの幅
		float timeCounter;						// レーザーの時間計測
		uint32_t laserEfkHandle;				// レーザーのエフェクトハンドル
		float addLaserScale;					// 線の拡大量
		XMMATRIX lineMatrix;					// 線用の行列

		// レーザー始点の回転
		const float RotSpeed;
		float rot;
	};
}