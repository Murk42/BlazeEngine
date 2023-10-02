#include "BlazeEngine/Application/UI/UIScene.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Application/UI/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI/Components/Bases/UISizeOverrideComponent.h"
#include "BlazeEngine/Application/UI/Components/Bases/UIPosOverrideComponent.h"

#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Math/Shapes.h"
#include "BlazeEngine/Input/Input.h"

#include "Components/UIElmenetCreationData.h"


namespace Blaze::UI
{
	Vec2f WeightedAverage(const std::array<Vec2f, 4>& points, Vec2f pivot)
	{
		return (points[3] - points[2]) * pivot.x + (points[0] - points[2]) * pivot.y;
	}
	Vec2f RotatePointAround(Vec2f pos, float angle, Vec2f pivot)
	{
		float sin = Math::Sin(angle);
		float cos = Math::Cos(angle);

		pos -= pivot;

		pos = Vec2f(
			pos.x * cos + pos.y * -sin,
			pos.x * sin + pos.y * cos
		);

		pos += pivot;

		return pos;
	}

	UIScene::UIScene()
		: ecsScene(nullptr), focusedElement(nullptr), blockingElement(nullptr), updateState(0)
	{
	}

	UIScene::~UIScene()
	{
		Clear();
	}

	Result UIScene::SetECSScene(ECS::Scene* scene)
	{

		if (ecsScene != nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to set a ECS scene to a UI scene but it already has a ECS scene set");

		ecsScene = scene;

		auto& registry = ecsScene->GetRegistry();

		if (!registry.HasComponentTypeData<Components::UITransformComponent>())
			return BLAZE_ERROR_RESULT("Blaze Engine", "The ECS scene that you are trying to set to a UI scene does not have UITransformComponent component registered");

		if (!registry.HasComponentTypeData<Components::UIElementComponent>())
			return BLAZE_ERROR_RESULT("Blaze Engine", "The ECS scene that you are trying to set to a UI scene does not have UIElementComponent component registered");

		ecsScene = scene;
		return Result();
	}
	ECS::Scene* UIScene::GetECSScene() const
	{
		return ecsScene;
	}

	const ECS::ComponentTypeRegistry& UIScene::GetRegistry() const
	{
		return ecsScene->GetRegistry();
	}

	Result UIScene::CreateLayer(StringView name)
	{
		//auto previousLayerIt = layers.LastIterator();
		//auto layerIt = layers.AddBack();		

		auto [it, inserted] = layerNames.Insert(name, { });

		if (!inserted)		
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");		

		auto layerIt = layers.AddBack();
		it->value = layerIt;

		layerIt->name = name;

		return Result();
	}
	Result UIScene::CreateLayerAfter(StringView name, StringView relative)
	{		
		auto relativeMapIt = layerNames.Find(relative);

		if (relativeMapIt.IsNull())
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a layer relative to a non existing layer");

		auto [it, inserted] = layerNames.Insert(name, { });

		if (!inserted)		
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");

		auto layerIt = layers.AddAfter(relativeMapIt->value);
		
		it->value = layerIt;
		layerIt->name = name;		

		return Result();
	}
	Result UIScene::CreateLayerBefore(StringView name, StringView relative)
	{
		auto relativeMapIt = layerNames.Find(relative);

		if (relativeMapIt.IsNull())
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a layer relative to a non existing layer");

		auto [it, inserted] = layerNames.Insert(name, { });

		if (!inserted)
			return BLAZE_WARNING_RESULT("BlazeEngine", "Trying to create a already existing layer");

		auto layerIt = layers.AddBefore(relativeMapIt->value);

		it->value = layerIt;
		layerIt->name = name;

		return Result();
	}
	bool UIScene::HasLayer(StringView layer)
	{
		return layerNames.Contains(layer);
	}

	ECS::Entity* UIScene::Create(StringView name, StringView layerName, ArrayView<const ECS::ComponentTypeData*> componentsTypeData)
	{
		Array<const ECS::ComponentTypeData*> typesData{ componentsTypeData.Count() + 2 };
		ecsScene->GetRegistry().GetComponentTypeData<Components::UITransformComponent>(typesData[0]);
		ecsScene->GetRegistry().GetComponentTypeData<Components::UIElementComponent>(typesData[1]);

		for (uint i = 0; i < componentsTypeData.Count(); ++i)
			typesData[i + 2] = componentsTypeData[i];		

		PrepareElementCreation(name, layerName);

		ECS::Entity* entity = ecsScene->Create(typesData);
		auto element = (Components::UIElementComponent*)entity->GetComponent(1);

		FinishElementCreation(element);

		return entity;
	}
	Result UIScene::Destroy(Components::UIElementComponent* element)
	{
		if (!element->name.Empty())
			nameToElementMap.Erase(element->name);		

		element->layerIt->elements.Erase(element->layerElementIt);

		CHECK_RESULT(ecsScene->Destroy(element->GetEntity()));

		return Result();
	}
	ECS::Entity* UIScene::GetElement(StringView name) const
	{
		auto it = nameToElementMap.Find(name);
		if (it.IsNull())
			return nullptr;
		return it->value->GetEntity();
	}

	Result UIScene::MoveToFirstElementInLayer(Components::UIElementComponent* element)
	{
		auto& elements = element->layerIt->elements;
		elements.MoveFront(element->layerElementIt);
		return Result();
	}
	Result UIScene::MoveToLastElementInLayer(Components::UIElementComponent* element)
	{
		auto& elements = element->layerIt->elements;
		elements.MoveBack(element->layerElementIt);
		return Result();
	}
	Result UIScene::MoveAfterElementInLayer(Components::UIElementComponent* element, Components::UIElementComponent* relative)
	{
		if (element->layerIt != relative->layerIt)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to move a element inside a layer relative to a element that isn't in the same layer");

		auto& elements = element->layerIt->elements;
		auto relativeIt = relative->layerElementIt;		

		elements.MoveAfter(element->layerElementIt, relativeIt);

		return Result();
	}
	Result UIScene::MoveBeforeElementInLayer(Components::UIElementComponent* element, Components::UIElementComponent* relative)
	{
		if (element->layerIt != relative->layerIt)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to move a element inside a layer relative to a element that isn't in the same layer");

		auto& elements = element->layerIt->elements;
		auto relativeIt = relative->layerElementIt;

		elements.MoveBefore(element->layerElementIt, relativeIt);

		return Result();
	}
	Result UIScene::MoveOneAfterInLayer(Components::UIElementComponent* element)
	{
		auto& elements = element->layerIt->elements;		
		elements.MoveAfter(element->layerElementIt, element->layerElementIt);

		return Result();
	}
	Result UIScene::MoveOneBeforeInLayer(Components::UIElementComponent* element)
	{
		auto& elements = element->layerIt->elements;
		elements.MoveBefore(element->layerElementIt, element->layerElementIt);

		return Result();
	}

	Result UIScene::RenderScene()
	{
		for (auto& layer : layers)
			for (auto el : layer.elements)
				for (auto& component : el.element->uiComponents)
				{
					auto system = (UISystem*)component->GetSystem();
					system->Render(this, component);
				}


		return Result();
	}
	Result UIScene::UpdateScene()
	{
		Result result;

		updateState++;

		for (auto& layer : layers)
			for (auto el : layer.elements)
				result += ResolveSize(el.transform);

		updateState++;

		for (auto& layer : layers)
			for (auto el : layer.elements)
				result += ResolvePosition(el.transform);

		return result;
	}
	Result UIScene::RenderDebugData()
	{
		//auto& lineRenderer = Graphics::Core::GetDefaultLine2DRenderer();
		//auto& textRenderer = Graphics::Core::GetDefaultTextRenderer();
		//
		//bool hover = false;
		//Layer::Element hovered;
		//for (auto& layer : layers)
		//	for (Layer::Element& el : layer.elements)
		//	{
		//		//Vec2f pos = element->transform->GetFinalPos();
		//		Vec2f size = el.transform->GetFinalSize();
		//		//Mat2f matrix = element->transform->GetMatrix();
		//		Mat4f matrix = el.transform->GetMatrix();
		//		Vec2f p1 = (matrix * Vec4f(0, 0, 0, 1)).xy();
		//		Vec2f p2 = (matrix * Vec4f(0, size.y, 0, 1)).xy();
		//		Vec2f p3 = (matrix * Vec4f(size.x, 0, 0, 1)).xy();
		//		Vec2f p4 = (matrix * Vec4f(size.x, size.y, 0, 1)).xy();
		//
		//		Vec2f pivot = (matrix * Vec4f(el.transform->pivot * size, 0, 1)).xy();
		//
		//		Vec2f pivot1 = pivot + Vec2f(3, 3);
		//		Vec2f pivot2 = pivot - Vec2f(3, 3);
		//		Vec2f pivot3 = pivot + Vec2f(-3, 3);
		//		Vec2f pivot4 = pivot - Vec2f(-3, 3);
		//
		//		Vec2f centerPos = (matrix * Vec4f(size / 2, 0, 1)).xy();
		//
		//		Math::Shapes::Quad2Df quad{ { p1, p2, p4, p3} };
		//
		//		Graphics::Shapes::DrawShape(quad, 0xffffffff, 1);
		//
		//		Graphics::Shapes::DrawLine2D(pivot1, pivot2, 0xff0000ff, 1);
		//		Graphics::Shapes::DrawLine2D(pivot3, pivot4, 0xff0000ff, 1);
		//
		//		if (quad.IsInside((Vec2f)Input::GetMousePos()))
		//		{
		//			hovered = el;
		//			hover = true;
		//		}
		//	}
		//
		//if (hover)
		//{
		//	auto el = hovered;
		//	Vec2f size = el.transform->GetFinalSize();
		//	Mat4f matrix = el.transform->GetMatrix();
		//	Vec2f p1 = (matrix * Vec4f(0, 0, 0, 1)).xy();
		//	Vec2f p2 = (matrix * Vec4f(0, size.y, 0, 1)).xy();
		//	Vec2f p3 = (matrix * Vec4f(size.x, 0, 0, 1)).xy();
		//	Vec2f p4 = (matrix * Vec4f(size.x, size.y, 0, 1)).xy();
		//	Vec2f centerPos = (matrix * Vec4f(size / 2, 0, 1)).xy();
		//
		//	Math::Shapes::Quad2Df quad{ { p1, p2, p4, p3} };
		//
		//	Vec2f textPos;
		//	Vec2f textLineEnd = textPos - Vec2f(3, 3);
		//	Vec2f textLineStart = centerPos;
		//
		//	{
		//		auto ray = Math::Shapes::Ray2Df(centerPos, Vec2f(1, -1));
		//
		//		std::array<Vec2f, 2> points;
		//		uint count = 0;
		//
		//		if (Math::Shapes::Intersection(quad, ray, points, count))
		//		{
		//			textLineStart = points[count - 1];
		//			textLineEnd = textLineStart + Vec2f(5, -5);
		//			textPos = textLineEnd + Vec2f(5, -5);
		//		}
		//		else
		//		{
		//			textLineStart = ray.point;
		//			textLineEnd = ray.PointOnRay(5);
		//			textPos = textLineEnd + Vec2f(5, -5);
		//		}
		//	}
		//
		//	float fontSize = 12;
		//	ColorRGBAf textColor[] = { 0xffffffff };
		//	StringUTF8 text;
		//	text += StringView(el.element->name.Size() == 0 ? "Unnamed" : el.element->name) + "\n";
		//	text += " pos: " + StringParsing::Convert(el.transform->pos.x) + ", " + StringParsing::Convert(el.transform->pos.y) + "\n";
		//	text += "      " + StringParsing::Convert(el.transform->GetFinalPos().x) + ", " + StringParsing::Convert(el.transform->GetFinalPos().y) + "\n";
		//	text += "size: " + StringParsing::Convert(el.transform->size.x) + ", " + StringParsing::Convert(el.transform->size.y) + "\n";
		//	text += "      " + StringParsing::Convert(el.transform->GetFinalSize().x) + ", " + StringParsing::Convert(el.transform->GetFinalSize().y) + "\n";
		//
		//
		//	Graphics::DefaultTextVertexGenerator vertexGenerator;
		//	auto font = Graphics::Core::GetDefaultFont().GetFontResolutionWithNextBiggerFontHieght(12);
		//	Graphics::TextGraphicsData textCache{ text, fontSize, font, textColor, vertexGenerator };
		//
		//	Vec2f textBoxPos1 = textLineEnd;
		//	Vec2f textBoxPos2 = textLineEnd + textCache.GetSize() * Vec2f(1, -1) + Vec2f(10, -10);
		//
		//	Graphics::Shapes::DrawLine2D(textLineStart, textLineEnd, 0xffffffff, 1);
		//
		//	Graphics::Shapes::DrawBoxOutline2D(textBoxPos1, textBoxPos2, 0xffffffff, 1);
		//
		//	Graphics::Core::GetDefaultTextRenderer().Render({ .pos = textPos, .pivot = Vec2f(0, 0) }, textCache);
		//}
		//
		//
		return Result();
	}

	Result UIScene::UpdateSystem(const ECS::ComponentTypeData& typeData)
	{
		if (!typeData.GetTypeTags().Contains("UIComponent"))
			Debug::Logger::LogError("Blaze Engine", "Component isnt a UIComponent");

		return UpdateSystemUnsafe(typeData);
	}

	UISystem* UIScene::GetSystem(const ECS::ComponentTypeData& typeData)
	{
		if (!typeData.GetTypeTags().Contains("UIComponent"))
			Debug::Logger::LogError("Blaze Engine", "Component isnt a UIComponent");

		return GetSystemUnsafe(typeData);
	}

	void UIScene::Clear()
	{
		for (auto& layer : layers)
			while (!layer.elements.Empty())
				Destroy(layer.elements.First().element);

		ecsScene = nullptr;

		nameToElementMap.Clear();
		layerNames.Clear();
		layers.Clear();

		focusedElement = nullptr;
		blockingElement = nullptr;

		updateState = 0;
	}

	bool UIScene::TakeFocus(Components::UIElementComponent* element)
	{
		if (IsFocusTaken())
			return false;
		focusedElement = element;
		return true;
	}
	void UIScene::ReturnFocus()
	{
		focusedElement = nullptr;
	}
	bool UIScene::IsFocusTaken()
	{
		return focusedElement != nullptr;
	}
	Components::UIElementComponent* UIScene::GetFocusedElement()
	{
		return focusedElement;
	}

	bool UIScene::IsMouseBlocked() const
	{
		return blockingElement != nullptr;
	}
	Components::UIElementComponent* UIScene::GetBlockingElement() const
	{
		return blockingElement;
	}

	Result UIScene::UpdateSystemUnsafe(const ECS::ComponentTypeData& typeData)
	{
		auto uiSystem = GetSystemUnsafe(typeData);

		if (uiSystem->PreUpdate(this))
		{
			for (const auto& layer : layers)
				for (const auto& el : layer.elements)
					if (auto component = el.entity->GetComponent(typeData))
						uiSystem->Update(this, (UIComponent*)component);

			uiSystem->PostUpdate(this);
		}

		return Result();
	}

	UISystem* UIScene::GetSystemUnsafe(const ECS::ComponentTypeData& typeData)
	{
		return (UISystem*)ecsScene->GetSystem(typeData.Index());
	}

	Result UIScene::PrepareElementCreation(StringView name, StringView layerName)
	{
		Components::UIElementComponent** nameToElementItPtr = nullptr;
		if (name.Size() != 0)
		{
			auto [nameToElementIt, inserted] = nameToElementMap.Insert(name, nullptr);

			if (!inserted)			
				return BLAZE_ERROR_RESULT("BlazeEngine", "Trying to ad a element, but an element with the same name already exists.");			

			nameToElementItPtr = &nameToElementIt->value;
		}

		auto layerMapIt = layerNames.Find(layerName);

		if (layerMapIt.IsNull())		
			return BLAZE_ERROR_RESULT("BlazeEngine", "Trying to ad a element to a layer that doesn't exist");		

		auto layerIt = layerMapIt->value;				
		auto layerElementIt = layerIt->elements.AddBack();
		
		currentUIElementCreationData = &*uiElementCreationData.AddFront();
		currentUIElementCreationData->scene = this;
		currentUIElementCreationData->layerIt = layerIt;
		currentUIElementCreationData->layerElementIt = layerElementIt;
		currentUIElementCreationData->name = name;

		currentUIElementCreationData->nameToElementIt = nameToElementItPtr;

		return Result();
	}

	Result UIScene::FinishElementCreation(Components::UIElementComponent* element)
	{
		if (currentUIElementCreationData->nameToElementIt != nullptr)
		{
			*currentUIElementCreationData->nameToElementIt = element;
			currentUIElementCreationData->nameToElementIt = nullptr;
		}

		auto entity = element->GetEntity();
		currentUIElementCreationData->layerElementIt->element = element;
		currentUIElementCreationData->layerElementIt->transform = (Components::UITransformComponent*)entity->GetComponent(0);
		currentUIElementCreationData->layerElementIt->entity = entity;
		currentUIElementCreationData->layerElementIt = decltype(currentUIElementCreationData->layerElementIt)();

		uiElementCreationData.EraseFirst();

		if (uiElementCreationData.Empty())
			currentUIElementCreationData = nullptr;
		else
			currentUIElementCreationData = &uiElementCreationData.First();

		auto elementComponentOffset = BaseOffset<ECS::Component, Components::UIElementComponent>();

		return Result();
	}

	Result UIScene::ResolveSize(Components::UITransformComponent* transform)
	{
		if (transform->updateState == updateState)
			return Result();

		for (auto comp : transform->elementComponent->uiComponents)
		{
			auto system = (UISystem*)comp->GetSystem();
			system->PreTransform(this, (UIComponent*)comp);
		}

		if (transform->anchor != nullptr)
			ResolveSize(transform->anchor);

		float anchorScale = transform->anchor != nullptr ?
			transform->anchor->finalScale :
			1;

		float anchorRotation = transform->anchor != nullptr ?
			transform->anchor->finalRotation :
			0;


		transform->finalScale = anchorScale * transform->scale;
		transform->finalRotation = transform->rotation + anchorRotation;

		if (transform->sizeOverrideComponent == nullptr)
		{
			//TODO resolve this:		
			//Vec2f anchorSize = transform->anchor != nullptr ?
			//	transform->anchor->finalSize :
			//	(Vec2f)Graphics::Renderer::GetRenderArea().size;
			
			transform->finalSize = transform->size * transform->finalScale;
		}
		else
			transform->finalSize = transform->sizeOverrideComponent->ResolveSize(transform);

		transform->updateState = updateState;

		return Result();
	}
	Result UIScene::ResolvePosition(Components::UITransformComponent* transform)
	{
		if (transform->updateState == updateState)
			return Result();

		if (transform->anchor != nullptr)
			ResolvePosition(transform->anchor);

		if (transform->posOverrideComponent == nullptr)
		{
			float anchorRotation = transform->anchor != nullptr ?
				transform->anchor->finalRotation :
				0;

			Vec2f anchorPos = transform->anchor != nullptr ?
				transform->anchor->finalPos :
				Vec2f();

			float anchorScale = transform->anchor != nullptr ?
				transform->anchor->finalScale :
				1;

			//TODO resolve this:		
			//Vec2f anchorSize = transform->anchor != nullptr ?
			//	transform->anchor->finalSize :
			//	(Vec2f)Graphics::Renderer::GetRenderArea().size;

			//Vec2f offset = transform->pos * anchorScale + transform->anchorPivot * anchorSize - transform->pivot * transform->finalSize;
			//Vec2f pivot = transform->pivot * transform->finalSize;
			//
			//transform->finalPos = anchorPos + RotatePointAround(offset + RotatePointAround(Vec2f(0, 0), transform->rotation, pivot), anchorRotation, Vec2f());
		}
		else
			transform->finalPos = transform->posOverrideComponent->ResolvePos(transform);

		transform->finalPos.x = std::floor(transform->finalPos.x);
		transform->finalPos.y = std::floor(transform->finalPos.y);

		transform->matrix =
			Mat4f::TranslationMatrix(Vec3f(transform->finalPos.x, transform->finalPos.y, 0)) *
			Mat4f::RotationMatrixAxisZ(transform->finalRotation) *
			Mat4f::ScalingMatrix(Vec3f(transform->finalScale, transform->finalScale, 1));

		transform->updateState = updateState;

		for (auto comp : transform->elementComponent->uiComponents)
		{
			auto system = (UISystem*)comp->GetSystem();
			system->PostTransform(this, (UIComponent*)comp);
		}

		return Result();
	}
	UIScene::Layer::Layer()
	{
	}
	UIScene::Layer::~Layer()
	{
	}

}