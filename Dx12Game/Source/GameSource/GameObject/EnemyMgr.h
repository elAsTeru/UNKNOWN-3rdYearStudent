#pragma once
#include "GameObjectBase.h"

namespace GameObject
{
	struct EnemyData
	{
		int kind;				// 敵の種類
		DirectX::XMFLOAT3 pos;	// 出現位置
		DirectX::XMFLOAT3 rot;	// 出現向き
		float spawnTime;		// 出現時間
	};

	/// <summary> 敵の生成管理 </summary>
	class EnemyMgr : public Base
	{
	public:
		EnemyMgr();
		~EnemyMgr();

		void Init()override {}
		void Update(){}
		void Draw()const {};

		const int& GetClearPhaseNum() { return this->phaseCount; }
		/// <summary> 敵の出現を処理、フェーズが終了でtrueを返却 </summary>
		bool Update(const float& _ElapsedTime);
		bool Load(std::string _JsonFilePath);
		/// <summary> 次のフェーズに移行できたらtrueを返却 </summary>
		bool Next();

	private:
		// Private Variable	
		int spawnCount;												// スポーン回数
		int phaseCount;												// フェーズカウント
		std::unordered_map<int, std::vector<EnemyData>> enemyData;	// フェーズごとの敵のデータ
		// Private Method
	};
}