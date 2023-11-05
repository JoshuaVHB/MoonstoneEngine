#pragma once

#pragma warning( disable : 4244 ) 

#include <concepts>
#include <type_traits>
#include <memory>
#include <filesystem>
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>



// Creates static function for the renderer 
// that calls the implementation function of the same name
#define CALL_IMPL(fnName)\
static auto fnName(auto&& ...args)\
{\
	return implementation->##fnName(std::forward<decltype(args)>(args)...);\
}

// Forward declars


class Mesh;
class Effect;
class Camera;
class Material;
class Texture;
struct AABB;
struct ID3D11DepthStencilView;

/// Renderer class :
/// 
/// It simply is an interface. It has a pointer to some implementation. All methods are static and call the 
/// method of the same name of the implementation.
/// 
/// Add new pure virtual methods in the _Impl struct, and don't forget to add CALL_IMPL in the Renderer class.
/// Set an implementation in main.cpp, such as d3d11_impl before calling Renderer:: functions
 
class Renderer 
{

public:

	struct _Impl 
	{
		friend class Renderer;
	public:

	
	protected:

		// -- Core

		virtual void drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation) = 0;
		virtual void draw(size_t count) = 0;
		virtual void clearScreen(float, float, float, float) = 0;
		virtual void clearScreen() = 0;
		virtual void clearText() = 0;
		virtual void setBackbufferToDefault() = 0;
		virtual void setDepthBuffer(ID3D11DepthStencilView* other) = 0;

		// -- Render

		virtual void renderMesh(Camera&, const Mesh&, const Effect&) = 0;
		virtual void renderMesh(Camera&, const Mesh&) = 0;
		virtual void renderCubemap(Camera&, const Mesh&, const Effect&) = 0;
		virtual void renderPBRMesh(Camera& camera, const Mesh& mesh, const Material& mat) = 0;
		virtual void blitTexture(const Texture&, const DirectX::XMVECTOR& = { 1,1,1,1 }) = 0;
		virtual void writeTextOnScreen(const std::string& text, int screenX, int screenY, float scale) = 0;
		virtual void renderText() = 0;

		// -- Debug

		virtual void renderAABB(Camera& camera, const AABB&) = 0;
		virtual void renderDebugPerspectiveCameraOutline(Camera& viewCamera, const Camera& outlinedCamera) = 0;
		virtual void renderDebugLine(Camera& cam, DirectX::XMVECTOR from, DirectX::XMVECTOR to) = 0;
		virtual void showImGuiDebugData() = 0;



	};
	// size_t count
	CALL_IMPL(draw);
	// size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation
	CALL_IMPL(drawIndexed) ;
	CALL_IMPL(blitTexture) ;
	CALL_IMPL(clearScreen) ;
	CALL_IMPL(renderMesh) ;
	CALL_IMPL(renderCubemap) ;
	CALL_IMPL(renderPBRMesh) ;
	CALL_IMPL(renderAABB) ;
	CALL_IMPL(renderDebugPerspectiveCameraOutline)	;

	CALL_IMPL(showImGuiDebugData)					;
	CALL_IMPL(setBackbufferToDefault)				;

	CALL_IMPL(clearText)						;
	CALL_IMPL(renderText)						;

	// const std::string& text,
	// int screenX, int screenY
	// float scale
	CALL_IMPL(writeTextOnScreen)						;

	CALL_IMPL(setDepthBuffer)						;
	CALL_IMPL(renderDebugLine)						;

	
	/////////////////////////////////////////////////////////////////////////////
	template<typename _RenderingImplementation>
		requires (std::derived_from<_RenderingImplementation, _Impl>)
	static void setImplementation() 
	{
		implementation = std::make_unique<_RenderingImplementation>();
	}


private:

	static std::unique_ptr<_Impl> implementation;

};


