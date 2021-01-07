#include "RenderSystem.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/GameSystem/Components/Transform3D.h"
#include "BlazeEngine/GameSystem/Components/Model.h"
#include "BlazeEngine/GameSystem/Components/Camera.h"

namespace Blaze
{		
	void RenderSystem::Update(Array<Camera> cameras, Array<Model> models, Array<Transform3D> transforms)
	{
		for (auto& camera : cameras)
		{
			Transform3D* cameraTransform = camera.GetEntity().GetComponent<Transform3D>();
			if (cameraTransform == nullptr)
			{
				Logger::AddLog(LogType::Warning, "RenderSystem::Update", "Entity that has a camera component does not have a transform component");
				continue;
			}			
			Mat4f viewMat = Math::ViewMatrix(cameraTransform->pos, cameraTransform->scale, cameraTransform->rot);

			for (auto& model : models)
				for (auto& transform : transforms)
					if (model.GetEntityID() == transform.GetEntityID())
					{
						if (model.mesh != nullptr)
						{
							if (model.mesh->GetMaterialCount() != model.materials.size())
							{
								Logger::AddLog(LogType::Warning, "RenderSystem::Update", "Model material count is not same as mesh material count");
								continue;
							}
							
							model.mesh->GetVertexLayout().Bind();														

							uint offset = 0;
							for (uint i = 0; i < model.materials.size(); ++i)
							{
								if (model.materials[i] == nullptr)
								{
									Logger::AddLog(LogType::Warning, "RenderSystem::Update", "Model material is nullptr");
									continue;
								}


								{									
									PropertyID id;
									id = model.materials[i]->GetPropertyID("u_MVP");
									if (id != -1)
										model.materials[i]->SetProperty(id, camera.projectionMatrix * viewMat * transform.mat);

									id = model.materials[i]->GetPropertyID("u_model");
									if (id != -1)
										model.materials[i]->SetProperty(id, transform.mat);

									id = model.materials[i]->GetPropertyID("u_view");
									if (id != -1)
										model.materials[i]->SetProperty(id, viewMat);

									id = model.materials[i]->GetPropertyID("u_projection");
									if (id != -1)
										model.materials[i]->SetProperty(id, camera.projectionMatrix);
								}

								model.materials[i]->UpdateUniforms();
								model.materials[i]->BindTextures();
								model.materials[i]->GetShaderProgram().Bind();

								Renderer::DrawTriangles(model.mesh->GetIndexMaterialCounts()[i], offset);
								offset += model.mesh->GetIndexMaterialCounts()[i];
							}
						}

						break;
					}
		}
	}
}