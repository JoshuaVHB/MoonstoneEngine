#include "../Renderer.h"

struct direct3D11_impl : public Renderer::_Impl {

private:

	virtual int foo(int a, int b) override final { return a + b; }
	virtual void bar() override final { return; }

};