cbuffer debugCB : register(b0)
{
    float4x4 W;
    float4x4 VP;
    float2 uvScale;
    float2 _padding;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};
struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, W);
    pos = mul(pos, VP);
    output.pos = pos;
    return output;
}