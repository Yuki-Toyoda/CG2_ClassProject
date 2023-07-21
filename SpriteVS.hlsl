#include "Sprite.hlsli"

// hlsl�̃O���[�o���ϐ��Ƃ��Ē�`
ConstantBuffer<TransformaitonMatrix> gTranformationMat : register(b0);

// ���_�V�F�[�_���C���֐�
VertexShaderOutput main(VertexShaderInput input)
{
    // �o�͗p�\���̂�p��
    VertexShaderOutput output;
    // ���͂����̂܂܏o�͂���
    output.position = mul(input.position, gTranformationMat.mat);
    // �o�͂���
    return output;
}
