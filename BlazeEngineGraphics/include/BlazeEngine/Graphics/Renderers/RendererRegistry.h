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

		template<IsBaseOf<RendererBase> T>
		T* RegisterRenderer(RendererBase* renderer, StringView name)
		{
			return static_cast<T*>(RegisterRenderer(renderer, name));
		}
		template<IsBaseOf<RendererBase> T>
		T* RegisterRenderer(T* renderer)
		{
			return static_cast<T*>(RegisterRenderer(renderer, { }));
		}
		RendererBase* RegisterRenderer(RendererBase* renderer, StringView name);

		template<IsBaseOf<RendererBase> T>
		T* GetRenderer(const StringView& name) const
		{
			return GetRenderer(RendererBase::GetTypeID<T>(), name);
		}
		template<IsBaseOf<RendererBase> T>
		T* GetRenderer() const
		{
			return GetRenderer(RendererBase::GetTypeID<T>(), { });
		}
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
		Map<Key, RendererBase*> renderers;
	};
}