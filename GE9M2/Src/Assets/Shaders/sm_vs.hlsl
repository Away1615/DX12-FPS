#define MAX_POINT_LIGHTS 10

cbuffer smCB : register(b0) {
    float4x4 W;
    float4x4 VP;
    
    float3 time;
    int useVSAnim;
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float4 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

PS_INPUT VS(VS_INPUT input) {
    
    float3 pos = input.Pos;
    
    // Vertex animation
    if (useVSAnim == 1) {
        float windStrength = 0.2f;
        float windSpeed = 1.2f;

        float t = time.x * windSpeed;
        pos.x += sin(t) * windStrength;
    }
    
    PS_INPUT output;
    // --- Position ---
    float4 worldPos = mul(float4(pos, 1.0f), W);
    output.PosWS = worldPos;
    output.Pos = mul(worldPos, VP);
    
    // --- NormalWS ---
    output.NormalWS = normalize(mul(input.Normal, (float3x3) W));
    
    // --- TangentWS ---
    output.TangentWS = normalize(mul(input.Tangent, (float3x3) W));
    
    // --- TexCoords ---
    output.TexCoords = input.TexCoords;

    return output;
}