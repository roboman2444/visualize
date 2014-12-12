#version 150

uniform mat4 unimat40;
uniform float unifloat0;
in vec3 posattrib;
out vec2 fragposition;
out float fragdepth;
void main(){
	gl_Position = unimat40 * vec4(posattrib.xy, posattrib.z * unifloat0, 1.0);
	fragdepth = gl_Position.z;
}
