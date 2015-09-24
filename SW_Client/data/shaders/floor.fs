precision mediump float;
uniform sampler2D floorTexture;

varying vec2 myUV;

void main()
{
	gl_FragColor = texture2D(floorTexture, myUV);
}