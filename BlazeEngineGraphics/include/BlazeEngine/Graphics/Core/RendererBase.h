#pragma once
#include <atomic>
#include <typeinfo>
#include "BlazeEngine/Graphics/Core/GraphicsContext.h"
#include "BlazeEngine/Graphics/Core/RenderContext.h"

namespace Blaze::Graphics
{
	using RendererTypeID = uint64;

#pragma warning(disable : 4250) //Disable "inherits via dominance" warning

	class BLAZE_API RendererBase
	{
	public:
		virtual ~RendererBase() { }
		virtual void StartRender(const RenderContext& context) = 0;
		virtual void EndRender() = 0;

		virtual RendererTypeID GetTypeID() const = 0;
		virtual GraphicsContext& GetGraphicsContext() const = 0;

		template<typename T>
		__declspec(noinline) static RendererTypeID GetTypeIDFor()
		{			
			volatile auto file = __FILE__;
			volatile auto line = __LINE__;
			volatile auto name = typeid(T).name();
			return (uint64)&GetTypeIDFor<T>;
		}
	private:
		static std::atomic_uint64_t typeIDCounter;
	};
}