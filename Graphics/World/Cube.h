#pragma once

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include <cstdint>

#include "Mesh.h"


using namespace DirectX;

class Cube : Mesh {


public:


	static Mesh getCubeMesh(
		ID3D11Device* device,
		ID3D11DeviceContext* context 
		) 
	
	{
		float dx = 1, dy = 1, dz = 1;

		XMVECTOR n0{ 0.0f, 0.0f, -1.0f }; // devant
		XMVECTOR n1{ 0.0f, 0.0f, 1.0f }; // arrière
		XMVECTOR n2{ 0.0f, -1.0f, 0.0f }; // dessous
		XMVECTOR n3{ 0.0f, 1.0f, 0.0f }; // dessus
		XMVECTOR n4{ -1.0f, 0.0f, 0.0f }; // face gauche
		XMVECTOR n5{ 1.0f, 0.0f, 0.0f }; // face droite

		XMVECTOR point[8] =
		{
			{-dx / 2, dy / 2, -dz / 2	},
			{dx / 2, dy / 2, -dz / 2	},
			{dx / 2, -dy / 2, -dz / 2	},
			{-dx / 2, -dy / 2, -dz / 2	},
			{-dx / 2, dy / 2, dz / 2	},
			{-dx / 2, -dy / 2, dz / 2	},
			{dx / 2, -dy / 2, dz / 2	},
			{dx / 2, dy / 2, dz / 2		}
		};

		std::vector<Vertex> vertices =
		{
			{point[0], n0},	{point[1], n0},	{point[2], n0},
			{point[3], n0},	{point[4], n1},	{point[5], n1},
			{point[6], n1},	{point[7], n1},	{point[3], n2},
			{point[2], n2},	{point[6], n2}, {point[5], n2},
			{point[0], n3},	{point[4], n3},	{point[7], n3},
			{point[1], n3},	{point[0], n4},	{point[3], n4},
			{point[5], n4},	{point[4], n4},	{point[1], n5},
			{point[7], n5},	{point[6], n5},	{point[2], n5},
		};
		std::vector<uint16_t> indices
		{
			0,1,2, // devant 
			0,2,3, // devant 

			5,6,7, // arrière 
			5,7,4, // arrière

			8,9,10, // dessous
			8,10,11, // dessous 

			13,14,15, // dessus 
			13,15,12, // dessus 

			19,16,17, // gauche 
			19,17,18, // gauche 

			20,21,22, // droite 
			20,22,23 // droite 
		};
	

		return Mesh(device, context, vertices, indices);

	}

};