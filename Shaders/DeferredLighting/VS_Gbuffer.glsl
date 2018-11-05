#version 100

attribute mediump vec3 Position_in;
attribute mediump vec3 Normal_in;

uniform mat4 Matrix;

varying mediump vec3 Normal;   

void main()
{
    gl_Position = Matrix * vec4(Position_in.x,Position_in.y,Position_in.z,1.0);
   	Normal = Normal_in;
}