//////////////////////

// Uniforms

Texture2D tex; // la texture
SamplerState SampleState; // l��tat de sampling


////////////////////

// -- Constant buffers 
cbuffer worldParams
{
    float4x4 viewProj; // la matrice totale 
    float4 lightPos; // la position de la source d��clairage (Point) 
    float4 cameraPos; // la position de la cam�ra 
    
    float4 sunColor; // la valeur ambiante de l��clairage 
    float sunStrength; // la valeur diffuse de l��clairage
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
    float3 lightDir : TEXCOORD1;
    float3 camDir : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

struct PSOut
{
    float4 Normal : SV_Target0; //Normal map
    float4 Diffuse : SV_Target1; //Color
    float4 Position : SV_Target2;
};


////////////////////

// -- Vertex Shader

VSOut gPassVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 uv : TEXCOORD)
{
    float4x4 MVP = mul(worldMat, viewProj);
    VSOut vs_out = (VSOut) 0;
    vs_out.Pos = mul(Pos, MVP);
    vs_out.Norm = mul(float4(Normale, 0.0f), worldMat).xyz;
    
    float3 worldPos = mul(Pos, worldMat).xyz;
    vs_out.lightDir = lightPos.xyz - worldPos;
    vs_out.camDir = cameraPos.xyz - worldPos;
    
    vs_out.uv = uv;
    return vs_out;
}

////////////////////

// -- Fragment Shader

PSOut gPassPS(VSOut vs) : SV_Target
{
    float3 N = normalize(vs.Norm);
    float3 couleur;
    float3 texSample = tex.Sample(SampleState, vs.uv).rgb;
    couleur.rgb = texSample;
    /*

    // Normaliser les param�tres 
    float3 L = normalize(vs.lightDir);
    float3 V = normalize(vs.camDir);
    
    // Valeur de la composante diffuse 
    float3 sunLight = saturate(dot(N, L));
    float3 R = normalize(2 * sunLight * N - L);
    // Puissance de 4 - pour l�exemple
    float S = pow(saturate(dot(R, L)), 4.f);

    
    couleur.rgb *= lerp(float3(0.09, 0.09, 0.09), sunColor.rgb, max(.1, sunLight * sunStrength));
    couleur += S / 4.f;
    */
    PSOut pso;
    pso.Normal = float4(N, 1.0F);
    pso.Diffuse = float4(couleur, 1.0F);
    pso.Position = vs.Pos;
    
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