#include "Sprite.hlsli"

// hlslのグローバル変数として定義
ConstantBuffer<Material> gMaterial : register(b0);

// ピクセルシェーダメイン関数
PixelShaderOutput main()
{
    
    // 出力用構造体を用意する
    PixelShaderOutput output;
    // 色を設定する
    output.color = gMaterial.color;
    // 出力する
    return output;
    
}