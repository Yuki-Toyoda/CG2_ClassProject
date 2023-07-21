// マテリアル
struct Material
{
    // 色
    float4 color;
};

// hlslのグローバル変数として定義
ConstantBuffer<Material> gMaterial : register(b0);

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
    output.color = gMaterial.color;
    // 出力する
    return output;
    
}