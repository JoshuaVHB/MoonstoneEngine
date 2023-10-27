
SamplerState SampleState; // l’état de sampling

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 instancePos : TEXCOORD1;
    float2 instanceUv : TEXCOORD2;
    
    
};

struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    
};

VertexOut sprite2DVS(VertexInput VSin) 
{
    VertexOut vso;
    vso.pos = float4(VSin.pos + VSin.instancePos, 0);
    vso.uv = VSin.uv;
    return vso;
}

float4 sprite2DPS(VertexOut vsin) : SV_Target
{    
    return float4(vsin.uv, 0, 1);
}


technique11 sprite2D
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, sprite2DVS()));
        SetPixelShader(CompileShader(ps_5_0, sprite2DPS()));
        SetGeometryShader(NULL);
    }
}