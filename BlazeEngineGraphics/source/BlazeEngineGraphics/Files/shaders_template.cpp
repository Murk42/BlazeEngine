#include "shaders.h"

//$time "../Shaders/OpenGL/line2d.frag"$
//$time "../Shaders/OpenGL/line2d.vert"$
//$time "../Shaders/OpenGL/line3d.frag"$
//$time "../Shaders/OpenGL/line3d.vert"$
//$time "../Shaders/OpenGL/panel.frag"$
//$time "../Shaders/OpenGL/panel.vert"$
//$time "../Shaders/OpenGL/text.frag"$
//$time "../Shaders/OpenGL/texturedRect.frag"$
//$time "../Shaders/OpenGL/texturedRect.vert"$

const unsigned int line2d_frag_size =       $size "../Shaders/OpenGL/line2d.frag"$;
const unsigned int line2d_vert_size =       $size "../Shaders/OpenGL/line2d.vert"$;
const unsigned int line3d_frag_size =		$size "../Shaders/OpenGL/line3d.frag"$;
const unsigned int line3d_vert_size =		$size "../Shaders/OpenGL/line3d.vert"$;
const unsigned int panel_frag_size =        $size "../Shaders/OpenGL/panel.frag"$;
const unsigned int panel_vert_size =        $size "../Shaders/OpenGL/panel.vert"$;
const unsigned int text_frag_size =         $size "../Shaders/OpenGL/text.frag"$;
const unsigned int texturedRect_frag_size = $size "../Shaders/OpenGL/texturedRect.frag"$;
const unsigned int texturedRect_vert_size = $size "../Shaders/OpenGL/texturedRect.vert"$;

const unsigned char line2d_frag_file[] =       { $bytes "../Shaders/OpenGL/line2d.frag"$ };
const unsigned char line2d_vert_file[] =       { $bytes "../Shaders/OpenGL/line2d.vert"$ };
const unsigned char line3d_frag_file[] =       { $bytes "../Shaders/OpenGL/line3d.frag"$ };
const unsigned char line3d_vert_file[] =       { $bytes "../Shaders/OpenGL/line3d.vert"$ };
const unsigned char panel_frag_file[] =        { $bytes "../Shaders/OpenGL/panel.frag"$ };
const unsigned char panel_vert_file[] =        { $bytes "../Shaders/OpenGL/panel.vert"$ };
const unsigned char text_frag_file[] =         { $bytes "../Shaders/OpenGL/text.frag"$ };
const unsigned char texturedRect_frag_file[] = { $bytes "../Shaders/OpenGL/texturedRect.frag"$ };
const unsigned char texturedRect_vert_file[] = { $bytes "../Shaders/OpenGL/texturedRect.vert"$ };