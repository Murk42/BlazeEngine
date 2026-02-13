#include "pch.h"
#include "BlazeEngine/UI/Graphics/GraphicsSubSystem.h"
#include "BlazeEngine/Core/Container/Set.h"
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Core/Container/Map.h"

#include "BlazeEngine/UI/Graphics/RenderNode.h"

namespace Blaze::UI
{
	using RenderItem = GraphicsSubSystem::RenderItem;
	using RenderGroup = GraphicsSubSystem::RenderGroup;

	class UISimpleRenderQueue
	{
		void _GetRenderOrder(UI::Node& node, Array<RenderItem>& arr)
		{
			if (auto* renderedNode = dynamic_cast<RenderNode*>(&node))
			{
				uint i = 0;
				while (auto unit = renderedNode->GetRenderUnit(i))
				{
					arr.AddBack(RenderItem{ .renderUnit = *unit, .node = node });
					++i;
				}
			}

			for (auto& child : node.GetChildren())
				_GetRenderOrder(child, arr);
		}
		Array<RenderItem> GetRenderOrder(UI::Screen& screen)
		{
			Array<RenderItem> arr;

			for (auto& node : screen.GetTree())
			{
				if (!node.IsEnabled())
					continue;

				if (auto* renderedNode = dynamic_cast<RenderNode*>(&node))
				{
					uint i = 0;
					while (auto unit = renderedNode->GetRenderUnit(i))
					{
						arr.AddBack(RenderItem{ .renderUnit = *unit, .node = node });
						++i;
					}
				}
			}

			//for (auto& child : graphicsSubSystem->screen->GetChildren())
			//	_GetRenderOrder(child, arr);

			return arr;
		}

	public:
		UISimpleRenderQueue()
		{

		}

		void CreateRenderQueue(GraphicsSubSystem& graphicsSubSystem, Array<RenderItem>& renderQueue, Array<RenderGroup>& renderGroups)
		{
			renderGroups.Clear();
			renderQueue = GetRenderOrder(*graphicsSubSystem.GetScreen());

			if (renderQueue.Empty())
				return;

			Graphics::RendererBase* renderer = nullptr;

			RenderGroup* group = nullptr;

			for (uintMem i = 0; i < renderQueue.Count(); ++i)
			{
				auto& renderItem = renderQueue[i];

				Graphics::RendererBase* previousRenderer = renderer;
				renderer = graphicsSubSystem.GetRendererRegistry().GetRenderer(renderItem.renderUnit.GetRequiredRendererTypeID(), renderItem.renderUnit.GetRequiredRendererName());

				if (renderer == nullptr)
				{
					auto name = renderItem.renderUnit.GetRequiredRendererName();

					if (name.Empty())
						BLAZE_LOG_WARNING("UI Render Pipeline couldn't find unnamed renderer");
					else
						BLAZE_LOG_WARNING("UI Render Pipeline couldn't find renderer named {}", name);

					renderQueue.EraseAt(i);
					--i;

					continue;
				}

				if (previousRenderer == renderer)
					++group->count;
				else
					group = &*renderGroups.AddBack(RenderGroup{ 1, *renderer });
			}
		}
	};
	class UIBestRenderQueue
	{
		struct UINodeCache
		{
			RenderNode& renderNode;
			UI::Node& node;
			Rectf boundingRect;
		};
		struct Node
		{
			Array<UINodeCache> uiNodes;
			Array<Node*> children;
		};
		struct NodeRenderPreGroup
		{
			Map<Graphics::RendererBase*, Array<RenderItem>> subGroups;
		};

		GraphicsSubSystem* graphicsSubSystem;

		bool RectIntersect(Rectf r1, Rectf r2)
		{
			return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
				r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
		}
		bool DoIntersect(Node* n1, Node* n2)
		{
			auto& uiNodes1 = n1->uiNodes;
			auto& uiNodes2 = n2->uiNodes;

			for (uintMem i = 0; i < uiNodes1.Count(); ++i)
				for (uintMem j = 0; j < uiNodes2.Count(); ++j)
					if (RectIntersect(uiNodes1[i].boundingRect, uiNodes2[j].boundingRect))
						return true;

			return false;
		}

		void _GetRenderOrder(UI::Node& node, Array<Node>& arr)
		{
			if (!node.IsEnabled())
				return;

			if (auto renderNode = dynamic_cast<RenderNode*>(&node))
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

				arr.AddBack(Node{
					.uiNodes = Array<UINodeCache>({ UINodeCache{.renderNode = *renderNode, .node = node, .boundingRect = Rectf(pMin, pMax - pMin) } }),
					.children = Array<Node*>(),
					});
			}

			for (auto& child : node.GetChildren())
				_GetRenderOrder(child, arr);
		}
		//Colapses the node tree into a array of renderable nodes by doing a depth first search
		Array<Node> GetRenderOrder(UI::Screen* screen)
		{
			Array<Node> arr;

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

		void PromoteNodes(Array<Node>& arr)
		{
			if (arr.Count() == 0)
				return;

			for (uintMem i = 1; i < arr.Count(); ++i)
				arr[i - 1].children.AddBack(&arr[i]);

			for (uintMem i = 1; i < arr.Count() - 1; ++i)
			{
				Node& node1 = arr[i];
				auto node2It = arr.FirstIterator() + i - static_cast<uintMem>(1);

				//if (node1.uiNodes.Empty())
				//	break;

				//I dont understand why this is here
				if (DoIntersect(&node1, node2It.Ptr()))
					continue;

				//j overflows to UINT_MAX
				for (; node2It != arr.AheadIterator(); --node2It)
				{
					Node& node2 = *node2It;

					//if (node2.uiNodes.Empty())
					//	continue;

					if (DoIntersect(&node1, &node2))
					{
						bool combined = false;
						for (uintMem k = 0; k < node2.children.Count(); ++k)
						{
							Node* child = node2.children[k];
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
		Array<NodeRenderPreGroup> CreatePreGroups(GraphicsSubSystem& graphicsSubSystem, const Array<Node>& renderOrder)
		{
			Array<NodeRenderPreGroup> preGroups;
			preGroups.ReserveExactly(renderOrder.Count());

			Set<RenderUnitBase*> usedRenderUnits;

			for (auto& el : renderOrder)
			{
				if (el.uiNodes.Count() == 0)
					continue;

				for (auto& node : el.uiNodes)
				{
					uint index = 0;
					while (auto renderUnit = node.renderNode.GetRenderUnit(index))
					{
						if (usedRenderUnits.Find(renderUnit).IsNull())
						{
							auto& preGroup = *preGroups.AddBack();
							usedRenderUnits.Insert(renderUnit);
							auto renderer = graphicsSubSystem.GetRendererRegistry().GetRenderer(renderUnit->GetRequiredRendererTypeID(), renderUnit->GetRequiredRendererName());
							auto& nodes = preGroup.subGroups.Insert(renderer).iterator->value;
							nodes.AddBack(RenderItem{ *renderUnit, node.node });
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
		void CreateGroupsAndQueue(const Array<NodeRenderPreGroup>& preGroups, Array<RenderGroup>& renderGroups, Array<RenderItem>& queue)
		{
			queue.Clear();
			renderGroups.ReserveExactly(preGroups.Count());

			for (auto& preGroup : preGroups)
			{
				for (auto& subGroup : preGroup.subGroups)
				{
					auto& renderGroup = *renderGroups.AddBack(RenderGroup{ subGroup.value.Count(), *subGroup.key });

					queue.Append(std::move(subGroup.value));
				}
			}
		}

	public:
		void CreateRenderQueue(GraphicsSubSystem& graphicsSubSystem, Array<RenderItem>& renderQueue, Array<RenderGroup>& renderGroups)
		{
			renderGroups.Clear();
			auto renderOrder = GetRenderOrder(graphicsSubSystem.GetScreen());

			PromoteNodes(renderOrder);

			auto preGroups = CreatePreGroups(graphicsSubSystem, renderOrder);

			CombinePreGroups(preGroups);

			CreateGroupsAndQueue(preGroups, renderGroups, renderQueue);
		}
	};

	GraphicsSubSystem::GraphicsSubSystem(Graphics::GraphicsContextBase& graphicsContext): 
		graphicsContext(graphicsContext), screen(nullptr), recreateRenderQueue(false)
	{
	}

	GraphicsSubSystem::GraphicsSubSystem(Graphics::GraphicsContextBase& graphicsContext, Graphics::RendererRegistry rendererRegistry) :
		graphicsContext(graphicsContext), rendererRegistry(rendererRegistry), screen(nullptr), recreateRenderQueue(false)
	{
	}
	GraphicsSubSystem::~GraphicsSubSystem()
	{
		if (screen != nullptr)
		{
			screen->treeChangedEventDispatcher.RemoveHandler<&GraphicsSubSystem::NodeTreeChanged>(*this);
			screen->destructionEventDispatcher.RemoveHandler<&GraphicsSubSystem::ScreenDestroyed>(*this);
		}
	}
	void GraphicsSubSystem::SetRendererRegistry(Graphics::RendererRegistry newRegistry)
	{
		rendererRegistry = std::move(newRegistry);
	}
	void GraphicsSubSystem::SetScreen(UI::Screen* newScreen)
	{
		if (screen != nullptr)
		{
			screen->treeChangedEventDispatcher.RemoveHandler<&GraphicsSubSystem::NodeTreeChanged>(*this);
			screen->destructionEventDispatcher.RemoveHandler<&GraphicsSubSystem::ScreenDestroyed>(*this);

			for (auto& node : screen->GetTree())
				node.enabledStateChangedEventDispatcher.RemoveHandler<&GraphicsSubSystem::ChildEnabledStateUpdated>(*this);
		}

		screen = newScreen;

		if (screen != nullptr)
		{
			for (auto& node : screen->GetTree())
				node.enabledStateChangedEventDispatcher.AddHandler<&GraphicsSubSystem::ChildEnabledStateUpdated>(*this);

			screen->treeChangedEventDispatcher.AddHandler<&GraphicsSubSystem::NodeTreeChanged>(*this);
			screen->destructionEventDispatcher.AddHandler<&GraphicsSubSystem::ScreenDestroyed>(*this);

			recreateRenderQueue = true;
		}
		else
		{
			renderQueue.Clear();
			renderGroups.Clear();
		}
	}
	void GraphicsSubSystem::Render(const Graphics::RenderContext& renderContext)
	{
		if (screen == nullptr)
			return;

		RenderContext RenderContext{ renderContext };

		for (auto& node : screen->GetTree())
			if (RenderNode* renderNode = dynamic_cast<RenderNode*>(&node))
				recreateRenderQueue |= renderNode->PreRender(RenderContext);

		if (recreateRenderQueue)
			RecreateRenderQueue();

		RenderItem* it = renderQueue.Ptr();
		for (auto& group : renderGroups)
		{
			group.renderer.StartRender(renderContext);

			for (uintMem i = 0; i < group.count; ++i)
				it[i].renderUnit.Render(it[i].node, group.renderer, RenderContext);

			group.renderer.EndRender(renderContext);

			it += group.count;
		}
	}
	void GraphicsSubSystem::RecreateRenderQueue()
	{
		UISimpleRenderQueue rq;

		rq.CreateRenderQueue(*this, renderQueue, renderGroups);

		recreateRenderQueue = false;
	}
	void GraphicsSubSystem::NodeTreeChanged(const Screen::ScreenTreeChangedEvent& event)
	{
		if (event.type == Screen::ScreenTreeChangedEvent::Type::NodeAdded)
			event.node.enabledStateChangedEventDispatcher.AddHandler<&GraphicsSubSystem::ChildEnabledStateUpdated>(*this);
		else if (event.type == Screen::ScreenTreeChangedEvent::Type::NodeRemoved)
			event.node.enabledStateChangedEventDispatcher.RemoveHandler<&GraphicsSubSystem::ChildEnabledStateUpdated>(*this);

		recreateRenderQueue = true;
	}
	void GraphicsSubSystem::ScreenDestroyed(const Screen::DestructionEvent& event)
	{
		SetScreen(nullptr);
	}
	void GraphicsSubSystem::ChildEnabledStateUpdated(const Node::NodeEnabledStateChangedEvent& event)
	{
		recreateRenderQueue = true;
	}
}