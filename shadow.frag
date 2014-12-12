#version 150

in vec2 fragposition;
in float fragdepth;
in vec2 fragtexcoord;

out vec4 fragColor;
out vec4 normColor;

void main(){
//		fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, fragtexcoord).a);
//		 = texture(texture0, fragtexcoord);
//		fragColor = vec4(1.0, 0.5, 0.5, 1.0);
		fragColor = vec4(fragdepth);
//		fragColor = vec4(0.5);
//		fragColor.r = gl_FragCoord.z;
//		fragColor = gl_FragDepth;
//		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
