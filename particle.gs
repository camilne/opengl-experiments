#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 proj;

in Vertex {
	float size;
	vec4 color;
} vertex[];

out vec4 f_color;
out vec2 f_textureCoords;

void main() {
  vec4 P = gl_in[0].gl_Position;

  // a: left-bottom 
  vec2 va = P.xy + vec2(-0.5, -0.5) * vertex[0].size;
  gl_Position = proj * vec4(va, P.zw);
  f_textureCoords = vec2(0.0, 0.0);
  f_color = vertex[0].color;
  EmitVertex();  
  
  // b: left-top
  vec2 vb = P.xy + vec2(-0.5, 0.5) * vertex[0].size;
  gl_Position = proj * vec4(vb, P.zw);
  f_textureCoords = vec2(0.0, 1.0);
  f_color = vertex[0].color;
  EmitVertex();  
  
  // d: right-bottom
  vec2 vd = P.xy + vec2(0.5, -0.5) * vertex[0].size;
  gl_Position = proj * vec4(vd, P.zw);
  f_textureCoords = vec2(1.0, 0.0);
  f_color = vertex[0].color;
  EmitVertex();  

  // c: right-top
  vec2 vc = P.xy + vec2(0.5, 0.5) * vertex[0].size;
  gl_Position = proj * vec4(vc, P.zw);
  f_textureCoords = vec2(1.0, 1.0);
  f_color = vertex[0].color;
  EmitVertex();  

  EndPrimitive(); 
}