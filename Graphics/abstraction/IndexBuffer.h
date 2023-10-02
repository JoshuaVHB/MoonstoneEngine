#pragma once

#include <concepts>
#include <d3d11.h>
#include <vector>
#include <cstdint>

#include "../../Utils/Debug.h"

class IndexBuffer
{
private:

	// -- Store indices
	std::vector<uint16_t> m_indices;

	// -- Object creation
	ID3D11Device* m_device			= nullptr;
	ID3D11DeviceContext* m_context	= nullptr;

	// -- Buffer stuff

	ID3D11Buffer* m_ibo				= nullptr;
	D3D11_BUFFER_DESC m_descriptor{};
	D3D11_SUBRESOURCE_DATA m_initData{};

public:

	IndexBuffer() {}

	IndexBuffer(ID3D11Device* device,
		ID3D11DeviceContext* context,
		const std::vector<uint16_t>& indices) 
	{
		m_indices = indices;
		m_device = device;
		m_context = context;


		// -- Index buffer

		ZeroMemory(&m_descriptor, sizeof(m_descriptor));

		m_descriptor.Usage = D3D11_USAGE_IMMUTABLE;
		m_descriptor.ByteWidth = static_cast<UINT>(m_indices.size())*sizeof(uint16_t);
		m_descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_descriptor.CPUAccessFlags = 0;

		ZeroMemory(&m_initData, sizeof(m_initData));
		m_initData.pSysMem = &indices[0];

		DX_TRY(device->CreateBuffer(&m_descriptor, &m_initData, &m_ibo));

	}


	size_t getBufferSize() const { return m_indices.size(); }

	void bind() 
	{
		m_context->IASetIndexBuffer(m_ibo, DXGI_FORMAT_R16_UINT, 0);
	}




};