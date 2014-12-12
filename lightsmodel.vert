#version 150

uniform mat4 unimat40;
in vec3 posattrib;
in vec2 tcattrib;
out vec2 fragposition;
out float fragdepth;
out vec2 fragtexcoord;


void main(){
	gl_Position = unimat40 * vec4(posattrib, 1.0);
//	gl_Position = vec4(posattrib, 1.0);
//	gl_Position.z = 0.0;
	fragdepth = gl_Position.z;
	fragposition = posattrib.xy;
	fragtexcoord = tcattrib;
}
