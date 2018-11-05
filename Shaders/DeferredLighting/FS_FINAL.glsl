#version 100

uniform sampler2D LightTexture;
uniform sampler2D Textura;

varying mediump vec2 UV;
varying mediump vec4 glPosition;

void main()
{ 
	mediump vec3 projcoord = glPosition.xyz / glPosition.w;
	projcoord = projcoord * 0.5 + 0.5;

	mediump vec4 LightTextureColor = texture2D(LightTexture,projcoord.xy);

	//gl_FragData[0] = texture2D(Textura,UV) 
	gl_FragData[0] = vec4(1.0,1.0,1.0,1.0);

	gl_FragData[0] *= vec4(LightTextureColor.x,LightTextureColor.y,LightTextureColor.z,1.0);
	
}