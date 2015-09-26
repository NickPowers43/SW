precision mediump float;

uniform vec3 lightPosition;
uniform float lightIntensity;

varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 positionOut;

void main()
{
	vec3 normalInter = normalize(normalOut);
	vec3 lightOffset = lightPosition - positionOut;
	//float cosTheta = clamp(dot(normalInter, lightOffset), 0.0, 1.0);
	float lightDistance = length(lightOffset);
	float colorBrightness = abs(sin(lightDistance));
	if(false)
	{
		lightDistance += 1.0;
		float lightBounce = 1.0 / (lightDistance);
		colorBrightness = lightBounce * lightIntensity;
	}
	gl_FragColor = vec4(colorOut.x * colorBrightness, colorOut.y * colorBrightness, colorOut.z * colorBrightness, colorOut.w);
}