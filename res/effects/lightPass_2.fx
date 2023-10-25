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


int LIGHT_COUNT = 128;
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
    
};

cbuffer lightsBuffer
{
    Light lights[128];
    
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




// -- Fragment Shader

float4 lightPassPS(float4 sspos : SV_Position) : SV_Target
{
    float4 diffuse      = albedo.Load(sspos);
    float4 fragnormal   = normal.Load(sspos);
    float4 fragPos      = position.Load(sspos);
    float specularPixel = max(specular.Load(sspos).a, 0.01);
    float4 skyBox       = unlitTexture.Load(sspos);
    
        
    if (skyBox.r != 0) return float4(skyBox.rgb, 1.0f);
    
    float3 viewDir = normalize(cameraPos.xyz - fragPos.xyz);
    
    float3 lighting = diffuse.rgb * 0.1f;
    //lighting += lights[0].diffuse.rgb;
    
    for (int i = 0; i < 8; ++i)
    {
      //  if (lights[i].isOn <= 1.0f) continue;
        float dist = length(lights[i].position.rgb - fragPos.rgb);
        // diffuse
        if (dist < lights[i].radius)
        {
            
            float3 lightDir = normalize(lights[i].position.rgb - fragPos.rgb);
            float d = dot(fragnormal.rgb, lightDir) * diffuse.rgb
            *lights[i].diffuse.rgb;
            
            
            float3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(fragnormal.rgb, halfwayDir), 0.0), 16.0);
            float3 specular = lights[i].diffuse.rgb * spec * specularPixel;
            // attenuation
            float attenuation = 1.0 ;
            d *= attenuation;
            specular *= attenuation;
            
            
            lighting += d + specular;
        }
    }   
    
      
    
    return float4(lighting, 1.0f);
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