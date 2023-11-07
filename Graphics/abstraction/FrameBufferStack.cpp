#include "FrameBufferStack.h"
#include "Framebuffer.h"


void FrameBufferStack::pushFBO(const FrameBuffer* fbo)
{
	m_stack.push(fbo);
	std::cout << "Pushed a new FBO of size " << fbo->getRenderTargetViews().size() << std::endl;
}

void FrameBufferStack::popFBO()
{
	m_stack.pop();

	if (m_stack.empty()) {
		FrameBuffer::unbind();
		return;
	}

	unsigned int width, height;
	width = m_stack.top()->getWidth();
	height = m_stack.top()->getHeight();

	m_stack.top()->bind(); // bind without pushing to the stack
	std::cout << "Poped a FBO "<< std::endl;

}

void FrameBufferStack::resetStack()
{
	while (!m_stack.empty())
		m_stack.pop();
}