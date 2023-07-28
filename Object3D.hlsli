// �ϊ��s��
cbuffer constBuffer : register(b0)
{
    float4 color;
    float4x4 mat;
}

// ���_�V�F�[�_�o�͗p
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};
// ���_�V�F�[�_���͗p
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

// �}�e���A��
struct Material
{
    // �F
    float4 color;
};
// �s�N�Z���V�F�[�_�o�͍\����
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};