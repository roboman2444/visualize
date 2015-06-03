#version 330
in vec2 posattrib;

#define N 512

layout (std140) uniform uniblock0{
        float data[N];
} uniblock0_t;


out VS_OUT {
	flat ivec2 bp;
} vs_out;

void main(){
//	gl_Position = unimat40 *
///	float myheight = 
//	vec4 newpos = vec4(posattrib.xy, uniblock0.data[ * unifloat0, 1.0);
//	gl_Position = unimat40 * newpos;
//	fragposition = gl_Position.xyz;
//	frageh = vec3(posattrib.z*5.0, 1.0-posattrib.z*5.0, 0.0);
//	sspace  = unimat41 *newpos;
	vs_out.bp = ivec2(posattrib);
}
