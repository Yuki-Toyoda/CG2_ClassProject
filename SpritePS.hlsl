#include "Sprite.hlsli"

// hlsl�̃O���[�o���ϐ��Ƃ��Ē�`
ConstantBuffer<Material> gMaterial : register(b0);

// �s�N�Z���V�F�[�_���C���֐�
PixelShaderOutput main()
{
    
    // �o�͗p�\���̂�p�ӂ���
    PixelShaderOutput output;
    // �F��ݒ肷��
    output.color = gMaterial.color;
    // �o�͂���
    return output;
    
}