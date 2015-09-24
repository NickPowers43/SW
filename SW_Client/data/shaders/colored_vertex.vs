uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 objMat;

attribute vec4 position;
attribute vec4 normal;
attribute vec4 color;

varying vec4 colorOut;

void main()
{
	gl_Position = projMat * viewMat * objMat * position;
	colorOut = color;
}