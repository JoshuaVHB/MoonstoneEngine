#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <memory>


#define CALL_IMPL(fnName)\
static auto fnName(auto&& ...args)\
{\
	return implementation->foo(std::forward<decltype(args)>(args)...);\
}
class Renderer 
{

public:

	struct _Impl 
	{
		friend class Renderer;
	
	protected:
		virtual int foo(int, int) = 0;
		virtual void bar() = 0;

	};



	CALL_IMPL(foo) ;
	CALL_IMPL(bar) ;

	

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

std::unique_ptr<Renderer::_Impl> Renderer::implementation = nullptr;

#include "Rendering_impl/direct3D11_impl.h"

