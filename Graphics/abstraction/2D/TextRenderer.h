#pragma once

#include "Sprite.h"


class TextRenderer {

public:

	struct CharInstance
	{
		XMVECTOR position;
		XMVECTOR size;
		XMVECTOR uv;
	};

private:

	ID3D11Buffer* m_instancedBuffer = nullptr;
	ID3D11Buffer* m_vertexBuffer	= nullptr;
	ID3D11BlendState* m_blendState	= nullptr;

	d3d11_graphics::RenderingContext m_renderContext;

	size_t m_instanceCount;
	InputLayout m_layout;
	Effect m_spriteEffect;
	Texture m_font = Texture(L"res/textures/inv_font.dds");

	std::vector<CharInstance> m_allText;

public:

	void clear();
	void writeTextOnScreen(const std::string& text, int screenX, int screenY, float scale);
	void render() const;

private:
	void bind() const ;
	void resizeBuffers();
	std::vector<CharInstance> generateText(const std::string& text, int x, int y, float scale);

public:
	TextRenderer();
	~TextRenderer();
};