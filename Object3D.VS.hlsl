#include "Object3D.hlsli"
// ���_�V�F�[�_���C���֐�
VertexShaderOutput main(VertexShaderInput input)
{
    // �o�͗p�\���̂�p��
    VertexShaderOutput output;
    // ���͂����̂܂܏o�͂���
    output.position = mul(input.position, mat);
    output.texcoord = input.texcoord;
    // �o�͂���
    return output;
}