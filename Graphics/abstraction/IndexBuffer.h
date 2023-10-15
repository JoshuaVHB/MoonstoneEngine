#pragma once

#include <concepts>
#include <d3d11.h>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "../../Utils/Debug.h"
#include "../../Platform/WindowsEngine.h"

class IndexBuffer
{
private:

	// -- Store indices
	std::vector<uint16_t> m_indices;

	// -- Buffer stuff

#ifdef D3D11_IMPL
	d3d11_graphics::RenderingContext m_renderContext;
	ID3D11Buffer* m_ibo				= nullptr;
#endif

public:

	IndexBuffer() {}

	IndexBuffer(const std::vector<uint16_t>& indices)
	{
#ifdef D3D11_IMPL

		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
		m_indices = indices;

		D3D11_BUFFER_DESC m_descriptor{};
		D3D11_SUBRESOURCE_DATA m_initData{};

		// -- Index buffer

		ZeroMemory(&m_descriptor, sizeof(m_descriptor));

		m_descriptor.Usage = D3D11_USAGE_IMMUTABLE;
		m_descriptor.ByteWidth = static_cast<UINT>(m_indices.size()) * sizeof(uint16_t);
		m_descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_descriptor.CPUAccessFlags = 0;

		ZeroMemory(&m_initData, sizeof(m_initData));
		m_initData.pSysMem = &indices[0];

		DX_TRY(m_renderContext.device->CreateBuffer(&m_descriptor, &m_initData, &m_ibo));
#endif
	}

	void swap(IndexBuffer& other) {
		std::swap(other.m_ibo, m_ibo);
		std::swap(other.m_indices, m_indices);
		std::swap(other.m_renderContext, m_renderContext);
	}

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&& other) noexcept
		: m_renderContext(other.m_renderContext)
		, m_ibo(std::exchange(other.m_ibo, nullptr))
		, m_indices(std::exchange(other.m_indices, {}))
	{	}
	IndexBuffer& operator=(IndexBuffer&& other) noexcept
	{
		IndexBuffer{ std::move(other) }.swap(*this);
		return *this;
	}

	~IndexBuffer()
	{
		DX_RELEASE(m_ibo);
	}

	size_t getBufferSize() const { return m_indices.size(); }

	std::vector<uint16_t> getIndices() const noexcept {
		return m_indices;
	}


	void bind() const
	{
#ifdef D3D11_IMPL
		m_renderContext.context->IASetIndexBuffer(m_ibo, DXGI_FORMAT_R16_UINT, 0);
#endif	
	}




};