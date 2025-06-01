#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderPipelines/OpenGL/UIRenderPipeline_OpenGL.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/DataStructures/Set.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::Graphics::OpenGL
{
	class GlobalRenderGroupStream :
		public RenderStream
	{
	public:
		GlobalRenderGroupStream(ArrayView<UIRenderItem> renderItems)
			: renderItem(renderItems.Ptr()), endRenderItem(renderItems.Ptr() + renderItems.Count())
		{			
		}

		void BeginStream() override
		{
			if (renderItem == endRenderItem)
				return;

			while (!renderItem->node.IsEnabled())
			{
				++renderItem;

				if (renderItem == endRenderItem)
					return;
			} 
			
			renderItem->renderUnit.BeginStream();
		}
		void* Advance() override
		{
			if (renderItem == endRenderItem)
				return nullptr;

			while (true)
			{
				auto rd = renderItem->renderUnit.Advance();

				if (rd != nullptr)
					return rd;				

				do
				{
					++renderItem;

					if (renderItem == endRenderItem)
						return nullptr;

				} while (!renderItem->node.IsEnabled());

				renderItem->renderUnit.BeginStream();
			}			
		}
	private:
		const UIRenderItem* renderItem;
		const UIRenderItem* endRenderItem;
	};
	class UISimpleRenderQueue
	{				
		void _GetRenderOrder(UI::Node& node, Array<UIRenderItem>& arr)
		{
			if (auto* renderedNode = dynamic_cast<RenderObject*>(&node))
			{
				uint i = 0;
				while (auto unit = renderedNode->GetRenderUnit(i))
				{				
					arr.AddBack(UIRenderItem{ .renderUnit = *unit, .node = node });
					++i;
				}
			}

			for (auto& child : node.GetChildren())
				_GetRenderOrder(child, arr);
		}
		Array<UIRenderItem> GetRenderOrder(UI::Screen& screen)
		{
			Array<UIRenderItem> arr;

			for (auto& node : screen.GetTree())
				if (auto* renderedNode = dynamic_cast<RenderObject*>(&node))
				{
					uint i = 0;
					while (auto unit = renderedNode->GetRenderUnit(i))
					{
						arr.AddBack(UIRenderItem{ .renderUnit = *unit, .node = node });
						++i;
					}
				}

			//for (auto& child : renderPipeline->screen->GetChildren())
			//	_GetRenderOrder(child, arr);

			return arr;
		}
		
	public: 
		UISimpleRenderQueue()			
		{

		}

		void CreateRenderQueue(UIRenderPipeline_OpenGL& renderPipeline, Array<UIRenderItem>& renderQueue, Array<UINodeRenderGroup>& renderGroups)
		{	
			renderGroups.Clear();
			renderQueue = GetRenderOrder(*renderPipeline.GetScreen());

			if (renderQueue.Empty())
				return;

			StreamRenderer* renderer = nullptr;
			StreamRenderer* oldRenderer = nullptr;

			UINodeRenderGroup* group = nullptr;			

			for (auto renderItem : renderQueue)
			{				
				oldRenderer = renderer;
				renderer = renderPipeline.GetRenderer(renderItem.renderUnit);				

				if (renderer == nullptr)
				{
					BLAZE_ENGINE_GRAPHICS_WARNING("UI Render Pipeline couldn't find renderer");
					continue;
				}

				if (oldRenderer == renderer)
					++group->count;
				else
					group = &*renderGroups.AddBack(UINodeRenderGroup{ 1, *renderer});									
			}			
		}
	};
	class UIBestRenderQueue
	{
		struct UINodeCache
		{
			RenderObject& renderObject;
			UI::Node& node;
			Rectf boundingRect;
		};
		struct RenderNode
		{
			Array<UINodeCache> uiNodes;
			Array<RenderNode*> children;
		};
		struct NodeRenderPreGroup
		{
			Map<StreamRenderer*, Array<UIRenderItem>> subGroups;
		};

		UIRenderPipeline_OpenGL* renderPipeline;

		bool RectIntersect(Rectf r1, Rectf r2)
		{
			return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
				r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
		}
		bool DoIntersect(RenderNode* n1, RenderNode* n2)
		{			
			auto& uiNodes1 = n1->uiNodes;
			auto& uiNodes2 = n2->uiNodes;

			for (uintMem i = 0; i < uiNodes1.Count(); ++i)
				for (uintMem j = 0; j < uiNodes2.Count(); ++j)
					if (RectIntersect(uiNodes1[i].boundingRect, uiNodes2[j].boundingRect))
						return true;

			return false;
		}

		void _GetRenderOrder(UI::Node& node, Array<RenderNode>& arr)
		{
			if (!node.IsEnabled())
				return;

			if (auto renderObject = dynamic_cast<RenderObject*>(&node))			
			{				
				auto finalTransform = node.GetFinalTransform();
				Vec2f pos = finalTransform.position;
				Vec2f size = finalTransform.size;
				float rot = finalTransform.rotation;
				float cos = Math::Cos(rot);
				float sin = Math::Sin(rot);
				Vec2f p1 = pos;
				Vec2f p2 = pos + Vec2f(cos, sin) * size.x;
				Vec2f p3 = pos + Vec2f(-sin, cos) * size.y;
				Vec2f p4 = p2 + p3 - p1;
				Vec2f pMax = Vec2f(std::max({ p1.x, p2.x, p3.x, p4.x }), std::max({ p1.y, p2.y, p3.y, p4.y }));
				Vec2f pMin = Vec2f(std::min({ p1.x, p2.x, p3.x, p4.x }), std::min({ p1.y, p2.y, p3.y, p4.y }));

				arr.AddBack(RenderNode{
					.uiNodes = Array<UINodeCache>({ UINodeCache{ .renderObject = *renderObject, .node = node, .boundingRect = Rectf(pMin, pMax - pMin) } }),
					.children = Array<RenderNode*>(),
					});
			}

			for (auto& child : node.GetChildren())
				_GetRenderOrder(child, arr);
		}
		//Colapses the node tree into a array of renderable nodes by doing a depth first search
		Array<RenderNode> GetRenderOrder(UI::Screen* screen)
		{
			Array<RenderNode> arr;			

			for (auto& child : screen->GetChildren())
				_GetRenderOrder(child, arr);

			return arr;
		}
		//void CombineChildren(RenderNode* node)
		//{		
		//	for (uintMem i = 0; i < node->children.Count(); ++i)
		//		for (uintMem j = i + 1; j < node->children.Count();)
		//		{
		//			RenderNode* n1 = node->children[i];
		//			RenderNode* n2 = node->children[j];
		//
		//			if (!DoIntersect(n1, n2))
		//			{
		//				n1->children.Append(std::move(n2->children));
		//				n1->uiNodes.Append(std::move(n2->uiNodes));
		//
		//				node->children.EraseAt(j);
		//			}
		//			else
		//				++j;
		//		}
		//
		//	for (auto child : node->children)
		//		CombineChildren(child);
		//}

		void PromoteNodes(Array<RenderNode>& arr)
		{
			if (arr.Count() == 0)
				return;

			for (uintMem i = 1; i < arr.Count(); ++i)
				arr[i - 1].children.AddBack(&arr[i]);

			for (uintMem i = 1; i < arr.Count() - 1; ++i)
			{
				RenderNode& node1 = arr[i];
				auto node2It = arr.FirstIterator() + i - 1;

				//if (node1.uiNodes.Empty())
				//	break;

				//I dont understand why this is here
				if (DoIntersect(&node1, node2It.Ptr()))
					continue;
				
				//j overflows to UINT_MAX
				for (; node2It != arr.AheadIterator(); --node2It)				
				{
					RenderNode& node2 = *node2It;

					//if (node2.uiNodes.Empty())
					//	continue;

					if (DoIntersect(&node1, &node2))
					{
						bool combined = false;
						for (uintMem k = 0; k < node2.children.Count(); ++k)
						{
							RenderNode* child = node2.children[k];
							if (!DoIntersect(&node1, child))
							{
								child->children.Append(std::move(node1.children));
								child->uiNodes.Append(std::move(node1.uiNodes));

								for (uintMem l = 0; l < arr[i + 1].children.Count(); ++l)
									if (arr[i + 1].children[l] == &arr[i])
										arr[i + 1].children.EraseAt(l);

								combined = true;
								break;
							}
						}

						if (!combined)
							node2.children.AddBack(&node1);

						break;
					}
				}
			}

			//for (auto it = arr.LastIterator(); it != arr.AheadIterator(); --it)
			//{			
			//	for (auto it2 = it - 1; it2 != arr.AheadIterator(); --it2)
			//		if (DoIntersect(&*it, &*it2))
			//		if (RectIntersect(it2->uiNodes[0].boundingRect, it->uiNodes[0].boundingRect))
			//		{
			//			it2->children.AddBack(&*it);
			//			break;
			//		}
			//}
		}
		Array<NodeRenderPreGroup> CreatePreGroups(UIRenderPipeline_OpenGL& renderPipeline, const Array<RenderNode>& renderOrder)
		{			
			Array<NodeRenderPreGroup> preGroups;
			preGroups.ReserveExactly(renderOrder.Count());

			Set<RenderUnit*> usedRenderUnits;			

			for (auto& el : renderOrder)
			{
				if (el.uiNodes.Count() == 0)
					continue;

				for (auto& node : el.uiNodes)
				{
					uint index = 0;
					while (auto renderUnit = node.renderObject.GetRenderUnit(index))
					{
						if (usedRenderUnits.Find(renderUnit).IsNull())
						{							
							auto& preGroup = *preGroups.AddBack();
							usedRenderUnits.Insert(renderUnit);
							auto renderer = renderPipeline.GetRenderer(*renderUnit);
							auto& nodes = preGroup.subGroups.Insert(renderer).iterator->value;
							nodes.AddBack(UIRenderItem{ *renderUnit, node.node });
						}						

						++index;
					}
				}
			}

			return preGroups;
		}
		void CombinePreGroups(Array<NodeRenderPreGroup>& preGroups)
		{			
			if (preGroups.Count() == 0)
				return;

			for (uint i = 0; i < preGroups.Count() - 1; ++i)
			{
				for (auto& pair : preGroups[i].subGroups)
				{
					auto it = preGroups[i + 1].subGroups.Find(pair.key);
					if (!it.IsNull())
					{
						pair.value.Append(std::move(it->value));						

						preGroups[i + 1].subGroups.Erase(it);

						if (preGroups[i + 1].subGroups.Count() == 0)
							preGroups.EraseAt(i + 1);

						break;
					}
				}
			}
		}
		void CreateGroupsAndQueue(const Array<NodeRenderPreGroup>& preGroups, Array<UINodeRenderGroup>& renderGroups, Array<UIRenderItem>& queue)
		{			
			queue.Clear();
			renderGroups.ReserveExactly(preGroups.Count());

			for (auto& preGroup : preGroups)
			{
				for (auto& subGroup : preGroup.subGroups)
				{					
					auto& renderGroup = *renderGroups.AddBack(UINodeRenderGroup{ subGroup.value.Count(), *subGroup.key });					

					queue.Append(std::move(subGroup.value));
				}
			}
		}
		
	public:
		void CreateRenderQueue(UIRenderPipeline_OpenGL& renderPipeline, Array<UIRenderItem>& renderQueue, Array<UINodeRenderGroup>& renderGroups)
		{
			renderGroups.Clear();
			auto renderOrder = GetRenderOrder(renderPipeline.GetScreen());

			PromoteNodes(renderOrder);

			auto preGroups = CreatePreGroups(renderPipeline, renderOrder);
			
			CombinePreGroups(preGroups);

			CreateGroupsAndQueue(preGroups, renderGroups, renderQueue);
		}
	};

	UIRenderPipeline_OpenGL::UIRenderPipeline_OpenGL(TexturedRectRenderer_OpenGL& texturedRectRenderer, TexturedRectRenderer_OpenGL& coloredCharacterRenderer_OpenGL, PanelRenderer_OpenGL& panelRenderer) :
		texturedRectRenderer(texturedRectRenderer),
		coloredCharacterRenderer_OpenGL(coloredCharacterRenderer_OpenGL),
		panelRenderer(panelRenderer),		
		screen(nullptr),
		recreateRenderQueue(false)		
	{
	}
	UIRenderPipeline_OpenGL::~UIRenderPipeline_OpenGL()
	{
		if (screen != nullptr)
		{
			screen->nodeTreeChangedEventDispatcher.RemoveHandler<&UIRenderPipeline_OpenGL::NodeTreeChanged>(*this);
			screen->screenDestructionEventDispatcher.RemoveHandler<&UIRenderPipeline_OpenGL::ScreenDestroyed>(*this);
		}
	}
	void UIRenderPipeline_OpenGL::SetScreen(UI::Screen* newScreen)
	{
		if (screen != nullptr)
		{						
			screen->nodeTreeChangedEventDispatcher.RemoveHandler<&UIRenderPipeline_OpenGL::NodeTreeChanged>(*this);
			screen->screenDestructionEventDispatcher.RemoveHandler<&UIRenderPipeline_OpenGL::ScreenDestroyed>(*this);
		}
		
		screen = newScreen;

		if (screen != nullptr)
		{
			screen->nodeTreeChangedEventDispatcher.AddHandler<&UIRenderPipeline_OpenGL::NodeTreeChanged>(*this);
			screen->screenDestructionEventDispatcher.AddHandler<&UIRenderPipeline_OpenGL::ScreenDestroyed>(*this);

			recreateRenderQueue = true;
		}
		else
		{
			renderQueue.Clear();
			renderGroups.Clear();
		}
	}	
	void UIRenderPipeline_OpenGL::Render(Vec2u targetSize)
	{ 
		if (screen == nullptr)
			return;				

		if (recreateRenderQueue)
			RecreateRenderQueue();
		 
		UIRenderItem* it = renderQueue.Ptr();
		for (auto& group : renderGroups) 
		{
			GlobalRenderGroupStream globalRenderGroupStream{ { it, group.count} };

			group.renderer.Render(globalRenderGroupStream, targetSize);

			it += group.count;
		}		
	}	
	StreamRenderer* UIRenderPipeline_OpenGL::GetRenderer(RenderUnit& renderUnit)
	{		
		if (renderUnit.GetRendererName() == "TexturedRectRenderer")
			return &texturedRectRenderer;
		else if (renderUnit.GetRendererName() == "PanelRenderer")
			return &panelRenderer;
		else if (renderUnit.GetRendererName() == "ColoredCharacterRenderer")
			return &coloredCharacterRenderer_OpenGL;

		return nullptr;
	}	
	void UIRenderPipeline_OpenGL::RecreateRenderQueue()
	{		
		UISimpleRenderQueue rq;

		rq.CreateRenderQueue(*this, renderQueue, renderGroups);

		recreateRenderQueue = false;
	}	
	void UIRenderPipeline_OpenGL::NodeTreeChanged(const UI::NodeTreeChangedEvent& event)
	{
		recreateRenderQueue = true;
	}
	void UIRenderPipeline_OpenGL::ScreenDestroyed(const UI::ScreenDestructionEvent& event)
	{
		SetScreen(nullptr);
	}
	void UIRenderPipeline_OpenGL::NodeEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event)
	{
		recreateRenderQueue = true;
	}
}