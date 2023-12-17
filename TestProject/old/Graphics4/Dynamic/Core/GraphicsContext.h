#pragma once
#include "DynamicImplementation.h"

namespace Graphics4::Dynamic
{
	class GraphicsContextBase : public DynamicImplementation<GraphicsContextBase>
	{
	public:
		GraphicsContextBase(const GraphicsContextBase&) = delete;
		GraphicsContextBase(GraphicsContextBase&& other) noexcept;
		GraphicsContextBase(StringView implementationName);
		virtual ~GraphicsContextBase();

		virtual void WaitForIdle() const;
		virtual String GetImplementationName() const;

		GraphicsContextBase& operator=(const GraphicsContextBase&) = delete;
		GraphicsContextBase& operator=(GraphicsContextBase&& other) noexcept;

		template<typename T> requires std::constructible_from<T>
		static bool AddImplementation(StringView name);
	protected:		
		GraphicsContextBase();
	private:
		template<typename T>
		static void SetImlementation(GraphicsContextBase* graphicsContextp);

		static bool AddImplementation(StringView name, void(*)(GraphicsContextBase*));
	};
	template<typename T> requires std::constructible_from<T>
	inline bool GraphicsContextBase::AddImplementation(StringView name)
	{
		return AddImplementation(name, SetImlementation<T>);
	}
	template<typename T>
	inline void GraphicsContextBase::SetImlementation(GraphicsContextBase* graphicsContext)
	{
		((DynamicImplementation<GraphicsContextBase>*)graphicsContext)->SetImplementation<T>();		
	}
}