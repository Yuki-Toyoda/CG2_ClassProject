#include "Object3D.hlsli"
// ���_�V�F�[�_���C���֐�
VertexShaderOutput main(VertexShaderInput input)
{
    // �o�͗p�\���̂�p��
    VertexShaderOutput output;
    // ���͂����̂܂܏o�͂���
    output.position = mul(input.position, mat);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)world));
    // �o�͂���
    return output;
}