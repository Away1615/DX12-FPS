
struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    return float4(1, 0, 0, 1);
}
