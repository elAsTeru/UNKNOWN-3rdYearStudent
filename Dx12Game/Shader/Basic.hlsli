
struct VSOutput
{
    float4 pos      : SV_POSITION;  // �ʒu���W�ł�.
    float4 normal   : NORMAL;
    float2 uv       : TEXCOORD;     // �e�N�X�`�����W�ł�.
    float1 alpha    : COLOR;
};