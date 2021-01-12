#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Mesh.h"
#include "BlazeEngine/Graphics/Texture.h"

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
		Vertex<Vec2f, Vec2f, Vec2f, float, float> vertex;
		void* eventFunctions[(uint)ButtonEvent::ButtonEvent_Count];

		Mesh mesh;
		ButtonState state;
	public:
		float depth;
		Rectu rect;		
		Vec2f cornerSize;
		TextureArray2D* texture;		

		Button();

		void SetEventFunction(ButtonEvent event, void* function);
				
		ButtonState GetState() const { return state; }
		const Mesh& GetMesh() const { return mesh; }		

		void Update();
		void UpdateMesh();
	};	
}