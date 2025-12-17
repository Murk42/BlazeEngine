#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/RendererRegistry.h"

namespace Blaze::Graphics
{
	RendererRegistry::RendererRegistry(ArrayView<RendererBase&> renderers)
		: RendererRegistry()
	{
		for (auto& renderer : renderers)
			RegisterRenderer(renderer);
	}
	void RendererRegistry::RegisterRenderer(RendererBase& renderer, StringView name)
	{
		Key key{ renderer.GetTypeID(), name };
		auto it = renderers.Find(key);

		if (!it.IsNull())
			renderers.Erase(it);

		renderers.Insert(key, renderer);
	}
	RendererBase* RendererRegistry::GetRenderer(uint64 type, const StringView& name) const
	{
		Key key{ type, name };
		auto it = renderers.Find(key);

		if (!it.IsNull())
			return &it->value;
		else
			return nullptr;
	}
	uint64 RendererRegistry::Key::Hash() const
	{
		return name.Hash() ^ Blaze::Hash<uint64>().Compute(type);
	}
	bool RendererRegistry::Key::operator==(const Key& other) const
	{
		return type == other.type && name == other.name;
	}
	bool RendererRegistry::Key::operator!=(const Key& other) const
	{
		return !(*this == other);
	}
}