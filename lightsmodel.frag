#version 150

in vec2 fragposition;
in float fragdepth;
in vec2 fragtexcoord;
uniform sampler2D texture0;

out vec4 fragColor;
out vec4 normColor;

void main(){
//		fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, fragtexcoord).a);
//		fragColor = texture(texture0, fragtexcoord);
//		fragColor = vec4(1.0, 0.5, 0.5, 1.0);
		fragColor = vec4(0.2);
		normColor.r = fragdepth;
//		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
