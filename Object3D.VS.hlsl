#include "Object3D.hlsli"
// 頂点シェーダメイン関数
VertexShaderOutput main(VertexShaderInput input)
{
    // 出力用構造体を用意
    VertexShaderOutput output;
    // 入力をそのまま出力する
    output.position = mul(input.position, mat);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)world));
    // 出力する
    return output;
}