#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ピクセルシェーダメイン関数
PixelShaderOutput main(VertexShaderOutput input)
{
    
    // 出力用構造体を用意する
    PixelShaderOutput output;
    
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 平行光源
    for (int i = 0; i < kDirectionalLight; i++)
    {
        if (directionalLight[i].active)
        {
            float NdotL = dot(normalize(input.normal), -normalize(directionalLight[i].direciton));
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color = color * textureColor * directionalLight[i].color * cos * directionalLight[i].intensity;
        }
        else
        {
         // 色を設定する
            output.color = color * textureColor;
        }
    }
    
    // 出力する
    return output;
    
}