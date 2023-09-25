#pragma once

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include <cstdint>


using namespace DirectX;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT3 normal;
};

class Cube {


public:
	static UINT numElements; 
	//static D3D11_INPUT_ELEMENT_DESC layout[];

	Cube() {

	
	}

};