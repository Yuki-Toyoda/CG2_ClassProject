
// ���_�V�F�[�_�o�͗p
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

// ���_�V�F�[�_���͗p
struct VertexShaderInput
{
    float4 position : POSITION0;
};

// ���_�V�F�[�_���C���֐�
VertexShaderOutput main(VertexShaderInput input)
{
    // �o�͗p�\���̂�p��
    VertexShaderOutput output;
    // ���͂����̂܂܏o�͂���
    output.position = input.position;
    // �o�͂���
    return output;
}
