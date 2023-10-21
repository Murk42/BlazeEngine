#include "pch.h"
#include "API_OpenGL.h"

namespace Graphics3::OpenGL
{	
	bool glewInitialized = false;

	static constexpr uint openGLMajorVersion = 4;
	static constexpr uint openGLMinorVersion = 5;
	static constexpr uint openGLProfile = 0; //0 - Core, 1 - Compatibility

	static constexpr uint depthBufferBitCount = 16;

	//Setting this to 0 results in a opengl error !
	static constexpr uint stencilBufferBitCount = 8;

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()		
		: currentWindowHandle(nullptr)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
			SDL_GL_CONTEXT_DEBUG_FLAG |
			SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG
		);
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);		

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openGLMajorVersion);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openGLMinorVersion);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBufferBitCount);
		
		initWindow = WindowSDL("", Vec2i(INT_MAX), Vec2i(640, 480), WindowHandleGraphicsaAPI::OpenGL);							

		context = SDL_GL_CreateContext((SDL_Window*)initWindow.GetHandle());

		if (!glewInitialized)
		{
			glewInit();
			glewInitialized = true;
		}

	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(ImplementationInfo*)
		: GraphicsContext_OpenGL()
	{
	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(GraphicsContext_OpenGL&& other) noexcept
	{
		initWindow = std::move(other.initWindow);
		context = other.context;
		
		other.context = NULL;
	}
	GraphicsContext_OpenGL::~GraphicsContext_OpenGL()
	{
		Destroy();
	}

	void GraphicsContext_OpenGL::Destroy()
	{
		SDL_GL_DeleteContext(context);

		initWindow.Destroy();
		context = NULL;
	}

	String GraphicsContext_OpenGL::GetImplementationName() const
	{
		return "OpenGL";
	}

	void GraphicsContext_OpenGL::WaitForIdle() const
	{
		glFlush();
	}	

	WindowSDL GraphicsContext_OpenGL::CreateWindow(String title, Vec2i pos, Vec2i size)
	{		
		if (initWindow.IsNullWindow())		
			return WindowSDL(title, pos, size, WindowHandleGraphicsaAPI::OpenGL);		
		else		
			return std::move(initWindow);							
	}

	void GraphicsContext_OpenGL::DestroyWindow(WindowSDL& window)
	{
		WindowSDL::WindowSDLHandle currentWindowHandle = SDL_GL_GetCurrentWindow();

		if (currentWindowHandle == window.GetHandle() && initWindow.IsNullWindow())
			initWindow = std::move(window);
		else
			window.Destroy();
	}

	GraphicsContext_OpenGL& GraphicsContext_OpenGL::operator=(GraphicsContext_OpenGL&& other) noexcept
	{
		Destroy();
		
		initWindow = std::move(other.initWindow);		
		context = other.context;
				
		other.context = NULL;

		return *this;
	}
	
	Semaphore_OpenGL::Semaphore_OpenGL(OpenGL::GraphicsContext_OpenGL& graphicsContext)
		: Dynamic::SemaphoreBase()
	{
	}


	RenderContext_OpenGL::RenderContext_OpenGL(GraphicsContext_OpenGL* graphicsContext)
		: graphicsContext(graphicsContext)
	{
	}

	RenderContext_OpenGL::RenderContext_OpenGL(RenderContext_OpenGL&& other) noexcept
		: graphicsContext(other.graphicsContext)
	{
		other.graphicsContext = nullptr;
	}

	RenderContext_OpenGL::~RenderContext_OpenGL()
	{
	}

	void RenderContext_OpenGL::WaitForIdle() const
	{
		glFlush();
	}

	void RenderContext_OpenGL::Clear()
	{
		graphicsContext = nullptr;
	}

	RenderContext_OpenGL& RenderContext_OpenGL::operator=(RenderContext_OpenGL&& other) noexcept
	{
		graphicsContext = other.graphicsContext;		
		other.graphicsContext = nullptr;		

		return *this;
	}

	Framebuffer_OpenGL::Framebuffer_OpenGL()
		: id(-1)
	{
	}
	Framebuffer_OpenGL::Framebuffer_OpenGL(Framebuffer_OpenGL&& other) noexcept
		: id(other.id), size(other.size)
	{
		other.id = -1;
		other.size = Vec2i();
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{
		Clear();
	}
	void Framebuffer_OpenGL::Clear()
	{
		if (id != -1)
			glDeleteFramebuffers(1, &id);

		id = -1;
		size = Vec2i();
	}

	Framebuffer_OpenGL& Framebuffer_OpenGL::operator=(Framebuffer_OpenGL&& other) noexcept
	{
		Clear();

		id = other.id;
		size = other.size;

		other.id = -1;
		other.size = Vec2i();

		return *this;		
	}

	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL()
		: renderWindow(nullptr)
	{
	}
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL(RenderWindowFramebuffer_OpenGL&& other) noexcept
		: Framebuffer_OpenGL(std::move(other))
	{
		renderWindow = other.renderWindow;

		other.renderWindow = nullptr;		
	}
	RenderWindowFramebuffer_OpenGL::~RenderWindowFramebuffer_OpenGL()
	{
		Clear();
	}
	void RenderWindowFramebuffer_OpenGL::Clear()
	{
		renderWindow = nullptr;
		id = -1;
		size = Vec2i();
	}
	Vec2i RenderWindowFramebuffer_OpenGL::GetSize() const
	{
		return renderWindow->GetSize();
	}
	Dynamic::RenderWindowBase* RenderWindowFramebuffer_OpenGL::GetRenderWindow() const
	{
		return renderWindow;
	}
	RenderWindowFramebuffer_OpenGL::operator Dynamic::FramebufferBase& ()
	{
		return (Framebuffer_OpenGL&)*this;
	}
	RenderWindowFramebuffer_OpenGL& RenderWindowFramebuffer_OpenGL::operator=(RenderWindowFramebuffer_OpenGL&& other) noexcept
	{
		Framebuffer_OpenGL::operator=(std::move(other));
		renderWindow = other.renderWindow;
		other.renderWindow = nullptr;

		return *this;
	}	

	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, StringView title, Vec2i pos, Vec2i size)
		: graphicsContext(&graphicsContext), size(size)
	{					
		window = graphicsContext.CreateWindow(title, pos, size);		
		window.resizedEventDispatcher.AddHandler(*this);		

		std::atomic_flag_test_and_set(&acquirable);

		framebuffer.renderWindow = this;
		framebuffer.id = 0;
		framebuffer.size = size;
	}

	RenderWindow_OpenGL::RenderWindow_OpenGL(RenderWindow_OpenGL&& other) noexcept
		: EventHandler(std::move(other)), window(std::move(other.window)), framebuffer(std::move(other.framebuffer))
	{						
		size = other.size;
		graphicsContext = other.graphicsContext;				

		other.graphicsContext = nullptr;				
		other.size = Vec2i();

		if (std::atomic_flag_test(&other.acquirable))
			std::atomic_flag_test_and_set(&acquirable);

		framebuffer.renderWindow = this;
		framebuffer.id = 0;
		framebuffer.size = size;
	}

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{				
		Destroy();
	}

	void RenderWindow_OpenGL::Destroy()
	{
		if (graphicsContext)		
			graphicsContext->DestroyWindow(window);					

		EventHandler::UnsubscribeFromDispatcher();		
		
		graphicsContext = nullptr;		
		size = Vec2i();				
	}

	Dynamic::RenderWindowFramebufferBase& RenderWindow_OpenGL::AcquireNextFramebuffer(Dynamic::SemaphoreBase& semaphore)
	{
		return AcquireNextFramebuffer((Semaphore_OpenGL&)semaphore);
	}

	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AcquireNextFramebuffer(Semaphore_OpenGL& semaphore)
	{
		//acquirable.wait(false);		
		//acquirable.clear();
		return framebuffer;		
	}

	//void RenderWindow_PresentRenderWindow(RenderContext&, RenderWindowFramebuffer& framebuffer, std::initializer_list<Semaphore*> semaphores)
	//{
	//	if (framebuffer.renderWindow != this)
	//		Debug::Logger::LogFatal("Blaze graphics API", "Trying to present a framebuffer with a render window that it doesn't belong to");
	//	else
	//	{
	//		SDL_GL_SwapWindow((SDL_Window*)window.GetHandle());
	//		std::atomic_flag_test_and_set(&acquirable);
	//		std::atomic_flag_notify_all(&acquirable);
	//	}
	//}

	void RenderWindow_OpenGL::SwapWindowFramebuffers()
	{
		SDL_GL_SwapWindow((SDL_Window*)window.GetHandle());
		//acquirable.test_and_set();
		//acquirable.notify_all();		
	}

	RenderWindow_OpenGL& RenderWindow_OpenGL::operator=(RenderWindow_OpenGL&& other) noexcept
	{
		EventHandler::operator=(std::move(other));

		if (std::atomic_flag_test(&other.acquirable))
			std::atomic_flag_test_and_set(&acquirable);		

		window = std::move(other.window);		
		graphicsContext = other.graphicsContext;
		size = other.size;
		framebuffer = std::move(other.framebuffer);

		other.graphicsContext = nullptr;				
		other.size = Vec2i();

		return *this;
	}

	void RenderWindow_OpenGL::OnEvent(Input::Events::WindowResizedEvent event)
	{
		size = event.size;		
		framebuffer.size = size;
	}	
}
