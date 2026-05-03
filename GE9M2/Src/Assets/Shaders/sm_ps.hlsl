#define MAX_POINT_LIGHTS 10
#define PI 3.1415926

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState samplerLinear : register(s0);

cbuffer smCB : register(b0) {
    float2 uvScale;
    int useAlphaTest;
    
    float skyLightIntensity;
    float3 skyLightColor;
    
    int pointLightCount;
    float4 lightPosWS[MAX_POINT_LIGHTS];
    float4 lightColor[MAX_POINT_LIGHTS];
    float4 lightParams[MAX_POINT_LIGHTS]; // x range, y intensity
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float4 PosWS : TEXCOORD0;
    float3 NormalWS : TEXCOORD1;
    float3 TangentWS : TEXCOORD2;
    float2 TexCoords : TEXCOORD3;
};

float3x3 getTBN(PS_INPUT input)
{
    float3 N = normalize(input.NormalWS);
    float3 T = normalize(input.TangentWS);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    return TBN;
}

float4 PS(PS_INPUT input) : SV_Target0 {
    
    // --- Albedo ---
    float4 albedoSample = albedoTex.Sample(samplerLinear, input.TexCoords * uvScale);

    // --- Alpha Test ---
    if (useAlphaTest == 1 && albedoSample.a < 0.5f)
        discard;

    float3 albedo = albedoSample.rgb;
    //return float4(albedo, 1.0);

    // --- TBN ---
    float3 normalWS = normalize(input.NormalWS);

    float3x3 TBN = getTBN(input);
    
    float3 mapNormal = normalTex.Sample(samplerLinear, input.TexCoords * uvScale).xyz;
    mapNormal = mapNormal * 2.0 - 1.0;
    normalWS = normalize(mul(mapNormal, TBN));

    // --- Sky Light ---
    float3 lighting = albedo * skyLightColor * skyLightIntensity;
    
    // --- Point Light ---
    for (int i = 0; i < pointLightCount; ++i) {
        float3 L = lightPosWS[i].xyz - input.PosWS.xyz;
        float dist = length(L);
        float3 lightDir = normalize(L);
        
        // attenuation
        float attenuation = saturate(1.0 - dist / lightParams[i].x);
        attenuation *= attenuation;
        
        // Lambert
        float NdotL = max(dot(normalWS, lightDir), 0.0);
        float3 diffuse = (albedo / PI) * lightColor[i].xyz * NdotL;

        lighting += diffuse * lightParams[i].y * attenuation;
    }

    return float4(lighting, 1.0);
}