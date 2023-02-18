#pragma once
#include "GameObjectBase.h"
#include "SphereCollider.h"

namespace GameObject
{
	struct EnemyData
	{
		int kind;				// 敵の種類
		DirectX::XMFLOAT3 pos;	// 出現位置
		DirectX::XMFLOAT3 rot;	// 出現向き
		float spawnTime;		// 出現時間
	};

	/// <summary>
	/// 敵の出現や倒した数などを管理する
	/// </summary>
	class EnemyMgr : public Base
	{
	public:
		EnemyMgr();
		~EnemyMgr();

		void Start() {}
		void Init()override {}
		void Update(){}
		const int& GetClearPhaseNum() { return this->phaseCount; }
		/// <summary>
		/// 敵の出現を行っていく、フェーズが終了したら、trueを返却する
		/// </summary>
		bool Update(const float& _ElapsedTime);
		bool Load(std::string _JsonFilePath);
		/// <summary>
		/// 次のフェーズに移れたらtrueを返却
		/// </summary>
		bool Next();

	private:
		// Private Variable	
		int spawnCount;												// スポーン回数
		int phaseCount;												// フェーズカウント
		std::unordered_map<int, std::vector<EnemyData>> enemyData;	// フェーズごとの敵のデータ
		// Private Method
	};
}