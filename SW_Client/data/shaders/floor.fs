precision mediump float;
uniform sampler2D floorTexture;

varying vec2 myUV;
varying vec3 positionOut;

void main()
{
    vec3 lightSourcePosition = vec3(floor(positionOut.x), 1.7, floor(positionOut.z));
    lightSourcePosition += vec3(0.5, 0.0, 0.5);
	vec3 lightOffset = lightSourcePosition - positionOut;
	float lightDistance = length(lightOffset);
	float colorBrightness;
	if(true)
	{
        lightDistance *= 0.5;
		lightDistance += 1.0;
		float lightBounce = 1.0 / (lightDistance * lightDistance * lightDistance);
		colorBrightness = lightBounce;
	}
    vec4 colorOut = texture2D(floorTexture, myUV);
	gl_FragColor = vec4(colorOut.x * colorBrightness, colorOut.y * colorBrightness, colorOut.z * colorBrightness, colorOut.w);

}