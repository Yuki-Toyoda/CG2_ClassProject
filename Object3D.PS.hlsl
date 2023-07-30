#include "Object3D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// �s�N�Z���V�F�[�_���C���֐�
PixelShaderOutput main(VertexShaderOutput input)
{
    
    // �o�͗p�\���̂�p�ӂ���
    PixelShaderOutput output;
    
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // ���s����
    for (int i = 0; i < kDirectionalLight; i++)
    {
        if (directionalLight[i].active)
        {
            float NdotL = dot(normalize(input.normal), -normalize(directionalLight[i].direciton));
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color = color * textureColor * directionalLight[i].color * cos * directionalLight[i].intensity;
        }
        else
        {
         // �F��ݒ肷��
            output.color = color * textureColor;
        }
    }
    
    // �o�͂���
    return output;
    
}