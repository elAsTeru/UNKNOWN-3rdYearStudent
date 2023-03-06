#include "Laser.h"
#include "Dx12Wrapper.h"
#include "Tool/EfkMgr.h"
#include "CSphColl.h"

#include <cmath>

namespace GameObject
{
	Laser::Laser() :
		Base(Tag::Enemy, "Laser"),
		parentPos(nullptr),
		startPos(),
		endPos(),
		isActivate(false),
		colls(),
		Delay(2),
		Duration(10),
		Width(3),
		timeCounter(0),
		RotSpeed(5.0f)
	{}

	Laser::~Laser()
	{}

	void Laser::Init()
	{
		this->parentPos = nullptr;
		this->startPos = {};
		this->endPos = {};
		this->isActivate = false;
		this->timeCounter = 0;
		this->rot = 0.0f;
		this->addLaserScale = 0.0f;
	}

	void Laser::Update()
	{
		// 始点が決まっていたら
		if (!MyMath::IsNearZero(this->startPos))
		{
			// endPos = ep ではない、endPosはレーザーの終点、epは現在epのある位置

			Vector3 ep = {};					// 線の終点
			if (this->parentPos)				// 親が設定されていた場合
			{ ep = *this->parentPos; }
			if (this->endPos != Vector3::Zero)	// 終点が決定している場合
			{ ep = this->endPos; }
			// 2点間の中点
			Vector3 midPos = (this->startPos + ep) / 2;
			// 2点間の角度
			float dirY = MyMath::Vec2ToAnglesLH(Vector2(ep.x - this->startPos.x, ep.z - this->startPos.z));
			// 2点間の距離
			Vector3 dist3D = { std::fabs(this->startPos.x - ep.x),0,std::fabs(this->startPos.z - ep.z) };
			// 2点間の距離を大きさに
			float vecSize = dist3D.Length();

			// 親オブジェクトがある || 終点が決定している場合
			if (this->parentPos || !MyMath::IsNearZero(this->endPos))
			{
				// レーザーの大きさを伝えるための板のサイズ
				this->addLaserScale = 0.0f;
				if (auto temp = GameObject::Mgr::FindActiveObj("LaserColl"))
				{
					this->addLaserScale = temp->GetComponent<Component::CSphColl>()->radius;
				}

				// レーザーの範囲に仮置きで板を描画
				float laserWid = 0.1f;
				this->lineMatrix
					= Matrix::CreateScale(vecSize / 2.0f, 1.0f, laserWid + this->addLaserScale)
					* Matrix::CreateRotationY(dirY)
					* Matrix::CreateTranslation(midPos);
			}

			// 終点が決定したのであれば
			if (this->endPos != Vector3::Zero)
			{
				// 当たり判定は一度しか設置しない
				if (timeCounter += Sys::Timer::GetDeltaTime();!this->isActivate && (timeCounter >= Delay))
				{
					// レーザー有効化
					this->isActivate = true;
					// タイマーリセット
					timeCounter = 0;
					// 2点間の単位ベクトルを求める
					Vector3 unit3D = ep - this->startPos;
					unit3D.Normalize();
					
					// 当たり判定を単位ベクトルを使用して配置していく
					for (int i = 0;;++i)
					{
						// もし単位ベクトルのスカラー倍が始点終点間ベクトルの大きさを超える場合
						if(Vector3 length = unit3D * i * Width; length.Length() >= vecSize)
						{
							auto obj = GameObject::Mgr::FindDeactiveObj("LaserColl");
							// 強制的に最後の当たり判定は終点に置かれる
							obj->Init();
							obj->transform->position = this->endPos;
							obj->SetActive(true);
							this->colls.push_back(obj);
							break;
						}

						auto obj = GameObject::Mgr::FindDeactiveObj("LaserColl");
						obj->Init();
						obj->transform->position = this->startPos + (unit3D * i * Width);
						obj->SetActive(true);
						this->colls.push_back(obj);
					}
				}
				else
				{
					// 一定時間経過したらレーザを無効化する
					this->timeCounter += Sys::Timer::GetDeltaTime();
					if (this->timeCounter >= this->Duration)
					{
						for (auto& coll : colls)
						{
							coll->SetActive(false);
						}
						colls.clear();
						// 当たり判定を無効化できたら、自身を無効化する
						this->SetActive(false);
						// エフェクトを停止
						Effect::EfkMgr::StopEffect(this->laserEfkHandle);
					}
				}
			}
			// レーザーのエフェクトをループさせる
			Effect::EfkMgr::UpdateLoopPlayEffect(this->startPos, this->laserEfkHandle, Sys::Timer::GetHitStopTime() * 100, Vector3(0.0f, dirY, 0.0f));
		}

		// 回転
		this->rot += Sys::Timer::GetHitStopTime() * this->RotSpeed;
	}

	void Laser::Draw() const
	{
		// 始点描画
		XMMATRIX startPointMatrix
			= Matrix::CreateScale(4.0f)
			* Matrix::CreateRotationX(this->rot)
			* Matrix::CreateRotationZ(this->rot)
			* Matrix::CreateTranslation(this->startPos);
		MyDX::Dx12Wrapper::DrawMesh(startPointMatrix, Res::MeshType::GeoBox, Res::MaterialType::Red);

		startPointMatrix
			= Matrix::CreateScale(3.0f)
			* Matrix::CreateRotationX(-this->rot)
			* Matrix::CreateRotationZ(-this->rot)
			* Matrix::CreateTranslation(this->startPos);
		MyDX::Dx12Wrapper::DrawMesh(startPointMatrix, Res::MeshType::GeoTriangle, Res::MaterialType::Red);
		
		// 親オブジェクトがある || 終点が決定している場合
		if (this->parentPos || !MyMath::IsNearZero(this->endPos))
		{
			if (MyMath::IsNearZero(this->addLaserScale))
			{
				MyDX::Dx12Wrapper::DrawMesh(this->lineMatrix, Res::MeshType::Board, Res::MaterialType::White);
			}
			else
			{
				MyDX::Dx12Wrapper::DrawMesh(this->lineMatrix, Res::MeshType::Board, Res::MaterialType::Red);
			}
		}
	}

	bool Laser::SetPos(Vector3 _Pos)
	{
		// 始点が空なら位置を記録して、エフェクトを描画
		if (MyMath::IsNearZero(this->startPos))
		{
			this->startPos = _Pos;			
			return false;
		}
		// 終点が空なら位置を記録して始点終点が揃ったのを伝えるためにtrueを返却
		else if(MyMath::IsNearZero(this->endPos))
		{
			this->endPos = _Pos;

			// 2点間の角度
			float dirY = MyMath::Vec2ToAnglesLH(Vector2(this->endPos.x - this->startPos.x, this->endPos.z - this->startPos.z));
			// エフェクト
			this->laserEfkHandle = Effect::EfkMgr::PlayEffect(Res::EfkType::Laser, this->startPos, true, Vector3(0.0f, dirY, 0.0f));

			return true;
		}
		return true;
	}
}