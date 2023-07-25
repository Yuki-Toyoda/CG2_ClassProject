// 変換行列
cbuffer constBuffer : register(b0)
{
    float4 color;
    float4x4 mat;
}

// 頂点シェーダ出力用
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};
// 頂点シェーダ入力用
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

// マテリアル
struct Material
{
    // 色
    float4 color;
};
// ピクセルシェーダ出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};