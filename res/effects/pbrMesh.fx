//////////////////////

// Uniforms

Texture2D albedo;
Texture2D normal;
Texture2D roughness;
Texture2D specular;
Texture2D ambiantOcclusion;


SamplerState SampleState; // l’état de sampling


////////////////////

// -- Constant buffers 
cbuffer worldParams
{
    float4x4 viewProj; // la matrice totale 
    float4 lightPos; // la position de la source d’éclairage (Point) 
    float4 cameraPos; // la position de la caméra 
    
    float4 sunColor; // la valeur ambiante de l’éclairage 
    float sunStrength; // la valeur diffuse de l’éclairage
}

cbuffer meshParams
{
    float4x4 worldMat; // matrice de transformation dans le monde
};

cbuffer materialParams
{
    float4  Ka; // ambiant	
    float4  Kd; // diffuse
    float4  Ks; // specular
    float4  Ke; // emissive	
    float   Ns; // exponent specular	
    float   Ni; // refraction, optical density	
    float   transparency; // 1 - d

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

////////////////////

// -- Vertex Shader

VSOut baseMeshVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 uv : TEXCOORD)
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

float3 cookTorrence(
    float3 materialDiffuseColor,
	float3 materialSpecularColor,
	float3 normal,
	float3 lightDir,
	float3 viewDir,
	float3 lightColor)
{
    float roughness = 0.1f;
    float k = 0.2f;
    float NdotL = max(0, dot(normal, lightDir));
    float Rs = 0.0;
    if (NdotL > 0)
    {
        float3 H = normalize(lightDir + viewDir);
        float NdotH = max(0, dot(normal, H));
        float NdotV = max(0, dot(normal, viewDir));
        float VdotH = max(0, dot(lightDir, H));

		// Fresnel reflectance
        float F = pow(1.0 - VdotH, 5.0);
        F *= (1.0 - 0.8);
        F += 0.8;

		// Microfacet distribution by Beckmann
        float m_squared = roughness * roughness;
        float r1 = 1.0 / (4.0 * m_squared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (m_squared * NdotH * NdotH);
        float D = r1 * exp(r2);

		// Geometric shadowing
        float two_NdotH = 2.0 * NdotH;
        float g1 = (two_NdotH * NdotV) / VdotH;
        float g2 = (two_NdotH * NdotL) / VdotH;
        float G = min(1.0, min(g1, g2));

        Rs = (F * D * G) / (3.14159 * NdotL * NdotV);
    }
    return materialDiffuseColor * lightColor * NdotL + lightColor * materialSpecularColor * NdotL * (k + Rs * (1.0 - k));
}


float4 baseMeshPS(VSOut vs) : SV_Target
{
    float3 couleur;

    // Normaliser les paramètres 
    float3 N = normalize(vs.Norm);
    float3 L = normalize(vs.lightDir);
    float3 V = normalize(vs.camDir);
    
    // Valeur de la composante diffuse 
    float3 sunLight = saturate(dot(N, L));
    float3 R = normalize(2 * sunLight * N - L);
    // Puissance de 4 - pour l’exemple
    float S = pow(saturate(dot(R, L)), 4.f);

    float3 texSample = albedo.Sample(SampleState, vs.uv).rgb;
    
    //couleur.rgb = texSample;
    
    couleur.rgb = Ka.rgb;
    couleur.rgb *= lerp(float3(0.09, 0.09, 0.09), sunColor.rgb, max(.1, sunLight * sunStrength));
    couleur += S / 4.f;
    return float4(couleur.rgb, transparency);
}

////////////////////

// -- Technique

technique11 MiniPhong
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, baseMeshVS()));
        SetPixelShader(CompileShader(ps_5_0, baseMeshPS()));
        SetGeometryShader(NULL);
    }
}