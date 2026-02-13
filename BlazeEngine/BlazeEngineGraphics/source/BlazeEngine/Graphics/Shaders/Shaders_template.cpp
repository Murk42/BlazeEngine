#include "Shaders.h"

namespace Blaze::ShaderSources
{
	static const char basicMesh_frag_str[] = { $bytes "../Shaders/OpenGL/basicMesh.frag"$, '\0' };
	const StringView basicMesh_frag = StringView(basicMesh_frag_str);
	static const char basicMesh_geom_str[] = { $bytes "../Shaders/OpenGL/basicMesh.geom"$, '\0' };
	const StringView basicMesh_geom = StringView(basicMesh_geom_str);
	static const char basicMesh_vert_str[] = { $bytes "../Shaders/OpenGL/basicMesh.vert"$, '\0' };
	const StringView basicMesh_vert = StringView(basicMesh_vert_str);
	static const char line2d_frag_str[] = { $bytes "../Shaders/OpenGL/line2d.frag"$, '\0' };
	const StringView line2d_frag = StringView(line2d_frag_str);
	static const char line2d_vert_str[] = { $bytes "../Shaders/OpenGL/line2d.vert"$, '\0' };
	const StringView line2d_vert = StringView(line2d_vert_str);
	static const char line3d_frag_str[] = { $bytes "../Shaders/OpenGL/line3d.frag"$, '\0' };
	const StringView line3d_frag = StringView(line3d_frag_str);
	static const char line3d_vert_str[] = { $bytes "../Shaders/OpenGL/line3d.vert"$, '\0' };
	const StringView line3d_vert = StringView(line3d_vert_str);
	static const char panel_frag_str[] = { $bytes "../Shaders/OpenGL/panel.frag"$, '\0' };
	const StringView panel_frag = StringView(panel_frag_str);
	static const char panel_vert_str[] = { $bytes "../Shaders/OpenGL/panel.vert"$, '\0' };
	const StringView panel_vert = StringView(panel_vert_str);
	static const char texturedRect_frag_str[] = { $bytes "../Shaders/OpenGL/texturedRect.frag"$, '\0' };
	const StringView texturedRect_frag = StringView(texturedRect_frag_str);
	static const char texturedRect_vert_str[] = { $bytes "../Shaders/OpenGL/texturedRect.vert"$, '\0' };
	const StringView texturedRect_vert = StringView(texturedRect_vert_str);
}