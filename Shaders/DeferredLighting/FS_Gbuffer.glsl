#version 100

varying mediump  vec3 Normal;      

void main()
{
     gl_FragData[0].rgb= (normalize(Normal)*0.5)+0.5;
     gl_FragData[0].a = gl_FragCoord.z;
}
