#include "Pera.hlsli"

// ���m�N��(�O���[�X�P�[��)
float GrayScale(float4 _Col);
// �F�̔��]
float4 ReverseColor(float4 _Col);
// �F�̊K���𗎂�
float4 DownTone(float4 _Col);
// ��f�̕��ω��ɂ��ڂ���
float4 BlurPixelAverage(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// �G���{�X����
float4 Emboss(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// �V���[�v�l�X(�G�b�W�̋���)
float4 Sharpness(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
//�֊s�����o
float TakeOutline(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// �K�E�V�A���ڂ���(�ȈՔ�)
float4 Get5x5GaussianBlur(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy, float4 _Rect);


float4 Pera(Output input) : SV_Target
{
    float4 col = tex.Sample(smp, input.uv); // ���H�Ȃ�
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels);     // levels...�~�b�v�}�b�v�̃��x��
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);        // �Ԃ�l

    //if(input.uv.x < 0.2f && input.uv.y < 0.2f)
    //{
    //    return tex.Sample(smp, (input.uv - float2(0, 0.2)) * 5);
    //}
    //else if (input.uv.x < 0.2f && input.uv.y < 0.4f)
    //{
    //    // uv
    //    //return texNormal.Sample(smp, input.uv * 5);
    //    // outline
    //    return TakeOutline(texNormal, smp, input.uv * 5, dx, dy);
    //    // uv * outline
    //    return texNormal.Sample(smp, input.uv * 5) * TakeOutline(texNormal, smp, input.uv * 5, dx, dy);
    //}
    
    float4 bloomAccum = float4(0, 0, 0, 0);
    float2 uvSize = float2(1, 0.5f);
    float2 uvOfst = float2(0, 0);
    
    for (int i = 0; i < 8; ++i)
    {
        bloomAccum += Get5x5GaussianBlur(texShrinkHighLum, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
        uvOfst.y += uvSize.y;
        uvSize *= 0.5f;
    }
    
    return tex.Sample(smp, input.uv)
    + Get5x5GaussianBlur(texHighLum, smp, input.uv, dx, dy, float4(0,0,1,1))
    + saturate(bloomAccum);
}


BlurOutput BlurPS(Output input) : SV_TARGET
{
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels); // levels...�~�b�v�}�b�v�̃��x��
    float dx = 1.0 / w;
    float dy = 1.0 / h;

    BlurOutput ret;
    ret.col = tex.Sample(smp, input.uv);
    ret.highLum = Get5x5GaussianBlur(texHighLum, smp, input.uv, dx, dy, float4(0, 0, 1, 1));
    
    return ret;
}


float4 HorizontalBokehPS(Output input) : SV_TARGET
{
    float4 col = tex.Sample(smp, input.uv); // ���H�Ȃ�
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels); // levels...�~�b�v�}�b�v�̃��x��
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0); // �Ԃ�l


    ret += bkWeights[0] * col;
    for (int i = 1; i < 8; ++i)
    {
        ret += bkWeights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(dx * i, 0));
        ret += bkWeights[i >> 2][i % 4] * tex.Sample(smp, input.uv - float2(dx * i, 0));
    }
    return float4(ret.rgb, col.a);
}


float4 VerticalBokehPS(Output input) : SV_TARGET
{
    float4 col = tex.Sample(smp, input.uv); // ���H�Ȃ�
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels); // levels...�~�b�v�}�b�v�̃��x��
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0); // �Ԃ�l


    ret += bkWeights[0] * col;
    for (int i = 1; i < 8; ++i)
    {
        ret += bkWeights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * i));
        ret += bkWeights[i >> 2][i % 4] * tex.Sample(smp, input.uv - float2(0, dy * i));
    }
    return float4(ret.rgb, col.a);
}


float GrayScale(float4 _Col)
{
    return dot(_Col.rgb, float3(0.299, 0.587, 0.114));
}

float4 ReverseColor(float4 _Col)
{
    return float4(1.0f - _Col.rgb, _Col.a);
}

float4 DownTone(float4 _Col)
{
    return float4(_Col.rgb - fmod(_Col.rgb, 0.25f), _Col.a);
}


float4 BlurPixelAverage(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // ��f�̕��ω��ɂ��ڂ���
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy));    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx, -2 * _Dy));    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy));    // �E��
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0 * _Dy));    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx,  0 * _Dy));    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0 * _Dy));    // �E
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy));    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx,  2 * _Dy));    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy));    // �E��
    return ret / 9.0f;
}

float4 Emboss(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // �G���{�X����
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy)) *  2;    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 0      , -2 * _Dy));         // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy)) *  0;    // �E��
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     ));          // ��
    ret += _Tex.Sample(_Smp, _Uv);                                      // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;     // �E
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy)) *  0;    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2(0       ,  2 * _Dy)) + -1;    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy)) * -2;    // �E��
    return ret;
}

float4 Sharpness(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // �V���[�v�l�X(�G�b�W�̋���)
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy)) *  0;    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 0      , -2 * _Dy)) * -1;    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy)) *  0;    // �E��
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     )) * -1;     // ��
    ret += _Tex.Sample(_Smp, _Uv)                             *  5;      // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;     // �E
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy)) *  0;    // ����
    ret += _Tex.Sample(_Smp, _Uv + float2(0       ,  2 * _Dy)) + -1;    // ��
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy)) *  0;    // �E��
    return ret;
}

float TakeOutline(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // �֊s�����o
    ret += _Tex.Sample(_Smp, _Uv + float2( 0     , -2 * _Dy)) * -1;   // ��
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     )) * -1;   // ��
    ret += _Tex.Sample(_Smp, _Uv)                             *  4;   // ����
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;   // �E
    ret += _Tex.Sample(_Smp, _Uv + float2(0      ,  2 * _Dy)) + -1;   // ��
    //// ���]
    //float Y = GrayScale(ret);
    //Y = pow(1.0f - Y, 10.0f);
    //Y = step(0.2f, Y);
    
    //return float4(Y, Y, Y, _Tex.Sample(_Smp, _Uv).a);
    
    return float4(ret.rgb, 1.0f);
}

float4 Get5x5GaussianBlur(Texture2D<float4>_Tex, SamplerState _Smp ,float2 _Uv, float _Dx, float _Dy,float4 _Rect)
{
    float4 ret = _Tex.Sample(_Smp, _Uv);
    float l1 = -_Dx, l2 = -2 * _Dx;
    float r1 =  _Dx, r2 =  2 * _Dx;
    float u1 = -_Dy, u2 = -2 * _Dy;
    float d1 =  _Dy, d2 =  2 * _Dy;
    l1 = max(_Uv.x + l1, _Rect.x) - _Uv.x;
    l2 = max(_Uv.x + l2, _Rect.x) - _Uv.x;
    r1 = min(_Uv.x + r1, _Rect.z  - _Dx) - _Uv.x;
    r2 = min(_Uv.x + r2, _Rect.z  - _Dx) - _Uv.x;

    u1 = max(_Uv.y + u1, _Rect.y) - _Uv.y;
    u2 = max(_Uv.y + u2, _Rect.y) - _Uv.y;
    d1 = min(_Uv.y + d1, _Rect.w  - _Dy) - _Uv.y;
    d2 = min(_Uv.y + d2, _Rect.w  - _Dy) - _Uv.y;
    
    return float4((
    // �ŏ�i
      _Tex.Sample(_Smp, _Uv + float2(l2 , u2)).rgb
    + _Tex.Sample(_Smp, _Uv + float2(l1 , u2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2( 0 , u2)).rgb *  6
    + _Tex.Sample(_Smp, _Uv + float2(r1 , u2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(r2 , u2)).rgb
    // ��i
    + _Tex.Sample(_Smp, _Uv + float2(l2 , u1)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(l1 , u1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2( 0 , u1)).rgb * 24
    + _Tex.Sample(_Smp, _Uv + float2(r1 , u1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2(r2 , u1)).rgb * 4
    // ���i
    + _Tex.Sample(_Smp, _Uv + float2(l2 , 0)) .rgb*  6
    + _Tex.Sample(_Smp, _Uv + float2(l1 , 0)) .rgb* 24
    + ret.rgb * 36
    + _Tex.Sample(_Smp, _Uv + float2(r1 , 0)) .rgb* 24
    + _Tex.Sample(_Smp, _Uv + float2(r2 , 0)).rgb * 6
    // ���i
    + _Tex.Sample(_Smp, _Uv + float2(l2 , d1)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(l1 , d1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2( 0 , d1)).rgb * 24
    + _Tex.Sample(_Smp, _Uv + float2(r1 , d1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2(r2 , d1)).rgb * 4
    // �ŉ��i
    + _Tex.Sample(_Smp, _Uv + float2(l2 , d2)).rgb
    + _Tex.Sample(_Smp, _Uv + float2(l1 , d2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2( 0 , d2)).rgb *  6
    + _Tex.Sample(_Smp, _Uv + float2(r1 , d2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(r2 , d2)).rgb
    ) / 256, ret.a);
}