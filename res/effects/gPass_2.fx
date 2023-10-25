//////////////////////

// Uniforms

Texture2D tex; // la texture
Texture2D ambiantOcclusion; // la texture
Texture2D albedo; // la texture
Texture2D normalMap; // la texture
Texture2D roughness; // la texture
Texture2D specular; // la texture

SamplerState SampleState; // l’état de sampling


////////////////////

// -- Constant buffers 
cbuffer cameraParams
{
    float4x4 viewProj; // la matrice totale 
}

cbuffer meshParams
{
    float4x4 worldMat; // matrice de transformation dans le monde
};


/////////////////////

// -- VSOUT

struct VSOut
{
    float4 Pos : SV_Position;
    float3 Norm : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

struct PSOut
{
    float4 Normal : SV_Target0; //Normal map
    float4 Diffuse : SV_Target1; //Color
    float4 Position : SV_Target2;
    float4 Specular : SV_Target3;
    float4 ambiantOcclusion : SV_Target4;
    float4 roughness : SV_Target5;
};


////////////////////

// -- Vertex Shader

VSOut gPassVS(float4 Pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    float4x4 MVP = mul(worldMat, viewProj);
    VSOut vs_out = (VSOut) 0;
    vs_out.Pos = mul(Pos, MVP);
    vs_out.Norm = mul(float4(normal, 0.0f), worldMat).xyz;
    vs_out.worldPos = mul(Pos, worldMat);
    vs_out.uv = uv;
    
    return vs_out;
}

////////////////////

// -- Fragment Shader

PSOut gPassPS(VSOut vs) : SV_Target
{
    float3 outNormal = normalize(vs.Norm);
    float3 outAlbedo;
    float3 texSample = tex.Sample(SampleState, vs.uv).rgb;
    outAlbedo.rgb = texSample;

    PSOut pso;
    pso.Normal = float4(outNormal, 1.0F);
    pso.Diffuse = float4(outAlbedo,1.0);
    pso.Position = vs.worldPos;
    pso.Specular = float4(specular.Sample(SampleState, vs.uv).rgb, 1);
    pso.ambiantOcclusion = float4(ambiantOcclusion.Sample(SampleState, vs.uv).rgb ,1);
    pso.roughness = float4(roughness.Sample(SampleState, vs.uv).rgb ,1);
    
    return pso;
    
}

////////////////////

// -- Technique

technique11 gPass
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, gPassVS()));
        SetPixelShader(CompileShader(ps_5_0, gPassPS()));
        SetGeometryShader(NULL);
    }
}