
// ピクセルシェーダ出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// ピクセルシェーダメイン関数
PixelShaderOutput main()
{
    
    // 出力用構造体を用意する
    PixelShaderOutput output;
    // 色を設定する
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    // 出力する
    return output;
    
}