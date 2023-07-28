cbuffer constBuffer : register(b0)
{
    float4 color;
    float4x4 mat;
    float4x4 world;
    int enableLighting;
}

// ���_�V�F�[�_�o�͗p
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0; 
};
// ���_�V�F�[�_���͗p
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// �s�N�Z���V�F�[�_�o�͍\����
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// ���s�����̐�
static const int kDirectionalLight = 1;
// ���s����
struct DirectionalLight
{
    float4 color; // ���C�g�F
    float3 direciton; // ���C�g����
    float intensity; // �P�x
    uint active; // �L���g���K�[
};

cbuffer LightGroup : register(b1)
{
    DirectionalLight directionalLight[kDirectionalLight];
}