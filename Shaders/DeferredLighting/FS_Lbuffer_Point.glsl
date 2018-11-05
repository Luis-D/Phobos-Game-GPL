#version 100

uniform sampler2D Gbuffer;

varying mediump vec3 Vertex_Position;
varying mediump vec4 Center_Linear;
varying mediump vec4 Color_Quadratic;

void main()
{
	mediump vec4 GbufferData = texture2D(Gbuffer,UV);
	mediump vec3 Normal = GbufferData.xyz;

	mediump vec3 lightDir = normalize(Vertex_Position - Center_Linear.xyz);
	lowp float diff = max(dot(Normal,-lightDir), 0.0);

	if(diff = 0.0) {discard;}
	
	mediump float Distance = length (Center_Intensity.xyz - Vertex_Position);
	medium float att = 1.0 / (1.0 + Center_Linear.a * Distance + Color_Quadratic.a * (Distance*Distance));

	mediump vec3 FLight = (Color_Quadratic.xyz * diff *att);
	mediump vec3 Blanco =  vec3(1.0,1.0,1.0);	

     	gl_FragData[0].rgb = Blanco.xyz * FLight;
     	gl_FragData[0].a = GbufferData.a;
}