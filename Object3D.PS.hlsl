#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ピクセルシェーダメイン関数
PixelShaderOutput main(VertexShaderOutput input)
{
    
    // 出力用構造体を用意する
    PixelShaderOutput output;
    
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 色を設定する
    output.color = color * textureColor;
    // 出力する
    return output;
    
}