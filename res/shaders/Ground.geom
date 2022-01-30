#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform int dist;

void main(void){

  

	vec4 vertexPos = gl_in[0].gl_Position;
	gl_Position = projection * view * model * vertexPos;
	EmitVertex();

	vec4 offset = vec4(dist, 0.0, 0.0, 0.0);
	vertexPos = offset + gl_in[0].gl_Position;
	gl_Position = projection * view * model * vertexPos;
	EmitVertex();

	offset = vec4(0.0, 0.0, dist, 0.0);
	vertexPos = offset + gl_in[0].gl_Position;
	gl_Position = projection * view * model * vertexPos;
	EmitVertex();

	offset = vec4(dist, 0.0, dist, 0.0);
	vertexPos = offset + gl_in[0].gl_Position;
	gl_Position = projection * view * model * vertexPos;
	EmitVertex();


	EndPrimitive();

}


