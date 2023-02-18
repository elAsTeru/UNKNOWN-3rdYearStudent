#include "EfkType.h"
#include <stdexcept>

namespace MyRes
{
	// 前置インクリメント
	EfkType& operator ++(EfkType& rhs)
	{
		if (rhs == EfkType::End)
		{
			throw std::out_of_range("for outof range EfkType& operator ++ (EfkTtpe&)");
		}
		rhs = EfkType(static_cast<std::underlying_type<EfkType>::type>(rhs) + 1);

		return rhs;
	}
}