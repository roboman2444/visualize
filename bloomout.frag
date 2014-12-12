#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 fragColor;

void main(){
//	fragColor = lensflare;
	fragColor = texture(texture0, fragtexcoord) + texture(texture1, fragtexcoord) * 0.2;
}
