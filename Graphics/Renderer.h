#pragma once

#pragma warning( disable : 4244 ) 

#include <concepts>
#include <type_traits>
#include <tuple>
#include <memory>
#include <filesystem>



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
struct AABB;


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

		virtual void drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation) = 0;
		virtual Mesh loadMeshFromFile(const std::filesystem::path& path) = 0;
		virtual void renderMesh(Camera&, const Mesh&, const Effect&) = 0;
		virtual void renderCubemap(Camera&, const Mesh&, const Effect&) = 0;
		virtual void clearScreen(float, float, float, float) = 0;
		virtual void clearScreen() = 0;
		virtual void renderPBRMesh(Camera& camera, const Mesh& mesh, const Material& mat) = 0;
		virtual void renderAABB(Camera& camera, const AABB&) = 0;
		virtual void renderDebugPerspectiveCameraOutline(Camera& viewCamera, const Camera& outlinedCamera) = 0;
		virtual void showImGuiDebugData() = 0;

	private:


	};

	CALL_IMPL(drawIndexed) ;
	CALL_IMPL(clearScreen) ;
	CALL_IMPL(renderMesh) ;
	CALL_IMPL(renderCubemap) ;
	CALL_IMPL(loadMeshFromFile) ;
	CALL_IMPL(renderPBRMesh) ;
	CALL_IMPL(renderAABB) ;
	CALL_IMPL(renderDebugPerspectiveCameraOutline) ;
	CALL_IMPL(showImGuiDebugData) ;

	
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


