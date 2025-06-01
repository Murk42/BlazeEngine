#include "Shaders.h"

namespace Blaze::ShaderSources
{
	const String basicMesh_frag    = { { $bytes "../Shaders/OpenGL/basicMesh.frag"$ } };	
	const String basicMesh_geom    = { { $bytes "../Shaders/OpenGL/basicMesh.geom"$ } };
	const String basicMesh_vert    = { { $bytes "../Shaders/OpenGL/basicMesh.vert"$ } };		   
	const String line2d_frag       = { { $bytes "../Shaders/OpenGL/line2d.frag"$ } };
	const String line2d_vert       = { { $bytes "../Shaders/OpenGL/line2d.vert"$ } };
	const String line3d_frag       = { { $bytes "../Shaders/OpenGL/line3d.frag"$ } };
	const String line3d_vert       = { { $bytes "../Shaders/OpenGL/line3d.vert"$ } };
	const String panel_frag        = { { $bytes "../Shaders/OpenGL/panel.frag"$ } };
	const String panel_vert        = { { $bytes "../Shaders/OpenGL/panel.vert"$ } };
	const String text_frag         = { { $bytes "../Shaders/OpenGL/text.frag"$ } };
	const String texturedRect_frag = { { $bytes "../Shaders/OpenGL/texturedRect.frag"$ } };
	const String texturedRect_vert = { { $bytes "../Shaders/OpenGL/texturedRect.vert"$ } };
}