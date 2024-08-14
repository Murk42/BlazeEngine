#pragma once

namespace Blaze::Graphics
{
	class RenderStream
	{
	public:
		virtual void BeginStream() = 0;		
		virtual void* Advance() = 0;		
	};	
}