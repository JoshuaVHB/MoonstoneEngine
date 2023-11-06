#include "direct3D11_impl.h"

#include "../abstraction/Texture.h"

void direct3D11_impl::renderMesh(Camera& camera, const Mesh& mesh) {

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(baseMesh.getVertexLayout());


	worldParams sp;

	sp.viewProj = XMMatrixTranspose(camera.getVPMatrix());
	sp.lightPos = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
	sp.cameraPos = camera.getPosition();
	sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
	sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.ambiantMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.diffuseMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	baseMesh.updateSubresource(sp, "worldParams");
	baseMesh.sendCBufferToGPU("worldParams");

	baseMesh.updateSubresource(XMMatrixTranspose(mesh.getTransform().getTransformationMatrix()), "meshParams"); // TODO make this more flexible
	baseMesh.sendCBufferToGPU("meshParams");

	baseMesh.apply();
	mesh.draw();
	baseMesh.unbindResources();

	triangleCount += mesh.getTriangleCount();

}

void direct3D11_impl::renderMesh(Camera& camera, const Mesh& mesh, const Effect& effect) {

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(effect.getVertexLayout());

	effect.updateSubresource(XMMatrixTranspose(mesh.getTransform().getTransformationMatrix()), "meshParams"); // TODO make this more flexible
	effect.sendCBufferToGPU("meshParams");

	effect.apply();
	mesh.draw();
	effect.unbindResources();

	triangleCount += mesh.getTriangleCount();

}



void direct3D11_impl::renderPBRMesh(Camera& camera, const Mesh& mesh, const Material& mat) {

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(pbrMeshEffect.getVertexLayout());

	worldParams sp;

	sp.viewProj = XMMatrixTranspose(camera.getVPMatrix());
	sp.lightPos = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
	sp.cameraPos = camera.getPosition();
	sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
	sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.ambiantMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.diffuseMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMeshEffect.updateSubresource(sp, "worldParams");
	pbrMeshEffect.sendCBufferToGPU("worldParams");

	pbrMeshEffect.updateSubresource(XMMatrixTranspose(mesh.getTransform().getTransformationMatrix()), "meshParams"); // TODO make this more flexible
	pbrMeshEffect.sendCBufferToGPU("meshParams");


	//pbrMeshEffect.updateSubresource(mesh.getMaterials()[0].getCoefficients(), "materialParams");
	//pbrMeshEffect.sendCBufferToGPU("materialParams");


	pbrMeshEffect.bindTexture("albedo", mat.queryTexture<TextureType::ALBEDO>());


	pbrMeshEffect.apply();
	mesh.draw();
}

// todo make this more clear
void direct3D11_impl::renderCubemap(Camera& camera, const Mesh& mesh, const Effect& effect) {


	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(effect.getVertexLayout());
	effect.apply();
	mesh.draw();


}

void direct3D11_impl::blitTexture(const Texture& tex, const DirectX::XMVECTOR& colorShift){

	//Renderer::setBackbufferToDefault();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	blit.bindTexture("tex", tex);
	blit.setUniformVector("colorShift", colorShift);
	blit.apply();
	Renderer::draw(6);
	blit.unbindResources();

}

void direct3D11_impl::writeTextOnScreen(const std::string& text, int screenX, int screenY, float scale)
{
	m_textRenderer.writeTextOnScreen(text, screenX, screenY, scale);
}

void direct3D11_impl::renderText()
{
	m_textRenderer.render();
}