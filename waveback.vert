#version 150

uniform float unifloat0;
uniform mat4 unimat40;
uniform mat4 unimat41;
in vec3 posattrib;
in vec2 tcattrib;
out vec3 fragposition;
out vec3 frageh;
out vec2 fragtexcoord;


out vec4 sspace;


void main(){
	vec4 newpos = vec4(posattrib.xy, posattrib.z * unifloat0, 1.0);
	gl_Position = unimat40 * newpos;
	fragposition = gl_Position.xyz;
	frageh = vec3(posattrib.z*5.0, 1.0-posattrib.z*5.0, 0.0);
	fragtexcoord = tcattrib;
	sspace  = unimat41 *newpos;
}
