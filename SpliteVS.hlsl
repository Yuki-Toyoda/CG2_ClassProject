
// 頂点シェーダ出力用
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

// 頂点シェーダ入力用
struct VertexShaderInput
{
    float4 position : POSITION0;
};

// 頂点シェーダメイン関数
VertexShaderOutput main(VertexShaderInput input)
{
    // 出力用構造体を用意
    VertexShaderOutput output;
    // 入力をそのまま出力する
    output.position = input.position;
    // 出力する
    return output;
}
