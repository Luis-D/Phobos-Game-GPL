#version 100

uniform sampler2D Gbuffer;

uniform mediump vec4 Axis_Intensity_in;
uniform mediump vec3 Color;

varying mediump vec2 UV;

void main()
{
	mediump vec3 lightDir = normalize(Axis_Intensity_in.xyz);
	mediump vec4 GbufferData = texture2D(Gbuffer,UV);
	mediump vec3 Normal = GbufferData.xyz;if(length(Normal) <= 0.0) {discard;}Normal*=2.0; Normal-=1.0;	
	mediump float diff = dot(Normal,-lightDir);	
        if(diff<=0.0){discard;}
	mediump vec3 lighColor = Color.xyz*(diff) *  Axis_Intensity_in.a;
     	gl_FragColor.rgb = lighColor;
     	gl_FragColor.a =diff;
}