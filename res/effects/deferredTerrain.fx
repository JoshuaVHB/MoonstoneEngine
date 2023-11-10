//////////////////////

// Uniforms

Texture2D ambiantOcclusion; // la texture
Texture2D albedo; // la texture
Texture2D normalMap; // la texture
Texture2D roughness; // la texture
Texture2D specular; // la texture

Texture2D grassTexture; // la texture
Texture2D rockTexture; // la texture
Texture2D snowTexture; // la texture

SamplerState SampleState; // l’état de sampling


////////////////////


float4 sunPos = float4(100.0f, 1000.f, 10.0f, 1.0f ); // la position de la source d’éclairage (Point) 
float4 sunColor = float4(1.f, 1.f, .8f, 1.f); // la valeur ambiante de l’éclairage 
float4 sunStrength = float4(1,1,1,1); // la valeur diffuse de l’éclairage

// -- Constant buffers 

cbuffer cameraParams
{
    float4x4 viewProj; // la matrice totale 
    float4 cameraPos; // la position de la caméra     
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

VSOut deferredTerrainVS(float4 Pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
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

PSOut deferredTerrainPS(VSOut vs) : SV_Target
{
    

    float3 outNormal = normalize(vs.Norm);
    float3 outAlbedo;
    

    
    // -- Compute specular
    

    
    float3 N = normalize(vs.Norm);
    float3 L = normalize(sunPos - vs.worldPos);
    float3 V = normalize(cameraPos - vs.worldPos);
    float3 H = normalize(L + V);
    float diff = saturate(dot(N, L));
    float S = pow(saturate(dot(H, N)), 4.f);
    
    
     // -- Compute albedo
    
    float3 rockSample = rockTexture.Sample(SampleState, vs.uv).rgb;
    float3 grassSample = grassTexture.Sample(SampleState, vs.uv).rgb;
    float3 snowSample = snowTexture.Sample(SampleState, vs.uv).rgb;
    
    float3 baseTexture = lerp(grassSample, snowSample, (vs.worldPos.y) / 100.f);
    outAlbedo.rgb = lerp(rockSample, baseTexture, smoothstep(0.79, 1, outNormal.y));
    outAlbedo.rgb *= lerp(float3(0.09, 0.09, 0.09), sunColor.rgb, max(.1, diff * sunStrength.x));
    // -- Output to RTV
    
    PSOut pso;
    pso.Normal = float4(outNormal, 1.0F);
    pso.Diffuse = float4(outAlbedo, 1.0);
    pso.Position = vs.worldPos;
    pso.Specular = float4(S, S, S, 1);
    pso.ambiantOcclusion = float4(ambiantOcclusion.Sample(SampleState, vs.uv).rgb, 1);
    pso.roughness = float4(roughness.Sample(SampleState, vs.uv).rgb, 1);
    
    return pso;
    
}

////////////////////

// -- Technique

technique11 gPass
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, deferredTerrainVS()));
        SetPixelShader(CompileShader(ps_5_0, deferredTerrainPS()));
        SetGeometryShader(NULL);
    }
}