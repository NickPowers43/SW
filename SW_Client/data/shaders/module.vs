uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 objMat;

attribute vec4 position;
attribute vec4 color;

varying vec4 colorOut;

void main()
{
    vec4 transPosition = objMat * position;
	vec4 tempPosition = projMat * viewMat * transPosition;
	colorOut = color;
	gl_Position = tempPosition;
}
