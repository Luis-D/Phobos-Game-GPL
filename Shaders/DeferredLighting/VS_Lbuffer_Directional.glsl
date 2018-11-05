#version 100

attribute mediump vec4 Position_UV;


varying mediump vec2 UV;


void main()
{
gl_Position = vec4(Position_UV.x,Position_UV.y,0.0,1.0);
UV = Position_UV.zw;
}
