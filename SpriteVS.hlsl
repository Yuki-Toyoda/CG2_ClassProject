#include "Sprite.hlsli"

// hlslのグローバル変数として定義
ConstantBuffer<TransformaitonMatrix> gTranformationMat : register(b0);

// 頂点シェーダメイン関数
VertexShaderOutput main(VertexShaderInput input)
{
    // 出力用構造体を用意
    VertexShaderOutput output;
    // 入力をそのまま出力する
    output.position = mul(input.position, gTranformationMat.mat);
    // 出力する
    return output;
}
