#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{
	namespace UI
	{
		Vec2f viewportSize;

		void UIManager::CleanElement(UIElement* element)
		{
			if (element->updateState == updateState)
				return;

			if (element->anchor != nullptr)
				CleanElement(element->anchor);

			Vec2f size;
			Vec2f offset;

			if (element->anchor != nullptr)
			{
				size = element->anchor->GetSize();
				offset = element->anchor->GetAlignedPos();
			}
			else
				size = viewportSize;

			switch (element->anchorAlignment)
			{
			case Blaze::Align::Top:			offset += Vec2f(size.x / 2, size.y); break;
			case Blaze::Align::TopRight:	offset += Vec2f(size.x, size.y); break;
			case Blaze::Align::Right:		offset += Vec2f(size.x, size.y / 2); break;
			case Blaze::Align::BottomRight:	offset += Vec2f(size.x, 0); break;
			case Blaze::Align::Bottom:		offset += Vec2f(size.x / 2, 0); break;
			case Blaze::Align::BottomLeft:	offset += Vec2f(0, 0); break;
			case Blaze::Align::Left:		offset += Vec2f(0, size.y / 2); break;
			case Blaze::Align::TopLeft:		offset += Vec2f(0, size.y); break;
			case Blaze::Align::Center:		offset += Vec2f(size.x / 2, size.y / 2); break;
			}

			size = element->rect.size;

			switch (element->localAlignment)
			{
			case Blaze::Align::Top:			offset -= Vec2f(size.x / 2, size.y); break;
			case Blaze::Align::TopRight:	offset -= Vec2f(size.x, size.y); break;
			case Blaze::Align::Right:		offset -= Vec2f(size.x, size.y / 2); break;
			case Blaze::Align::BottomRight:	offset -= Vec2f(size.x, 0); break;
			case Blaze::Align::Bottom:		offset -= Vec2f(size.x / 2, 0); break;
			case Blaze::Align::BottomLeft:	offset -= Vec2f(0, 0); break;
			case Blaze::Align::Left:		offset -= Vec2f(0, size.y / 2); break;
			case Blaze::Align::TopLeft:		offset -= Vec2f(0, size.y); break;
			case Blaze::Align::Center:		offset -= Vec2f(size.x / 2, size.y / 2); break;
			}

			element->alignedRect.pos = element->rect.pos + offset;
			element->alignedRect.pos.x = std::round(element->alignedRect.pos.x);
			element->alignedRect.pos.y = std::round(element->alignedRect.pos.y);
			element->alignedRect.size = element->rect.size;

			element->updateState = updateState;
		}

		UIManager::UIManager()
			: managers(nullptr), typeCount(0), updateState(0)
		{			
		}

		UIManager::~UIManager()
		{
			delete[] managers;
		}

		void UIManager::SetElementTypeRegistry(const UIElementTypeRegistry& registry)
		{			
			this->registry = registry;
			typeCount = registry.GetElementTypeCount();
			managers = new UIBaseElementManager*[typeCount];
			std::fill_n(managers, typeCount, nullptr);			
		}

		uint UIManager::CreateLayer()
		{
			uint*& counts = layers.emplace_back();
			counts = new uint[typeCount];
			std::fill_n(counts, typeCount, 0);
			return layers.size() - 1;
		}		

		Result UIManager::SetElementManager(UIBaseElementManager* manager, uint typeIndex)
		{
			if (typeIndex >= typeCount)
				return Result(Log(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Index out of bounds. Index value: " + String::Convert(typeIndex)));

			managers[typeIndex] = manager;
			manager->manager = this;

			return Result();
		}

		Result UIManager::AddElement(UIElement* ptr, uint typeIndex, uint layer)
		{
			layers[layer][typeIndex]++;
			managers[typeIndex]->AddElement(ptr);			

			return Result();
		}

		Result UIManager::SetupElementManagers()
		{
			for (uint i = 0; i < typeCount; ++i)
				if (managers[i] != nullptr)
					managers[i]->Setup();

			return Result();
		}
		Result UIManager::CleanupElementManagers()
		{			
			for (uint i = 0; i < typeCount; ++i)
				if (managers[i] != nullptr)
					managers[i]->Cleanup();

			return Result();
		}

		Result UIManager::Render()
		{
			Vec2f viewportSize = (Vec2f)Renderer::GetViewportSize();
			Mat4f vp2d = Math::OrthographicMatrix<float>(0, viewportSize.x, 0, viewportSize.y, -1000, 1000);			

			std::vector<uint> counts;
			counts.resize(typeCount, 0);
			
			for (auto& layer : layers)
			{
				for (uint i = 0; i < typeCount; ++i)
				{
					if (managers[i] != nullptr)
					{
						size_t nextCount = counts[i] + layer[i];
						managers[i]->Render(counts[i], nextCount);

						counts[i] = nextCount;
					}
					else if (layer[i] != 0)
						Logger::AddLog(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
							"Blaze Engine", "Trying to render elements without setting the ElementManager");
				}
			}			

			return Result();
		}
		Result UIManager::Update()
		{										
			Vec2i mp = Input::GetMousePos();
			Blaze::UI::viewportSize = (Vec2f)Renderer::GetViewportSize();

			++updateState;
			blocked = false;			

			for (uint i = 0; i < typeCount; ++i)
				if (managers[i] != nullptr)
					for (int j = 0; j != managers[i]->GetElementCount(); ++j)
					{
						UIElement* element = managers[i]->GetElementBase(j);
						
						CleanElement(element);

						Rectf rect = element->alignedRect;

						if (!blocked && 
							mp.x >= rect.pos.x && mp.x < rect.pos.x + rect.size.x &&
							mp.y >= rect.pos.y && mp.y < rect.pos.y + rect.size.y && 
							element->clickable)
							blocked = true;						
					}

			for (uint i = 0; i < typeCount; ++i)
			{	
				if (managers[i] != nullptr)
				{
					size_t count = managers[i]->GetElementCount();					

					managers[i]->Update(0, count);						
				}									
			}			
							
			return Result();
		}		

		bool UIManager::TakeFocus()
		{
			if (focusTaken)
				return false;
			focusTaken = true;
			return true;
		}

		void UIManager::ReturnFocus()
		{
			focusTaken = false;
		}
	}
}