#include "Pera.hlsli"

// モノクロ(グレースケール)
float GrayScale(float4 _Col);
// 色の反転
float4 ReverseColor(float4 _Col);
// 色の階調を落す
float4 DownTone(float4 _Col);
// 画素の平均化によるぼかし
float4 BlurPixelAverage(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// エンボス効果
float4 Emboss(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// シャープネス(エッジの強化)
float4 Sharpness(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
//輪郭線抽出
float TakeOutline(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy);
// ガウシアンぼかし(簡易版)
float4 Get5x5GaussianBlur(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy, float4 _Rect);


float4 Pera(Output input) : SV_Target
{
    float4 col = tex.Sample(smp, input.uv); // 加工なし
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels);     // levels...ミップマップのレベル
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);        // 返り値

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
    tex.GetDimensions(0, w, h, levels); // levels...ミップマップのレベル
    float dx = 1.0 / w;
    float dy = 1.0 / h;

    BlurOutput ret;
    ret.col = tex.Sample(smp, input.uv);
    ret.highLum = Get5x5GaussianBlur(texHighLum, smp, input.uv, dx, dy, float4(0, 0, 1, 1));
    
    return ret;
}


float4 HorizontalBokehPS(Output input) : SV_TARGET
{
    float4 col = tex.Sample(smp, input.uv); // 加工なし
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels); // levels...ミップマップのレベル
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0); // 返り値


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
    float4 col = tex.Sample(smp, input.uv); // 加工なし
    
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels); // levels...ミップマップのレベル
    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0); // 返り値


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
    // 画素の平均化によるぼかし
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy));    // 左上
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx, -2 * _Dy));    // 上
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy));    // 右上
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0 * _Dy));    // 左
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx,  0 * _Dy));    // 中央
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0 * _Dy));    // 右
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy));    // 左下
    ret += _Tex.Sample(_Smp, _Uv + float2( 0 * _Dx,  2 * _Dy));    // 下
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy));    // 右下
    return ret / 9.0f;
}

float4 Emboss(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // エンボス効果
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy)) *  2;    // 左上
    ret += _Tex.Sample(_Smp, _Uv + float2( 0      , -2 * _Dy));         // 上
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy)) *  0;    // 右上
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     ));          // 左
    ret += _Tex.Sample(_Smp, _Uv);                                      // 中央
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;     // 右
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy)) *  0;    // 左下
    ret += _Tex.Sample(_Smp, _Uv + float2(0       ,  2 * _Dy)) + -1;    // 下
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy)) * -2;    // 右下
    return ret;
}

float4 Sharpness(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // シャープネス(エッジの強調)
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx, -2 * _Dy)) *  0;    // 左上
    ret += _Tex.Sample(_Smp, _Uv + float2( 0      , -2 * _Dy)) * -1;    // 上
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx, -2 * _Dy)) *  0;    // 右上
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     )) * -1;     // 左
    ret += _Tex.Sample(_Smp, _Uv)                             *  5;      // 中央
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;     // 右
    
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  2 * _Dy)) *  0;    // 左下
    ret += _Tex.Sample(_Smp, _Uv + float2(0       ,  2 * _Dy)) + -1;    // 下
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  2 * _Dy)) *  0;    // 右下
    return ret;
}

float TakeOutline(Texture2D<float4> _Tex, SamplerState _Smp, float2 _Uv, float _Dx, float _Dy)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // 輪郭線抽出
    ret += _Tex.Sample(_Smp, _Uv + float2( 0     , -2 * _Dy)) * -1;   // 上
    ret += _Tex.Sample(_Smp, _Uv + float2(-2 * _Dx,  0     )) * -1;   // 左
    ret += _Tex.Sample(_Smp, _Uv)                             *  4;   // 中央
    ret += _Tex.Sample(_Smp, _Uv + float2( 2 * _Dx,  0     )) * -1;   // 右
    ret += _Tex.Sample(_Smp, _Uv + float2(0      ,  2 * _Dy)) + -1;   // 下
    //// 反転
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
    // 最上段
      _Tex.Sample(_Smp, _Uv + float2(l2 , u2)).rgb
    + _Tex.Sample(_Smp, _Uv + float2(l1 , u2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2( 0 , u2)).rgb *  6
    + _Tex.Sample(_Smp, _Uv + float2(r1 , u2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(r2 , u2)).rgb
    // 上段
    + _Tex.Sample(_Smp, _Uv + float2(l2 , u1)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(l1 , u1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2( 0 , u1)).rgb * 24
    + _Tex.Sample(_Smp, _Uv + float2(r1 , u1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2(r2 , u1)).rgb * 4
    // 中段
    + _Tex.Sample(_Smp, _Uv + float2(l2 , 0)) .rgb*  6
    + _Tex.Sample(_Smp, _Uv + float2(l1 , 0)) .rgb* 24
    + ret.rgb * 36
    + _Tex.Sample(_Smp, _Uv + float2(r1 , 0)) .rgb* 24
    + _Tex.Sample(_Smp, _Uv + float2(r2 , 0)).rgb * 6
    // 下段
    + _Tex.Sample(_Smp, _Uv + float2(l2 , d1)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(l1 , d1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2( 0 , d1)).rgb * 24
    + _Tex.Sample(_Smp, _Uv + float2(r1 , d1)).rgb * 16
    + _Tex.Sample(_Smp, _Uv + float2(r2 , d1)).rgb * 4
    // 最下段
    + _Tex.Sample(_Smp, _Uv + float2(l2 , d2)).rgb
    + _Tex.Sample(_Smp, _Uv + float2(l1 , d2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2( 0 , d2)).rgb *  6
    + _Tex.Sample(_Smp, _Uv + float2(r1 , d2)).rgb *  4
    + _Tex.Sample(_Smp, _Uv + float2(r2 , d2)).rgb
    ) / 256, ret.a);
}