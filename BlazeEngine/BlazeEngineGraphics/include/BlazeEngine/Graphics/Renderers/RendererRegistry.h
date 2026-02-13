#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

namespace Blaze::Graphics
{
	class RendererRegistry
	{
	public:
		RendererRegistry() = default;
		RendererRegistry(const RendererRegistry& other) = default;
		RendererRegistry(RendererRegistry&& other) noexcept = default;
		RendererRegistry(ArrayView<RendererBase&> renderers);
		template<IsDerivedFrom<RendererBase> ... R>
		RendererRegistry(R& ... renderers);

		template<IsDerivedFrom<RendererBase> T>
		void RegisterRenderer(T& renderer, StringView name = { });
		void RegisterRenderer(RendererBase& renderer, StringView name = { });

		template<IsDerivedFrom<RendererBase> T>
		T* GetRenderer(const StringView& name) const;
		template<IsDerivedFrom<RendererBase> T>
		T* GetRenderer() const;
		RendererBase* GetRenderer(uint64 type, const StringView& name) const;

		RendererRegistry& operator=(const RendererRegistry& other) = default;
		RendererRegistry& operator=(RendererRegistry&& other) noexcept = default;
	private:
		struct Key
		{
			uint64 type;
			String name;

			uint64 Hash() const;

			bool operator==(const Key& other) const;
			bool operator!=(const Key& other) const;
		};
		Map<Key, RendererBase&> renderers;
	};

	template<IsDerivedFrom<RendererBase> ... R>
	inline RendererRegistry::RendererRegistry(R& ... renderers)
		: RendererRegistry()
	{
		(RegisterRenderer(renderers), ...);
	}
	template<IsDerivedFrom<RendererBase> T>
	inline void RendererRegistry::RegisterRenderer(T& renderer, StringView name)
	{
		RegisterRenderer(static_cast<RendererBase&>(renderer), name);
	}
	template<IsDerivedFrom<RendererBase> T>
	inline T* RendererRegistry::GetRenderer(const StringView& name) const
	{
		return dynamic_cast<T*>(GetRenderer(RendererBase::GetTypeID<T>(), name));
	}
	template<IsDerivedFrom<RendererBase> T>
	inline T* RendererRegistry::GetRenderer() const
	{
		return dynamic_cast<T*>(GetRenderer(RendererBase::GetTypeID<T>(), {}));
	}
}