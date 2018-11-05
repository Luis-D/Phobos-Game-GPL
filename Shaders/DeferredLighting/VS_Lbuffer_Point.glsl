#version 100

attribute mediump vec3 Vertex_Position_in;
attribute mediump vec4 Center_Linear_in;
attribute mediump vec4 Color_Quadratic_in;

varying mediump vec3 Vertex_Position;
varying mediump vec4 Center_Linear;
varying mediump vec4 Color_Quadratic;


uniform mediump mat4 Matrix;

void main()
{

gl_Position = Matrix* vec4(Vertex_Position_in.x,Vertex_Position_in.y,Vertex_Position_in.z,1.0);
Color_Quadratic = Color_Quadratic_in; 
Center_Linear = Center_Linear_in;
Vertex_Position = Vertex_Position_in;
}
