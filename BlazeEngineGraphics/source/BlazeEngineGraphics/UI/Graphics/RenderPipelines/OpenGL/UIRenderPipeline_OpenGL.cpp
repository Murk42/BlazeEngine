#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderPipelines/OpenGL/UIRenderPipeline_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/ImageRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/PanelRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextInputRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextRenderStream_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	UIRenderPipeline_OpenGL::UIRenderPipeline_OpenGL(TexturedRectRenderer_OpenGL& texturedRectRenderer, PanelRenderer_OpenGL& panelRenderer, TextRenderer_OpenGL& textRenderer) :
		texturedRectRenderer(texturedRectRenderer),
		panelRenderer(panelRenderer),
		textRenderer(textRenderer), 
		screen(nullptr)
	{
	}

	UIRenderPipeline_OpenGL::~UIRenderPipeline_OpenGL()
	{
	}

	void UIRenderPipeline_OpenGL::SetScreen(UI::Screen* newScreen)
	{
		if (screen != nullptr)
		{
			screen->nodeCreatedEventDispatcher.RemoveHandler(*this);
			screen->nodeDestroyedEventDispatcher.RemoveHandler(*this);
			screen->screenDestructionEventDispatcher.RemoveHandler(*this);			
		}
		
		screen = newScreen;

		if (screen != nullptr)
		{
			screen->nodeCreatedEventDispatcher.AddHandler(*this);
			screen->nodeDestroyedEventDispatcher.AddHandler(*this);
			screen->screenDestructionEventDispatcher.AddHandler(*this);						

			recreateRenderQueue = true;
		}
		else
		{
			renderQueue.Clear();
			renderGroups.Clear();
		}
	}	

	class UIRenderPipelineRenderStream_OpenGL :
		public RenderStream
	{				
		struct NodeDataInterface
		{			
			static uintMem Count(UI::Node* node)
			{
				return node->GetChildren().Count();
			}
			static UI::Node* GetChild(UI::Node* node, uintMem index)
			{
				return node->GetChildren()[index];
			}
		};		
		
		bool finishedRenderer;
		bool finishedStream;

		uintMem groupIndex;
		uintMem groupElementIndex;
		uintMem elementIndex;

		UIRenderPipeline_OpenGL* renderPipeline;									

		void BeginStream() override
		{
			if (finishedStream)
				return;

			finishedRenderer = false;

			groupElementIndex = 0;

			renderPipeline->renderQueue[elementIndex]->renderStream->BeginStream();			

			while (!finishedRenderer && renderPipeline->renderQueue[elementIndex]->renderStream->IsEmpty())
				Advance();
		}
		void Advance() override
		{			
			if (finishedRenderer)
				return;

			renderPipeline->renderQueue[elementIndex]->renderStream->Advance();

			while (true)
			{
				if (!renderPipeline->renderQueue[elementIndex]->renderStream->IsEmpty())
					return;

				++elementIndex;
				++groupElementIndex;

				if (renderPipeline->renderGroups[groupIndex].count == groupElementIndex)
				{
					++groupIndex;

					finishedRenderer = true;
					if (groupIndex == renderPipeline->renderGroups.Count())
						finishedStream = true;

					return;
				}
				else
					renderPipeline->renderQueue[elementIndex]->renderStream->BeginStream();
			}
		}
		void* GetCurrent() override
		{	
			if (finishedRenderer)
				return nullptr;

			return renderPipeline->renderQueue[elementIndex]->renderStream->GetCurrent();
		}
		bool IsEmpty() override
		{			
			return finishedRenderer;
		}
	public:
		UIRenderPipelineRenderStream_OpenGL(UIRenderPipeline_OpenGL* renderPipeline)
			: renderPipeline(renderPipeline), groupIndex(0), elementIndex(0), groupElementIndex(0), finishedStream(false), finishedRenderer(true)
		{				
			if (renderPipeline->renderGroups.Count() == 0)
				finishedStream = true;			
		}		

		StreamRenderer* GetNextStreamRenderer() const
		{		
			if (finishedStream)
				return nullptr;

			return renderPipeline->renderGroups[groupIndex].renderer;			
		}
	};

	void UIRenderPipeline_OpenGL::Render(Vec2u targetSize)
	{
		if (screen == nullptr)
			return;				

		if (recreateRenderQueue)
			RecreateRenderQueue();

		UIRenderPipelineRenderStream_OpenGL stream{ this };
				
		while (auto streamRenderer = stream.GetNextStreamRenderer())		
			streamRenderer->Render(stream, targetSize);					
	}	

	StreamRenderer* UIRenderPipeline_OpenGL::GetRenderer(UI::Node* node)
	{		
		if (auto* text = dynamic_cast<TextRenderStream_OpenGL*>(node->renderStream))
		{
			return &texturedRectRenderer;
		}
		else if (auto* image = dynamic_cast<ImageRenderStream_OpenGL*>(node->renderStream))
		{
			return &texturedRectRenderer;
		}
		else if(auto* panel = dynamic_cast<PanelRenderStream_OpenGL*>(node->renderStream))
		{
			return &panelRenderer;
		}		
		else if (auto* textInput = dynamic_cast<TextInputRenderStream_OpenGL*>(node->renderStream))
		{
			return &texturedRectRenderer;
		}

		return nullptr;
	}	
	
	class UISimpleRenderQueue
	{				
		void _GetRenderOrder(UI::Node* node, Array<UI::Node*>& arr)
		{
			auto renderer = renderPipeline->GetRenderer(node);

			if (renderer != nullptr)
				arr.AddBack(node);

			for (auto child : node->GetChildren())
				_GetRenderOrder(child, arr);
		}
		Array<UI::Node*> GetRenderOrder()
		{
			Array<UI::Node*> arr;
			arr.ReserveExactly(renderPipeline->screen->GetNodeCount());

			for (auto child : renderPipeline->screen->GetChildren())
				_GetRenderOrder(child, arr);

			return arr;
		}

		UIRenderPipeline_OpenGL* renderPipeline;
	public: 
		UISimpleRenderQueue(UIRenderPipeline_OpenGL* renderPipeline)
			: renderPipeline(renderPipeline)
		{

		}

		void CreateRenderQueue()
		{	
			renderPipeline->renderGroups.Clear();
			renderPipeline->renderQueue = GetRenderOrder();

			if (renderPipeline->renderQueue.Empty())
				return;

			StreamRenderer* renderer = nullptr; //renderPipeline->GetRenderer(renderPipeline->renderQueue[0]);
			StreamRenderer* oldRenderer = nullptr;

			UIRenderPipeline_OpenGL::NodeRenderGroup* group = nullptr;
			//group->renderer = renderer;

			for (auto node : renderPipeline->renderQueue)
			{				
				oldRenderer = renderer;
				renderer = renderPipeline->GetRenderer(node);

				if (oldRenderer == renderer)
					++group->count;
				else
				{
					group = &*renderPipeline->renderGroups.AddBack();
					group->count = 1;
					group->renderer = renderer;
				}

			}			
		}
	};

	class UIBestRenderQueue
	{
		struct UINodeCache
		{
			UI::Node* node;
			Rectf boundingRect;
		};
		struct RenderNode
		{
			Array<UINodeCache> uiNodes;
			Array<RenderNode*> children;
		};
		struct NodeRenderPreGroup
		{
			Map<StreamRenderer*, Array<UI::Node*>> subGroups;
		};
		using NodeRenderGroup = UIRenderPipeline_OpenGL::NodeRenderGroup;

		UIRenderPipeline_OpenGL* renderPipeline;

		bool RectIntersect(Rectf r1, Rectf r2)
		{
			return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
				r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
		}
		bool DoIntersect(RenderNode* n1, RenderNode* n2)
		{			
			auto& uiNodes1 = n2->uiNodes;
			auto& uiNodes2 = n2->uiNodes;

			for (uintMem i = 0; i < uiNodes1.Count(); ++i)
				for (uintMem j = 0; j < uiNodes2.Count(); ++j)
					if (RectIntersect(uiNodes1[i].boundingRect, uiNodes2[j].boundingRect))
						return true;

			return false;
		}

		void _GetRenderOrder(UI::Node* node, Array<RenderNode>& arr)
		{
			auto renderer = renderPipeline->GetRenderer(node);

			Vec2f pos = node->GetFinalPosition();
			Vec2f size = node->GetFinalSize();
			float rot = node->GetFinalRotation();
			float cos = Math::Cos(rot);
			float sin = Math::Sin(rot);
			Vec2f p1 = pos;
			Vec2f p2 = pos + Vec2f(cos, sin) * size.x;
			Vec2f p3 = pos + Vec2f(-sin, cos) * size.y;
			Vec2f p4 = p2 + p3 - p1;
			Vec2f pMax = Vec2f(std::max({ p1.x, p2.x, p3.x, p4.x }), std::max({ p1.y, p2.y, p3.y, p4.y }));
			Vec2f pMin = Vec2f(std::min({ p1.x, p2.x, p3.x, p4.x }), std::min({ p1.y, p2.y, p3.y, p4.y }));

			if (renderer != nullptr)
				arr.AddBack(Array<UINodeCache>({ { node, Rectf(pMin, pMax - pMin) }}), Array<RenderNode*>());

			for (auto child : node->GetChildren())
				_GetRenderOrder(child, arr);
		}
		Array<RenderNode> GetRenderOrder(UI::Screen* screen)
		{
			Array<RenderNode> arr;
			arr.ReserveExactly(screen->GetNodeCount());

			for (auto child : screen->GetChildren())
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
				if (arr[i].uiNodes.Count() == 0)
					break;

				if (DoIntersect(&arr[i], &arr[i - 1]))
					continue;

				//j overlfows to UINT_MAX
				for (uintMem j = i - 1; j < arr.Count(); --j)
				{
					RenderNode* n1 = &arr[i];
					RenderNode* n2 = &arr[j];

					if (n2->uiNodes.Count() == 0)
						continue;

					if (DoIntersect(n1, n2))
					{
						bool combined = false;
						for (uintMem k = 0; k < n2->children.Count(); ++k)
						{
							RenderNode* child = n2->children[k];
							if (!DoIntersect(n1, child))
							{
								child->children.Append(std::move(n1->children));
								child->uiNodes.Append(std::move(n1->uiNodes));

								for (uintMem l = 0; l < arr[i + 1].children.Count(); ++l)
									if (arr[i + 1].children[l] == &arr[i])
										arr[i + 1].children.EraseAt(l);

								combined = true;
								break;
							}
						}

						if (!combined)
							n2->children.AddBack(n1);

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
		Array<NodeRenderPreGroup> CreatePreGroups(const Array<RenderNode>& renderOrder)
		{			
			Array<NodeRenderPreGroup> preGroups;
			preGroups.ReserveExactly(renderOrder.Count());

			for (auto& el : renderOrder)
			{
				if (el.uiNodes.Count() == 0)
					continue;

				auto& preGroup = *preGroups.AddBack();
				for (auto& node : el.uiNodes)
				{
					auto renderer = renderPipeline->GetRenderer(node.node);
					auto& nodes = preGroup.subGroups.Insert(renderer).iterator->value;
					nodes.AddBack(node.node);
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
		void CreateGroupsAndQueue(const Array<NodeRenderPreGroup>& preGroups, Array<NodeRenderGroup>& renderGroups, Array<UI::Node*>& queue)
		{			
			queue.Clear();
			renderGroups.ReserveExactly(preGroups.Count());

			for (auto& preGroup : preGroups)
			{
				for (auto& subGroup : preGroup.subGroups)
				{					
					auto& renderGroup = *renderGroups.AddBack();
					renderGroup.count = subGroup.value.Count();
					renderGroup.renderer = subGroup.key;

					queue.Append(std::move(subGroup.value));
				}
			}
		}
		
	public:
		UIBestRenderQueue(UIRenderPipeline_OpenGL* renderPipeline)
			: renderPipeline(renderPipeline)
		{
		}

		void CreateRenderQueue()
		{
			auto renderOrder = GetRenderOrder(renderPipeline->screen);

			PromoteNodes(renderOrder);

			auto preGroups = CreatePreGroups(renderOrder);			
			
			CombinePreGroups(preGroups);

			CreateGroupsAndQueue(preGroups, renderPipeline->renderGroups, renderPipeline->renderQueue);
		}
	};

	void UIRenderPipeline_OpenGL::RecreateRenderQueue()
	{
		UIBestRenderQueue rq{ this };

		rq.CreateRenderQueue();

		recreateRenderQueue = false;
	}

	void UIRenderPipeline_OpenGL::OnEvent(UI::NodeCreatedEvent event)
	{		
		recreateRenderQueue = true;		
	}

	void UIRenderPipeline_OpenGL::OnEvent(UI::NodeDestroyedEvent event)
	{	
		recreateRenderQueue = true;		
	}

	void UIRenderPipeline_OpenGL::OnEvent(UI::ScreenDestructionEvent event)
	{
		SetScreen(nullptr);
	}
}