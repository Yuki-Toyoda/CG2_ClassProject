#include "Sprite.hlsli"
// ピクセルシェーダメイン関数
PixelShaderOutput main()
{
    
    // 出力用構造体を用意する
    PixelShaderOutput output;
    // 色を設定する
    output.color = color;
    // 出力する
    return output;
    
}