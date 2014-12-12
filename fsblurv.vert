#version 150

in vec3 posattrib;
in vec2 tcattrib;
uniform mat4 unimat40;
out vec2 fragposition;
out vec2 fragtexcoord;

out vec2 blurTexCoords[14];


void main(){
	gl_Position = vec4(posattrib, 1.0);
	gl_Position.z = 0.0;
	fragposition = posattrib.xy;
	fragtexcoord = tcattrib;
    blurTexCoords[ 0] =fragtexcoord + vec2(0.0, -0.028);
    blurTexCoords[ 1] =fragtexcoord + vec2(0.0, -0.024);
    blurTexCoords[ 2] =fragtexcoord + vec2(0.0, -0.020);
    blurTexCoords[ 3] =fragtexcoord + vec2(0.0, -0.016);
    blurTexCoords[ 4] =fragtexcoord + vec2(0.0, -0.012);
    blurTexCoords[ 5] =fragtexcoord + vec2(0.0, -0.008);
    blurTexCoords[ 6] =fragtexcoord + vec2(0.0, -0.004);
    blurTexCoords[ 7] =fragtexcoord + vec2(0.0,  0.004);
    blurTexCoords[ 8] =fragtexcoord + vec2(0.0,  0.008);
    blurTexCoords[ 9] =fragtexcoord + vec2(0.0,  0.012);
    blurTexCoords[10] =fragtexcoord + vec2(0.0,  0.016);
    blurTexCoords[11] =fragtexcoord + vec2(0.0,  0.020);
    blurTexCoords[12] =fragtexcoord + vec2(0.0,  0.024);
    blurTexCoords[13] =fragtexcoord + vec2(0.0,  0.028);
}
