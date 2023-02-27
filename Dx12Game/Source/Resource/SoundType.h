#pragma once

namespace Res
{
	// Sound Effect Type
	enum class SEType : unsigned char
	{
		Begin,

		// 選択音
		Enter = Begin,
		Select,

		SelectIn,		// 選択画面に入るときの音
		ResultIn,		// リザルトが表示されるときの音

		// アクション
		Shot,			// 弾を撃った時の音
		Generate,		// プレイヤーが生成されるときの音
		Died,			// 死亡音
		Eliminate,		// 敵を倒したときの音
		Reflect,		// 弾をはじき返すときの音
		Damage,			// ダメージを当てた時の音
		Bomb,			// 爆破音

		End
	};

	// Back Ground Music Type
	enum class BGMType : unsigned char
	{
		Begin = 0,

		Title = Begin,
		Select,
		Game,
		Result,
		
		End
	};

	//// Ambient Sound Type
	//enum class ASType : unsigned char
	//{
	//	Begin = 0,



	//	End
	//};
}