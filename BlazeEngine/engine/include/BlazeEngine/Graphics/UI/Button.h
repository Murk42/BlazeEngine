#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Mesh.h"
#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/DataStructures/Transform2D.h"

namespace Blaze
{
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
		Vertex<Vec2f, Vec4f, float, float> vertex;
		void* eventFunctions[(uint)ButtonEvent::ButtonEvent_Count];

		Mesh mesh;
		ButtonState state;
	public:
		Transform2D transform;				
		Recti textureCenterRect;

		Button();
		Button(Recti textureCenterRect);

		void SetEventFunction(ButtonEvent event, void* function);
				
		ButtonState GetState() const { return state; }
		const Mesh& GetMesh() const { return mesh; }		

		void Update();		
	};	
}