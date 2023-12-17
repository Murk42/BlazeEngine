#version 450									  
												  
layout(location = 0) in vec2 vert_pos;			  
layout(location = 1) in vec4 vert_color;		  
layout(location = 2) in float vert_radius;		  
												  
out vec2 geom_pos;								  
out vec4 geom_color;							  
out float geom_radius;							  
												  
void main()									  
{												  
	geom_pos = vert_pos;						  
	geom_color = vert_color;					  
	geom_radius = vert_radius;					  
}												  