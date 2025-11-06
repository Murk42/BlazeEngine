#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/RendererRegistry.h"

namespace Blaze::Graphics
{
	RendererBase* RendererRegistry::RegisterRenderer(RendererBase* renderer, StringView name)
	{
		Key key{ renderer->GetTypeID(), name };
		auto it = renderers.Find(key);

		if (!it.IsNull())
		{
			RendererBase* old = it->value;

			if (renderer == nullptr)
				renderers.Erase(it);
			else
				it->value = renderer;

			return old;
		}
		else
		{
			renderers.Insert(key, renderer);
			return nullptr;
		}
	}
	RendererBase* RendererRegistry::GetRenderer(uint64 type, const StringView& name) const
	{
		Key key{ type, name };
		auto it = renderers.Find(key);

		if (!it.IsNull())
			return it->value;
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