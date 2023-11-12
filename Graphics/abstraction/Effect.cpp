#include "Effect.h"
#include "../../Platform/WindowsEngine.h"

#include <d3dcompiler.h>
#include <optional>

#include "Texture.h"

Effect::Effect() 
	: m_effect(nullptr)
	, m_pass(nullptr)
	, m_technique(nullptr)
	, pSampleState(nullptr)
{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
#endif
}


	void Effect::loadEffectFromFile(const fs::path& pathToEffect) {


		ID3DBlob* blob = nullptr;


		ID3DBlob* compilationErrorMessage;
		HRESULT hr = D3DCompileFromFile(pathToEffect.c_str(),
			0, 0, 0, "fx_5_0", 0, 0, &blob, &compilationErrorMessage);

		if (hr != S_OK) {
			const void* errorBuffer = compilationErrorMessage->GetBufferPointer();
			const char* errorMessage = static_cast<const char*>(errorBuffer);
			throw std::runtime_error(errorMessage);
		}

		D3DX11CreateEffectFromMemory(
			blob->GetBufferPointer(), blob->GetBufferSize(), 0, m_renderContext.device, &m_effect);

		blob->Release();

		m_technique = m_effect->GetTechniqueByIndex(0);
		m_pass = m_technique->GetPassByIndex(0);


		createSampler();


	}

	void Effect::bindInputLayout(InputLayout layout) {

		// Créer l’organisation des sommets pour le VS de notre effet 
		D3DX11_PASS_SHADER_DESC passDesc;
		D3DX11_EFFECT_SHADER_DESC effectDesc;

		m_pass->GetVertexShaderDesc(&passDesc);

		passDesc.pShaderVariable->GetShaderDesc(passDesc.ShaderIndex, &effectDesc);
		const void* vsCodePtr = effectDesc.pBytecode;
		unsigned vsCodeLen = effectDesc.BytecodeLength;

		m_vertexLayout = NULL;
		DX_TRY_CODE(
			m_renderContext.device->CreateInputLayout(layout.asInputDesc(),
				static_cast<UINT>(layout.elems.size()), vsCodePtr, static_cast<SIZE_T>(vsCodeLen), &m_vertexLayout), 13);

	}

	void Effect::createSampler()
	{
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Création de l’état de sampling
		m_renderContext.device->CreateSamplerState(&samplerDesc, &pSampleState);
	}

	// Returns the slot number of the newly created cbuffer
	void Effect::addNewCBuffer(const std::string& name, uint32_t structSize)
	{
		ID3D11Buffer* constBuffer;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = structSize;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		m_renderContext.device->CreateBuffer(&bd, nullptr, &constBuffer);
		m_constantBuffers[name] = constBuffer;


	}

	void Effect::apply() const {

		m_pass->Apply(0, m_renderContext.context);

		// TODO MAKE SAMPLERS CORRECTLY  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = m_effect->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

	}

	void Effect::bindTexture(const std::string& uniformName, const ID3D11ShaderResourceView* tex) const
	{
		if (!tex) return;
		m_effect->GetVariableByName(uniformName.c_str())->AsShaderResource()->SetResource(
			(ID3D11ShaderResourceView*)(tex));
	}



	void Effect::bindTextureArray(const std::string& uniformName, const std::vector<ID3D11ShaderResourceView*>& tex) const
	{
		if (tex.empty()) return;
		m_effect->GetVariableByName(uniformName.c_str())->AsShaderResource()->SetResourceArray((ID3D11ShaderResourceView**)(tex.data()),0, static_cast<uint32_t>(tex.size()));
	}

	void Effect::bindTexture(const std::string& uniformName, const Texture& tex) const
	{
		if (!tex.getTexture()) return;
		m_effect->GetVariableByName(uniformName.c_str())->AsShaderResource()->SetResource(tex.getTexture());
	}

	void Effect::setUniformVector(const std::string& uniformName,const DirectX::XMVECTOR& value) const
	{
		m_effect->GetVariableByName(uniformName.c_str())->AsVector()->SetFloatVector(value.vector4_f32);
	}


	void Effect::sendCBufferToGPU(const std::string& cbuffName) const
	{
		ID3DX11EffectConstantBuffer* pCB = m_effect->GetConstantBufferByName(cbuffName.c_str());
		pCB->SetConstantBuffer(m_constantBuffers.at(cbuffName));
		DX_RELEASE(pCB);
	}
