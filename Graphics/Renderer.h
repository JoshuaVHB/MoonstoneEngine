#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <memory>


#define CALL_IMPL(fnName)\
static auto fnName(auto&& ...args)\
{\
	return implementation->##fnName(std::forward<decltype(args)>(args)...);\
}
class Renderer 
{

public:

	struct _Impl 
	{
		friend class Renderer;
	
	protected:



		virtual void clearScreen(float, float, float, float) = 0;
		virtual void clearScreen() = 0;

	};



	CALL_IMPL(clearScreen) ;

	

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


