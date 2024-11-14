#version 450

layout(location = 0) in vec4 vert_color;
layout(location = 1) in vec3 vert_pos1;
layout(location = 2) in vec3 vert_pos2;
layout(location = 3) in float vert_width;

layout(location = 0) uniform mat4 u_MVP;
layout(location = 1) uniform mat4 u_projection;
layout(location = 2) uniform vec2 u_targetSize;

out vec2 frag_pos1; 
out vec2 frag_pos2;
out vec4 frag_color;
out float frag_width;

vec4 Project(vec4 value)
{	
	return vec4(value.xyz / value.w, value.w);
}
vec4 ReverseProject(vec3 value)
{
	return vec4(value.xy * value.z, u_projection[2][2] * value.z + u_projection[3][2], value.z);
}

vec3 ScaleToTarget(vec3 value)
{
	return vec3((value.xy + vec2(1)) / 2 * u_targetSize, value.z);
}
vec3 ReverseScaleToTarget(vec3 value)
{
	return vec3(value.xy / u_targetSize * 2 - vec2(1), value.z);
}

#define BOUND 1.0

vec2 FindClipTOnAxis(float p1, float p2)
{
	float t1 = 0, t2 = 1;		
	
	if (p1 == p2)
		return vec2(0, 1);

	float a = 1.0f / (p2 - p1);
	float b = -p1 * a;

	if (p1 > BOUND && p2 > BOUND)
		return vec2(1, 1);

	if (p1 < -BOUND && p2 < -BOUND)
		return vec2(0, 0);

	if (p1 < -BOUND)			
		t1 = -BOUND * a + b;	

	if (p1 > BOUND)			
		t1 = BOUND * a + b;	

	if (p2 < -BOUND)			
		t2 = -BOUND * a + b;	

	if (p2 > BOUND)			
		t2 = BOUND * a + b;		

	return vec2(t1, t2);
}

vec2 FindClipT(vec3 p1, vec3 p2)
{
	vec2 tX = FindClipTOnAxis(p1.x, p2.x);
	vec2 tY = FindClipTOnAxis(p1.y, p2.y);
	vec2 tZ = FindClipTOnAxis(p1.z, p2.z);

	return vec2(max(max(tX.x, tY.x), tZ.x), min(min(tX.y, tY.y), tZ.y));
}

void Clip(inout vec4 p1, inout vec4 p2)
{
	vec2 t = FindClipT(p1.xyz, p2.xyz);
	
	vec4 start = p1;
	vec4 diff = p2 - p1;

	p1 = start + t.x * diff;
	p2 = start + t.y * diff;
}

void ClipZ0(inout vec3 p1, inout vec3 p2)
{
	float t1 = 0, t2 = 1;

	if (p1.z == p2.z)
		return;

	vec3 diff = p2 - p1;

	if (p1.z < 0)
		t1 = -p1.z / diff.z;
	
	if (p2.z < 0)
		t2 = -p1.z / diff.z;

	p2 = p1 + diff * t2;
	p1 += diff * t1;
}

void main() 
{	
	vec3 p1_screen = ScaleToTarget(Project(u_MVP * vec4(vert_pos1, 1)).xyw);
	vec3 p2_screen = ScaleToTarget(Project(u_MVP * vec4(vert_pos2, 1)).xyw);

	//ClipZ0(p1_screen, p2_screen);

	vec3 diff = p2_screen - p1_screen;
	float dist = length(diff.xy);
	vec3 dir = diff / dist;
	vec3 norm = vec3(-dir.y, dir.x, 0);
		
	float t = gl_VertexID < 2 ? -vert_width : dist + vert_width;
	float n = gl_VertexID % 2 == 0 ? -vert_width : vert_width;	
		
	vec3 pos_screen = p1_screen + dir * t + norm * n;		

	vec3 pos_normalized = ReverseScaleToTarget(pos_screen);	
				
	gl_Position = vec4(pos_normalized.xy * pos_normalized.z, (u_projection[2][2] * pos_normalized.z + u_projection[3][2]), pos_normalized.z);

	frag_color = vert_color;	 
	frag_pos1 = p1_screen.xy;
	frag_pos2 = p2_screen.xy;
	frag_width = vert_width;
}