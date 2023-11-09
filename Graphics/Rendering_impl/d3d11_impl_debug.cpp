#include "direct3D11_impl.h"

#include "../abstraction/Camera.h"

void direct3D11_impl::renderAABB(Camera& cam, const AABB& box) {
	Vec o = box.origin;
	Vec x = { XMVectorGetX(box.size), 0, 0 };
	Vec y = { 0, XMVectorGetY(box.size), 0 };
	Vec z = { 0, 0, XMVectorGetZ(box.size) };

	renderDebugLine(cam, o, o + x);
	renderDebugLine(cam, o + y, o + x + y);
	renderDebugLine(cam, o + z, o + x + z);
	renderDebugLine(cam, o + y + z, o + x + y + z);
	renderDebugLine(cam, o, o + y);
	renderDebugLine(cam, o + x, o + y + x);
	renderDebugLine(cam, o + z, o + y + z);
	renderDebugLine(cam, o + x + z, o + y + x + z);
	renderDebugLine(cam, o, o + z);
	renderDebugLine(cam, o + x, o + z + x);
	renderDebugLine(cam, o + y, o + z + y);
	renderDebugLine(cam, o + x + y, o + z + x + y);
}

void direct3D11_impl::renderDebugLine(Camera& cam, Vec from, Vec to) {

	setBackbufferToDefault();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetInputLayout(debugLine.getVertexLayout());


	debugLine.updateSubresource(cam.getVPMatrix(), "worldParams");
	debugLine.sendCBufferToGPU("worldParams");

	debugLine.apply();


	VertexBuffer vbo({ Vertex{from}, Vertex{to} });
	vbo.bind();

	context->Draw(2, 0);


}

void direct3D11_impl::renderDebugPerspectiveCameraOutline(Camera& viewCamera, const Camera& outlinedCamera) {
	//const PerspectiveProjection& proj = outlinedCamera.getProjection<PerspectiveProjection>();
	//PerspectiveProjection proj = PerspectiveProjection();


	float zfar = 1000.F;
	float znear = 0.10F;
	float fovy = 3.1415f / 4.F;
	float aspect = 2.0f;

	Vec pos = outlinedCamera.getPosition();
	Vec I = outlinedCamera.getHorizontalDir();
	Vec J = outlinedCamera.getUp();
	Vec F = outlinedCamera.getForward();
	float dh = sin(fovy * .5f);
	float dw = dh * aspect;
	Vec U1 = F + dh * J + dw * I;
	Vec U2 = F - dh * J + dw * I;
	Vec U3 = F - dh * J - dw * I;
	Vec U4 = F + dh * J - dw * I;
	float zNear = -znear;
	float zFar = -zfar;

	renderDebugLine(viewCamera, pos, pos + F * zFar); // dir
	renderDebugLine(viewCamera, pos, pos + Vec{ 0,1,0,1 }); // world up
	renderDebugLine(viewCamera, pos, pos + I); // right
	renderDebugLine(viewCamera, pos, pos + J); // up
	renderDebugLine(viewCamera, pos, pos + U1 * zFar);
	renderDebugLine(viewCamera, pos, pos + U2 * zFar);
	renderDebugLine(viewCamera, pos, pos + U3 * zFar);
	renderDebugLine(viewCamera, pos, pos + U4 * zFar);
	renderDebugLine(viewCamera, pos + U1 * zFar, pos + U2 * zFar);
	renderDebugLine(viewCamera, pos + U2 * zFar, pos + U3 * zFar);
	renderDebugLine(viewCamera, pos + U3 * zFar, pos + U4 * zFar);
	renderDebugLine(viewCamera, pos + U4 * zFar, pos + U1 * zFar);

}

void direct3D11_impl::showImGuiDebugData() {
	ImGui::Begin("D3D11 Rendering debug data:");
	ImGui::Text("Current drawn triangle count : %d", triangleCount);
	ImGui::End();

	triangleCount = 0;

}