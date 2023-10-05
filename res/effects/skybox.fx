
// Uniforms

TextureCube tex; // la texture
SamplerState sam; // l’état de sampling

cbuffer SkyboxCbuffer
{
    matrix viewProj;
    
};

struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
    
};


VSOut skyboxVS(float3 pos: POSITION)
{
    VSOut vso;
    vso.worldPos = pos;
    vso.pos = mul(float4(pos, 0.0f), viewProj); // nullify translation by removing w component
    vso.pos.z = vso.pos.w; // z stays at 1 after w division
    return vso;
    
}


float4 skyboxFS(float3 worldPos : SV_POSITION) : SV_TARGET
{
    return tex.Sample(sam, worldPos);
}

technique11 skybox
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, skyboxVS()));
        SetPixelShader(CompileShader(ps_5_0, skyboxFS()));
        SetGeometryShader(NULL);
    }
}