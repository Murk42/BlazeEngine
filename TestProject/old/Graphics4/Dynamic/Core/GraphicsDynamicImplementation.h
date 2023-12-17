#pragma once
#include "DynamicImplementation.h"
#include "GraphicsContext.h"

namespace Graphics4::Dynamic
{
	template<typename Impl, typename Base, typename GraphicsContext, typename ... Args>
	concept GraphicsImplementationOf =
		ImplementationOf<Impl, Base> &&
		ImplementationOf<GraphicsContext, GraphicsContextBase> &&
		std::constructible_from<Base, GraphicsContextBase&, Args...> &&
		std::constructible_from<Impl, GraphicsContext&, Args...>;

	/// <summary>
	/// Allows the derived classes of this class to dynamically change the implementation and try to get its underlyting implementation 
	/// with TryCast. It is possible to dynamically deduce the wanted implementation with a GraphicsContextBase object that is mapped
	/// with a implementation
	/// </summary>
	/// <typeparam name="Derived">The class that will have dynamic implementations</typeparam>
	/// <typeparam name="Allocator">The allocator used to allocate the underlying implementation</typeparam>
	template<typename Derived, typename ... Args> requires std::constructible_from<Derived, GraphicsContextBase&, Args...>
	class GraphicsDynamicImplementation : public DynamicImplementation<Derived>
	{
	public:
		GraphicsDynamicImplementation(GraphicsDynamicImplementation&& other) noexcept;
		GraphicsDynamicImplementation(GraphicsContextBase& graphicsContext, Args&& ... args);
		template<typename Impl> requires ImplementationOf<Impl, Derived> && std::constructible_from<Impl, Impl&&>
		GraphicsDynamicImplementation(Impl&& impl);		

		template<typename Impl, typename GraphicsContext> requires GraphicsImplementationOf<Impl, Derived, GraphicsContext, Args...>
		static void AddImplementation();

		GraphicsDynamicImplementation& operator=(GraphicsDynamicImplementation&& other) noexcept;
	protected:
		GraphicsDynamicImplementation();
	private:
		using TryCreateFunction = bool(*)(DynamicImplementation<Derived>*, GraphicsContextBase&, Args&&...);
		inline static Array<TryCreateFunction> funcs;		
	};

	template<typename Derived, typename ... Args> requires std::constructible_from<GraphicsContextBase, Args...>
	inline GraphicsDynamicImplementation<Derived, Args...>::GraphicsDynamicImplementation(GraphicsDynamicImplementation&& other) noexcept
		: DynamicImplementation<Derived>(std::move(other))
	{
	}
	template<typename Derived, typename ... Args> requires std::constructible_from<GraphicsContextBase, Args...>
	inline GraphicsDynamicImplementation<Derived, Args...>::GraphicsDynamicImplementation(GraphicsContextBase& graphicsContext, Args&& ... args)
	{
		for (auto& func : funcs)
			if (func(this, graphicsContext, std::forward<Args>(args)...))
				break;
	}
	template<typename Derived, typename ...Args> requires std::constructible_from<GraphicsContextBase, Args...>
	template<typename Impl> requires ImplementationOf<Impl, Derived> && std::constructible_from<Impl, Impl&&>
	inline GraphicsDynamicImplementation<Derived, Args...>::GraphicsDynamicImplementation(Impl&& impl)
	{
		SetImplementation<Impl, Impl&&>(std::move(impl));
	}
	template<typename Derived, typename ... Args> requires std::constructible_from<GraphicsContextBase, Args...>
	inline GraphicsDynamicImplementation<Derived, Args...>& GraphicsDynamicImplementation<Derived, Args...>::operator=(GraphicsDynamicImplementation&& other) noexcept
	{
		DynamicImplementation<Derived>::operator=(std::move(other));
		return *this;
	}
	template<typename Derived, typename ... Args> requires std::constructible_from<GraphicsContextBase, Args...>
	inline GraphicsDynamicImplementation<Derived, Args...>::GraphicsDynamicImplementation()
	{
	}
	
	template<typename Impl, typename GraphicsContext, typename Derived, typename ... Args>
	bool TrySetImplementation(DynamicImplementation<Derived>* derived, GraphicsContextBase& graphicsContext, Args&& ... args)
	{
		if (auto impl = graphicsContext.TryCast<GraphicsContext>())
		{
			derived->SetImplementation<Impl, GraphicsContext&, Args...>(*impl, std::forward<Args>(args)...);
			return true;
		}
		return false;
	}

	template<typename Derived, typename ... Args> requires std::constructible_from<GraphicsContextBase, Args...>
	template<typename Impl, typename GraphicsContext> requires GraphicsImplementationOf<Impl, Derived, GraphicsContext, Args...>
	inline void GraphicsDynamicImplementation<Derived, Args...>::AddImplementation()
	{					
		//bool(*func)(DynamicImplementation<Derived>*, GraphicsContextBase&, Args&&...);
		TryCreateFunction& func = *funcs.AddBack(nullptr);
		func = &Dynamic::TrySetImplementation<Impl, GraphicsContext, Derived, Args...>;		
	}	
}