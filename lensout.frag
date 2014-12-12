#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

out vec4 fragColor;

void main(){
	vec4 lensmod = texture(texture2, fragtexcoord);
	vec2 lensStarTexcoord = (vec3(fragtexcoord, 1.0)).xy;
	lensmod += texture(texture3, lensStarTexcoord);
	vec4 lensflare = texture(texture1, fragtexcoord) * lensmod;
//	fragColor = lensflare;
	fragColor = texture(texture0, fragtexcoord) + lensflare;
}
