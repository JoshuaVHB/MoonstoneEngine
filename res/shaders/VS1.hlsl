cbuffer param { 
	float4x4 matWorldViewProj; 
}


float4 VS1(float4 Pos_in : POSITION) : SV_Position 
{
	return mul(Pos_in, matWorldViewProj);
}