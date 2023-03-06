#include "LineRes.h"

namespace Res
{
    const D3D12_INPUT_ELEMENT_DESC LineVertex::InputLineElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC LineVertex::InputLineLayout = { LineVertex::InputLineElements,LineVertex::InputLineElementCount };
}