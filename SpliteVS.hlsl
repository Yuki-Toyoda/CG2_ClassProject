#include "Sprite.hlsli"
// 頂点シェーダメイン関数
VertexShaderOutput main(VertexShaderInput input)
{
    // 出力用構造体を用意
    VertexShaderOutput output;
    // 入力座標に3D変換行列を乗算する
    output.position = mul(mat, input.position);
    // 出力する
    return output;
}
