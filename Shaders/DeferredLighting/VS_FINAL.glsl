#version 100

attribute mediump vec3 Position_in;
attribute mediump vec2 UV_in;

uniform mat4 Matrix;

varying mediump vec2 UV;
varying mediump vec4 glPosition;

void main()
{
        gl_Position = Matrix * vec4(Position_in.x,Position_in.y,Position_in.z,1.0);
  	glPosition = gl_Position;
	UV = UV_in;
}