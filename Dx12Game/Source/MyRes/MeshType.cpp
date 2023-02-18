#include "MeshType.h"
#include <stdexcept>

namespace MyRes
{
	// 前置インクリメント
	MeshType& operator ++(MeshType& rhs)
	{
		if (rhs == MeshType::End)
		{
			throw std::out_of_range("for #include <stdexcept>outof range MeshType& operator ++ (MeshType&)");
		}
		rhs = MeshType(static_cast<std::underlying_type<MeshType>::type>(rhs) + 1);

		return rhs;
	}
}