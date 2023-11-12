//////////////////////

// GBUFFER

Texture2D albedo; // la texture
Texture2D normal; // la texture
Texture2D position; // la texture
Texture2D ambiantOcclusion; // la texture
Texture2D roughness; // la texture
Texture2D specular; // la texture
Texture2D unlitTexture; // la texture
SamplerState SampleState; // l’état de sampling

// -- Constant buffers 
cbuffer cameraParams
{
    float4x4 viewProj; // la matrice totale 
    float4 cameraPos; // la position de la caméra 
}


float4 sunPos = float4(100.0f, 1000.f, 10.0f, 1.0f); // la position de la source d’éclairage (Point) 
float3 sunDir = normalize(float3(100.0f, 100.f, 10.0f)); // la position de la source d’éclairage (Point) 
float4 sunColor = float4(1.f, 1.f, .6f, 1.f); // la valeur ambiante de l’éclairage 
float4 sunStrength = float4(1, 1, 1, 1); // la valeur diffuse de l’éclairage

int LIGHT_COUNT = 32;
struct Light
{
    float4 direction; // For directional lights and spotlight
    float4 range;
    float4 position;
    float4 ambiant;
    float4 diffuse;
    
    float falloff;
    float radius;
    float strength;
    float isOn;    
    
    uint type;
    float3 padding;
};

cbuffer lightsBuffer
{
    Light lights[32];
    
};


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
    int actualId = int (id);
    vso.Pos = float4(vertices[actualId].xy, 0, 1);
    vso.uv = vertices[actualId].zw;
    
    return vso;
}

////////////////////


/////////////////////

float toonify(float diffuse)
{
    return smoothstep(-0.05f, +0.05f, diffuse - 0.5f) * diffuse;
}


float3 computeSunDiffuseLight(float3 normal)
{
    
    return lerp(float3(0.19, 0.19, 0.19), sunColor.rgb, max(.3, toonify(saturate(dot(normal, sunDir))) * sunStrength.x));

}


// -- Fragment Shader

float4 lightPassPS(float4 sspos : SV_Position) : SV_Target
{
    float4 diffuse      = albedo.Load(sspos);
    float4 fragnormal   = normal.Load(sspos);
    float4 fragPos      = position.Load(sspos);
    float specularPixel = specular.Load(sspos).x;
    float4 skyBox       = unlitTexture.Load(sspos);
    
        
    if (skyBox.r != 0) return float4(skyBox.rgb, 1.0f);
    
    float3 viewDir = normalize(cameraPos.xyz - fragPos.xyz);
    
    float3 lighting = diffuse.rgb * 0.3F;
    lighting += sunColor * 0.4 * smoothstep(-0.1f, 0.1f, saturate(dot(fragnormal, normalize(sunDir))) - 0.65f);
    //lighting += lights[0].diffuse.rgb;
    
    for (int i = 0; i < LIGHT_COUNT; ++i)
    {
        if (lights[i].isOn < 1.0f) continue;
        float dist = length(lights[i].position.rgb - fragPos.rgb);
        // diffuse
        //if (dist < lights[i].radius)
        {
        
                // diffuse
            float3 lightDir = normalize(lights[i].position - fragPos);
            float3 ds = max(dot(fragnormal.xyz, lightDir), 0.0) * diffuse * lights[i].diffuse;
            
            
            float attenuation = 1.0 / (dist);
            ds *= attenuation;
            lighting += ds;
            
        }
    }   
      
    
    return float4(lighting, 1.0f) + float4(specularPixel * 0.4 * diffuse.rgb * float3(1,1,1), 0.F);
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