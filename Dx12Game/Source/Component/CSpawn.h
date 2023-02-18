#pragma once
#include "ComponentBase.h"
#include "GameObjectBase.h"
#include "InputMgr.h"

namespace Component
{
	class CSpawn : public Base
	{
	public:
		// Public Variable
		MyRes::MeshType meshType;	// メッシュの種類
		int textureID;				// テクスチャID
		float delay;				// 発生遅延
		float duration;				// 持続時間
		float minScale;				// 最小拡大率
		float maxScale;				// 最大拡大率

		// Public Method
		void Start()override 
		{
		};

		void Run(const int _Remaining)
		{
			this->remaining = _Remaining;
			this->elapsed = 0;
		}

		/// <summary>
		/// 発生用エフェクトに近いもの 通常は現在の倍率0~1を返却 終了したら-1を返却
		/// </summary>
		float Update()
		{
			// 残り実行回数があれば 処理を行う
			if (this->remaining > 0)
			{
				// 発生遅延時間を越えてなければ処理を終了
				if(this->elapsed += MySys::Timer::GetDeltaTime(); this->elapsed < this->delay)
				{ return 0; }

				// 現在の拡大率を求める
				float magnification = this->elapsed / (this->delay + this->duration);
				// 倍率が最大値を越えていたら補正
				if (magnification >= 1.000f)
				{
					magnification = 1;
				}
				// 拡大率を求める
				float scale = std::lerp(this->minScale, this->maxScale, magnification);

				//if (this->meshType != MyRes::MeshType::End)
				//{
				//	// 描画
				//	XMMATRIX matrix
				//		= Matrix::CreateScale(scale)
				//		* Matrix::CreateRotationY(parent->transform->rotation.y)
				//		* Matrix::CreateTranslation(parent->transform->position);
				//	MyDX::Dx12Wrapper::DrawBasicMesh({ matrix,this->meshType,this->textureID });
				//}

				// 1回の実行時間を超えていたら
				if (this->elapsed >= (this->delay + this->duration))
				{
					this->elapsed = 0;
					if (--this->remaining; this->remaining <= 0)
					{
						return -1; //処理終了
					}
				}
				return magnification;	// まだ続く
			}
			return -1;			// 処理は終了済み
		}

	private:
		// Private Variable
		int remaining;				// 残り実行回数
		float elapsed;			// 経過時間
	};
}