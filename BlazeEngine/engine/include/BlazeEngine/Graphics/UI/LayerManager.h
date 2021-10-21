#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/Input/Event.h"

#include <vector>

namespace Blaze::UI
{
	class Layer;

	class BLAZE_API LayerManager
	{
		Mat4f proj;
		Vec2i viewportSize;
	public:
		std::vector<Layer*> layers;

		void AddLayer(Layer&);

		void SetupLayers();
		void UpdateLayers();
		void RenderLayers();

		void MouseMotionEvent(InputEvent::MouseMotion);
		void MousePressedEvent(InputEvent::MousePressed);
		void MouseReleasedEvent(InputEvent::MouseReleased);
		void KeyPressedEvent(InputEvent::KeyPressed);
		void TextInputEvent(InputEvent::TextInput);

		void SetViewportSize(Vec2i size);
		inline Vec2i GetViewportSize() const { return viewportSize; }

		friend class Layer;
	};
}