#include "pch.h"
#include "BlazeEngineGraphics/RenderStructure/RenderGroupRenderer.h"

namespace Blaze::Graphics
{	 
	class GlobalRenderGroupStream :
		public RenderStream 
	{			
	public:
		GlobalRenderGroupStream(RenderGroupStream& renderGroupStream)
			: renderGroupStream(renderGroupStream), renderStream(nullptr)
		{						
		}		

		void BeginStream() override
		{
			if (renderStream == nullptr)
				return;

			renderStream->BeginStream();
		}
		void* Advance() override
		{
			if (renderStream == nullptr)
				return nullptr;

			if (void* renderData = renderStream->Advance())			
				return renderData;
			else
			{
				while (renderStream = renderGroupStream.AdvanceStream())
				{
					renderStream->BeginStream();

					if (void* renderData = renderStream->Advance())
						return renderData;
				}
								
				return nullptr;
			}
		}		
	private:				
		RenderStream* renderStream;
		RenderGroupStream& renderGroupStream;
	};
	
	void RenderGroupRenderer::Render(RenderGroupStream& stream, Vec2u targetSize)
	{
		stream.BeginStream();
		while (auto renderer = stream.AdvanceGroup())
		{
			GlobalRenderGroupStream globalRenderGroupStream{ stream };

			renderer->Render(globalRenderGroupStream, targetSize);
		}
	}
}