
// �s�N�Z���V�F�[�_�o�͍\����
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// �s�N�Z���V�F�[�_���C���֐�
PixelShaderOutput main()
{
    
    // �o�͗p�\���̂�p�ӂ���
    PixelShaderOutput output;
    // �F��ݒ肷��
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    // �o�͂���
    return output;
    
}