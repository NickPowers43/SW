uniform mat4 viewMat;
uniform mat4 projMat;


attribute vec2 floorPos;
attribute vec2 floorUV;

varying vec2 myUV;
varying vec3 positionOut;

void main()
{
	vec4 transPoint = vec4(floorPos.x, 0.0, floorPos.y, 1.0);
    positionOut = vec3(transPoint);
	transPoint = projMat * (viewMat * transPoint);
	
	gl_Position = transPoint;
	myUV = floorUV;
}
