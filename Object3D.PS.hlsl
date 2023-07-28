#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// �s�N�Z���V�F�[�_���C���֐�
PixelShaderOutput main(VertexShaderOutput input)
{
    
    // �o�͗p�\���̂�p�ӂ���
    PixelShaderOutput output;
    
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // �F��ݒ肷��
    output.color = color * textureColor;
    // �o�͂���
    return output;
    
}