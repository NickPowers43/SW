uniform mat4 viewMat;
uniform vec2 playerPos;

attribute vec4 worldPos;
attribute vec4 shadowAttrib;

varying float alpha;

void main()
{
	vec2 worldPosV2 = vec2(worldPos.x, worldPos.y);
	vec2 fworldPos = worldPosV2 + ((normalize(worldPosV2 - playerPos) * 100.0) * shadowAttrib.x);
	vec4 transPoint = vec4(fworldPos.x, fworldPos.y, 0.0, 1.0);

	transPoint = viewMat * transPoint;

	gl_Position = transPoint;
	alpha = shadowAttrib.z;
}