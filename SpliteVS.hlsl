#include "Sprite.hlsli"
// ���_�V�F�[�_���C���֐�
VertexShaderOutput main(VertexShaderInput input)
{
    // �o�͗p�\���̂�p��
    VertexShaderOutput output;
    // ���͍��W��3D�ϊ��s�����Z����
    output.position = mul(mat, input.position);
    // �o�͂���
    return output;
}
