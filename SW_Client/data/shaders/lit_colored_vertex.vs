uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 objMat;

attribute vec4 position;
attribute vec4 normal;
attribute vec4 color;

varying vec4 colorOut;
varying vec3 normalOut;
varying vec3 positionOut;

void main()
{
    vec4 transPosition = objMat * position;
	vec4 tempPosition = projMat * viewMat * transPosition;
	positionOut = vec3(transPosition);
	colorOut = color;
	normalOut = vec3(objMat * normal);
	gl_Position = tempPosition;
}
