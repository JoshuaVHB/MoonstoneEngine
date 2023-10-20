//////////////////////

// Uniforms

Texture2D albedo; // la texture
Texture2D normal; // la texture
Texture2D position; // la texture
Texture2D unlitTexture; // la texture
SamplerState SampleState; // l’état de sampling

cbuffer worldParams
{
    float4x4 viewProj; // la matrice totale 
    float4 lightPos; // la position de la source d’éclairage (Point) 
    float4 cameraPos; // la position de la caméra 
    
    float4 sunColor; // la valeur ambiante de l’éclairage 
    float sunStrength; // la valeur diffuse de l’éclairage
}


////////////////////////////////////////////////////////////////////////////////

// -- VSOUT

struct VSOut
{
    float4 Pos : SV_Position;
    float2 uv : TEXCOORD0;
};


static const float4 vertices[6] =
{
    float4(-1, -1,-1, -1), // topleft
    float4(-1, +1,-1, +1), // bottomleft
    float4(+1, +1,+1, +1), // bottomright
    
    float4(-1, -1,-1, -1), // topleft
    float4(+1, +1,+1, +1), // bottomright
    float4(+1, -1, +1, -1), // topright
};

////////////////////////////////////////////////////////////////////////////////

// -- Vertex Shader

VSOut lightPassVS(uint id : SV_VertexID)
{
    VSOut vso;
    vso.Pos = float4(vertices[id].xy, 0, 1);
    vso.uv = vertices[id].zw;
    
    return vso;
}

////////////////////

// -- Fragment Shader

float4 lightPassPS(float4 sspos : SV_Position) : SV_Target
{
    float4 diffuse      = albedo.Load(sspos);
    float4 fragnormal   = normal.Load(sspos);
    float4 fragPos      = position.Load(sspos);
    float4 skyBox       = unlitTexture.Load(sspos);
    
    
    float3 lighting = float3(diffuse.rgb);
    float3 viewDir = normalize(cameraPos - fragPos);
    float4 color = float4(lighting, 1);   
    float sunlight = max(0, dot(normalize(fragnormal), normalize(float3(1, 300, 5))));
    color.rgb *= lerp(float3(0.09, 0.09, 0.09), sunColor.rgb, max(.1, sunlight * sunStrength));
    /*

    // Sun coloration
    
    
    float3 R = normalize(2 * sunLight * N - viewDir);
    float S = pow(saturate(dot(R, viewDir)), 4.f);
    color.rgb *= S;*/
    
    if (skyBox.r != 0)
        color.rgb = skyBox.rgb;
    
    return float4(color.rgb, 1.0f);
}

////////////////////


// -- Technique

technique11 gPass
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, lightPassVS()));
        SetPixelShader(CompileShader(ps_5_0, lightPassPS()));
        SetGeometryShader(NULL);
    }
}