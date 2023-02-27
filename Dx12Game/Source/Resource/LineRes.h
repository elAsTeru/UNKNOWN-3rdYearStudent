#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXTK12/SimpleMath.h>

namespace Res
{
	// 一頂点の情報
	class LineVertex
	{
	public:
		DirectX::XMFLOAT3 position;

		LineVertex() = default;
		LineVertex(
			DirectX::XMFLOAT3 const& _Position):
			position(_Position)
		{ /*DO_NOTHING*/ }

		static const D3D12_INPUT_LAYOUT_DESC InputLineLayout;

	private:
		// Private Variable
		static const int InputLineElementCount = 1;
		static const D3D12_INPUT_ELEMENT_DESC InputLineElements[InputLineElementCount];
	};
}