#include "direct3D11_impl.h"

direct3D11_impl::direct3D11_impl()
{
	WindowsEngine& engine = WindowsEngine::getInstance();
	d3d11_graphics& gfx = engine.getGraphics();
	device = gfx.getDevice();
	context = gfx.getImmediateContext();
	swapChain = gfx.getSwapChain();
	rtv = gfx.getRenderTargetView();
	depth = gfx.getDepthBuffer().getView();

	pbrMeshEffect.loadEffectFromFile("res/effects/pbrMesh.fx");
	baseMesh.loadEffectFromFile("res/effects/baseMesh.fx");
	blit.loadEffectFromFile("res/effects/blit.fx");


	InputLayout layout, debuglayout;
	layout.pushBack<3>(InputLayout::Semantic::Position);
	layout.pushBack<3>(InputLayout::Semantic::Normal);
	layout.pushBack<2>(InputLayout::Semantic::Texcoord);
	pbrMeshEffect.bindInputLayout(layout);
	baseMesh.bindInputLayout(layout);

	pbrMeshEffect.addNewCBuffer("meshParams", sizeof(XMMATRIX));
	pbrMeshEffect.addNewCBuffer("worldParams", sizeof(worldParams));
	pbrMeshEffect.addNewCBuffer("materialParams", sizeof(MaterialCoefs));

	baseMesh.addNewCBuffer("worldParams", sizeof(worldParams));
	baseMesh.addNewCBuffer("meshParams", sizeof(XMMATRIX));

	debugLine.loadEffectFromFile("res/effects/debugLine.fx");
	debuglayout.pushBack<3>(InputLayout::Semantic::Position);
	debugLine.bindInputLayout(debuglayout);
	debugLine.addNewCBuffer("worldParams", sizeof(XMMATRIX));


}



 void direct3D11_impl::clearScreen()   {

	d3d11_graphics& gfx = WindowsEngine::getInstance().getGraphics();
	gfx.clearDepth();
	gfx.clearFramebuffer();
	m_textRenderer.clear();

}

 void direct3D11_impl::setBackbufferToDefault()   {

	context->OMSetRenderTargets(1, &rtv, depth);
}

 void direct3D11_impl::setDepthBuffer(ID3D11DepthStencilView* other)   {

	depth = other;
}


 void direct3D11_impl::clearScreen(float r, float g, float b, float a)   {
	const FLOAT rgba[4] = { r,g,b,a };
	context->ClearRenderTargetView(rtv, rgba);
	clearScreen();
}

 void direct3D11_impl::drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation)  {

	context->DrawIndexed(static_cast<UINT>(count), startIndexLocation, baseVertexLocation);
}

 void direct3D11_impl::draw(size_t count)  {

	context->Draw(static_cast<UINT>(count), 0);
}

 void direct3D11_impl::clearText()
{
	 m_textRenderer.clear();
}