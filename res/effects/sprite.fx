

//////////////////////

// Uniforms

Texture2D tex; // la texture
SamplerState SampleState; // l’état de sampling

struct VSIn
{
    uint vertexId;
    uint vertexTexid;

};

// -- VSOUT

struct VSOut
{
    float4 Pos : SV_Position;
    float2 uv : TEXCOORD0;
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

VSOut spriteVS(uint id : SV_VertexID)
{
    
    VSOut vso;
    vso.Pos = float4(vertices[id].xy, 0, 1);
    vso.uv = vertices[id].zw;
    
    return vso;
}

////////////////////

// -- Fragment Shader

float4 spritePS(VSOut vs) : SV_Target
{
    
    
    return float4(1,1, 0, 1);
    
}

////////////////////

// -- Technique

technique11 sprite
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, spriteVS()));
        SetPixelShader(CompileShader(ps_5_0, spritePS()));
        SetGeometryShader(NULL);
    }
}