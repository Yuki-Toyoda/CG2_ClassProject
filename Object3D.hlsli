cbuffer constBuffer : register(b0)
{
    float4 color;
    float4x4 mat;
    float4x4 world;
    int enableLighting;
}

// 頂点シェーダ出力用
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0; 
};
// 頂点シェーダ入力用
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// ピクセルシェーダ出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// 平行光源の数
static const int kDirectionalLight = 1;
// 平行光源
struct DirectionalLight
{
    float4 color; // ライト色
    float3 direciton; // ライト方向
    float intensity; // 輝度
    uint active; // 有効トリガー
};

cbuffer LightGroup : register(b1)
{
    DirectionalLight directionalLight[kDirectionalLight];
}