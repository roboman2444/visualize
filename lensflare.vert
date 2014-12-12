#version 150

in vec3 posattrib;
in vec2 tcattrib;
uniform mat4 unimat40;
out vec2 fragposition;
out vec2 fragtexcoord;


void main(){
	gl_Position = vec4(posattrib, 1.0);
	gl_Position.z = 0.0;
	fragposition = posattrib.xy;
	fragtexcoord = tcattrib;
}
