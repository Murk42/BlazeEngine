#include "Shaders.h"

namespace Blaze::ShaderSources
{
	static const char antialiasedText_frag_str[] = { $bytes "OpenGL/antialiasedText.frag"$, '\0' };
	const StringView antialiasedText_frag = StringView(antialiasedText_frag_str);
	static const char antialiasedText_vert_str[] = { $bytes "OpenGL/antialiasedText.vert"$, '\0' };
	const StringView antialiasedText_vert = StringView(antialiasedText_vert_str);
}