#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Mesh.h"
#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/Graphics/Material.h"
#include "BlazeEngine/DataStructures/Transform2D.h"

#include <functional>

namespace Blaze
{
	struct ButtonData
	{
		TextureArray2D* texture;
		Recti textureCenterRect;
		float texScale;
	};

	enum class ButtonState
	{
		Down,
		Up,
		Hovered,
		Pressed,
		Released,		
	};

	enum class ButtonEvent
	{
		Down,    /* void Function(Button*) */
		Up,      /* void Function(Button*) */
		Pressed, /* void Function(Button*) */
		Released, /* void Function(Button*) */

		ButtonEvent_Count
	};

	class BLAZE_API Button
	{
		union EventFunction
		{
			std::function<void(Button*)> func;

			EventFunction() { }
			~EventFunction() { }
		};

		Vertex<Vec2f, float, float> vertex;		
		EventFunction eventFunctions[(uint)ButtonEvent::ButtonEvent_Count];

		Mesh mesh;
		ButtonState state;

		void SetEventFunction(ButtonEvent, void*);
	public:
		ButtonData* buttonData;
		Transform2D transform;						

		Button();		
		Button(ButtonData* buttonData);
		
		template<ButtonEvent E>
		void SetEventFunction(std::function<void(Button*)> function)
		{
			SetEventFunction(E, (void*)&function);
		}						
				
		ButtonState GetState() const { return state; }
		const Mesh& GetMesh() const { return mesh; }		

		void Update();		

		void Render(BaseMaterial& material, const Mat4f& mat);
	};	
}