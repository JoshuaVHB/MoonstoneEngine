//////////////////////

// Uniforms

Texture2D whitePixel; // la texture
Texture2D tex_slots[8];

SamplerState SampleState; // l’état de sampling

struct VSIn
{
    float4 pos : POSITION;
    float4 uv : TEXCOORD;
    float4 texId : TEXID;
};

// -- VSOUT

struct VSOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    int texId : TEXCOORD1;
    float2 size : TEXCOORD2;
};

static const float4 vertices[6] =
{
    float4(-1, -1, 0, 0),
    float4(+1, +1, 1, 1),
    float4(+1, -1, 1, 0),
    float4(-1, +1, 0, 1),
    float4(+1, +1, 1, 1),
    float4(-1, -1, 0, 0)
};

////////////////////

// -- Vertex Shader

VSOut spriteVS(VSIn vsin, uint id : SV_VertexID)
{
    
    VSOut vso;
    vso.pos     = float4((vertices[id].xy * vsin.pos.zw) + vsin.pos.xy, 0, 1);
    vso.uv      = float2((vertices[id].zw * vsin.uv.zw) + vsin.uv.xy);
    vso.texId   = int(vsin.texId.x);
    vso.size    = float2(vsin.pos.zw);
    return vso;
}

////////////////////

// -- Fragment Shader

float4 spritePS(VSOut vs) : SV_Target
{
        
    return float4(1, 1, 0, 1);
    
}
////////////////////

// -- Technique

technique11 quad
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, spriteVS()));
        SetPixelShader(CompileShader(ps_5_0, spritePS()));
        SetGeometryShader(NULL);
    }
}