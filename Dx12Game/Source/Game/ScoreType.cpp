#include "ScoreType.h"

#include <stdexcept>

namespace MyRes
{
	// 前置インクリメント
	ScoreType& operator ++(ScoreType& rhs)
	{
		if (rhs == ScoreType::End)
		{
			throw std::out_of_range("for outof range ScoreType& operator ++ (ScoreType&)");
		}
		rhs = ScoreType(static_cast<std::underlying_type<ScoreType>::type>(rhs) + 1);

		return rhs;
	}
}