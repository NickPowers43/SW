precision mediump float;

uniform vec3 lightPosition;
uniform float lightIntensity;

varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 positionOut;

void main()
{
    vec3 lightSourcePosition = vec3(floor(positionOut.x), 1.7, floor(positionOut.z));
    lightSourcePosition += vec3(0.5, 0.0, 0.5);
	vec3 normalInter = normalize(normalOut);
	vec3 lightOffset = lightSourcePosition - positionOut;
	float lightDistance = length(lightOffset);
	float colorBrightness;
	lightDistance *= 0.5;
	lightDistance += 1.0;
	float lightBounce = 1.0 / (lightDistance * lightDistance * lightDistance);
	colorBrightness = lightBounce;
	gl_FragColor.rgb = vec3(colorBrightness, colorBrightness, colorBrightness);//vec3(colorOut.x * colorBrightness, colorOut.y * colorBrightness, colorOut.z * colorBrightness);
}