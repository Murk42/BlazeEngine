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

		initWindow = (SDL_Window*)CreateWindowHandle("", Vec2i(INT_MAX), Vec2i(640, 480), WindowHandleGraphicsaAPI::OpenGL);
		initWindowTaken = false;

		context = SDL_GL_CreateContext(initWindow);

		if (!glewInitialized)
		{
			glewInit();
			glewInitialized = true;
		}

	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(GraphicsContext_OpenGL&& other) noexcept
	{
		initWindow = other.initWindow;
		initWindowTaken = other.initWindowTaken;
		context = other.context;

		other.initWindow = NULL;
		other.initWindowTaken = true;
		other.context = NULL;
	}
	GraphicsContext_OpenGL::~GraphicsContext_OpenGL()
	{
		Clear();
	}

	void GraphicsContext_OpenGL::WaitForIdle() const
	{
		glFlush();
	}

	void GraphicsContext_OpenGL::Clear()
	{
		SDL_GL_DeleteContext(context);

		DestroyWindowHandle((WindowHandle)initWindow);
		

		initWindow = NULL;
		initWindowTaken = true;
		context = NULL;
	}

	SDL_Window* GraphicsContext_OpenGL::CreateWindow(String title, Vec2i size)
	{
		if (!initWindowTaken)
		{
			initWindowTaken = true;
			SDL_SetWindowTitle(initWindow, title.Ptr());
			SDL_SetWindowSize(initWindow, size.x, size.y);
			return initWindow;
		}
		
		return (SDL_Window*)CreateWindowHandle(title, Vec2i(INT_MAX), size, WindowHandleGraphicsaAPI::OpenGL);
	}

	void GraphicsContext_OpenGL::DestroyWindow(SDL_Window* window)
	{
		if (initWindow == window)
		{
			initWindowTaken = false;
			SDL_HideWindow(window);
		}
		else
		{
			DestroyWindowHandle((WindowHandle)window);
			SDL_GL_MakeCurrent(initWindow, context);
		}
	}

	GraphicsContext_OpenGL& GraphicsContext_OpenGL::operator=(GraphicsContext_OpenGL&& other) noexcept
	{
		Clear();
		
		initWindow = other.initWindow;
		initWindowTaken = other.initWindowTaken;
		context = other.context;
		
		other.initWindow = NULL;
		other.initWindowTaken = true;
		other.context = NULL;

		return *this;
	}

	Semaphore_OpenGL::Semaphore_OpenGL()		
	{
	}

	void Semaphore_OpenGL::Clear() {
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
		Framebuffer::operator=(std::move(other));
		renderWindow = other.renderWindow;
		other.renderWindow = nullptr;

		return *this;
	}	

	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL* graphicsContext, String title, Vec2i size)
		: graphicsContext(graphicsContext)
	{		
		framebuffer.renderWindow = this;
		framebuffer.id = 0;
		framebuffer.size = size;

		if (!graphicsContext)
			return;		

		window = graphicsContext->CreateWindow(title, size);
		engineWindow = new Window((WindowHandle)window);
		engineWindow->resizedEventDispatcher.AddHandler(*this);
		this->size = size;		

		SDL_ShowWindow(window);		

		std::atomic_flag_test_and_set(&aquirable);
	}

	RenderWindow_OpenGL::RenderWindow_OpenGL(RenderWindow_OpenGL&& other) noexcept		
		: EventHandler(std::move(other))
	{						
		if (std::atomic_flag_test(&other.aquirable))
			std::atomic_flag_test_and_set(&aquirable);

		graphicsContext = other.graphicsContext;
		window = other.window;
		engineWindow = other.engineWindow;
		framebuffer = std::move(other.framebuffer);
		size = other.size;
		framebuffer.renderWindow = this;

		other.graphicsContext = nullptr;
		other.window = nullptr;
		other.engineWindow = nullptr;
		other.size = Vec2i();
	}

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{				
		Clear();
	}

	void RenderWindow_OpenGL::Clear()
	{
		if (graphicsContext)
		{
			graphicsContext->DestroyWindow(window);
			delete engineWindow;
		}

		EventHandler::UnsubscribeFromDispatcher();		

		engineWindow = nullptr;
		graphicsContext = nullptr;
		window = nullptr;
		size = Vec2i();				
	}

	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AquireNextFramebuffer(Semaphore& semaphore)
	{
		std::atomic_flag_wait(&aquirable, false);
		std::atomic_flag_clear(&aquirable);
		return framebuffer;		
	}

	void RenderWindow_OpenGL::PresentRenderWindow(RenderContext&, RenderWindowFramebuffer& framebuffer, std::initializer_list<Semaphore*> semaphores)
	{
		if (framebuffer.renderWindow != this)
			Debug::Logger::LogFatal("Blaze graphics API", "Trying to present a framebuffer with a render window that it doesn't belong to");
		else
		{
			SDL_GL_SwapWindow(window);
			std::atomic_flag_test_and_set(&aquirable);
			std::atomic_flag_notify_all(&aquirable);
		}
	}

	RenderWindow_OpenGL& RenderWindow_OpenGL::operator=(RenderWindow_OpenGL&& other) noexcept
	{
		EventHandler::operator=(std::move(other));

		if (std::atomic_flag_test(&other.aquirable))
			std::atomic_flag_test_and_set(&aquirable);		

		graphicsContext = other.graphicsContext;
		window = other.window;
		engineWindow = other.engineWindow;
		framebuffer = std::move(other.framebuffer);
		size = other.size;

		other.graphicsContext = nullptr;
		other.engineWindow = nullptr;
		other.window = nullptr;
		other.size = Vec2i();

		return *this;
	}

	void RenderWindow_OpenGL::OnEvent(Input::Events::WindowResizedEvent event)
	{
		size = event.size;		
		framebuffer.size = size;
	}

	GraphicsContext_OpenGL CreateGraphicsContext(ImplementationInfo* implementationInfo)
	{
		if (implementationInfo->api != ImplementationAPI::OpenGL)
			Debug::Logger::LogWarning("Blaze graphics API", "Passing non OpenGL implementation info to a static OpenGL funtion.");
		return GraphicsContext_OpenGL();
	}
	Semaphore_OpenGL CreateSemaphore(GraphicsContext_OpenGL& graphicsContext)
	{
		return Semaphore_OpenGL();
	}
	RenderContext_OpenGL CreateGraphicsRenderContext(GraphicsContext_OpenGL& graphicsContext)
	{
		return RenderContext_OpenGL(&graphicsContext);
	}
	RenderContext_OpenGL CreatePresentRenderContext(GraphicsContext_OpenGL& graphicsContext)
	{
		return RenderContext_OpenGL(&graphicsContext);
	}	
	RenderWindow_OpenGL CreateRenderWindow(GraphicsContext_OpenGL& graphicsContext, String title, Vec2i size)
	{						
		return RenderWindow_OpenGL(&graphicsContext, title, size);
	}
	RenderWindowFramebuffer_OpenGL& AquireNextFramebuffer(RenderWindow_OpenGL& renderWindow, Semaphore_OpenGL& signalSemaphore)
	{				
		return renderWindow.AquireNextFramebuffer(signalSemaphore);
	}
	void PresentRenderWindow(RenderContext_OpenGL& renderContext, RenderWindow_OpenGL& renderWindow, RenderWindowFramebuffer_OpenGL& framebuffer, std::initializer_list<Semaphore_OpenGL*> waitSemaphores)
	{	
		renderWindow.PresentRenderWindow(renderContext, framebuffer, waitSemaphores);
	}	
}
