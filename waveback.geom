#version 330
#extension GL_ARB_gpu_shader5 : enable
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//in ivec2 posattrib;

uniform vec3 univec30;
uniform ivec2 univec20;
uniform mat4 unimat40;
uniform mat4 unimat41;

#define N 512

layout (std140) uniform uniblock0{
        float data[N];
} uniblock0_t;

in VS_OUT{
	flat ivec2 bp;
} gs_in[];


uniform sampler2D texture1;

out vec3 fragh;
out vec3 worldpos;
out vec3 fragposition;
flat out vec3 norm;
out vec4 sspace;



void main(){

	ivec2 mypos;
	mypos = gs_in[0].bp;
	vec2 myoff = vec2(univec20)/2.0;
	vec4 newpos;
	float het;
	vec4 jet;
//	jet = textureGather(texture1, mypos / vec2(univec20), 0);
	jet.w = texture(texture1, vec2(mypos.x, mypos.y) / vec2 (univec20)).x;
	jet.x = texture(texture1, vec2(mypos.x+1, mypos.y) / vec2 (univec20)).x;
	jet.z = texture(texture1, vec2(mypos.x, mypos.y+1) / vec2 (univec20)).x;
	jet.y = texture(texture1, vec2(mypos.x+1, mypos.y+1) / vec2 (univec20)).x;

	vec2 mynewpos = mypos - myoff;

	vec4 p0 =  vec4((mynewpos) * univec30.xy, jet.w * univec30.z, 1.0);
	vec4 p1 =  vec4((mynewpos + ivec2(1,0)) * univec30.xy, jet.x * univec30.z, 1.0);
	vec4 p2 =  vec4((mynewpos + ivec2(0,1)) * univec30.xy, jet.z * univec30.z, 1.0);
	vec4 p3 =  vec4((mynewpos + ivec2(1,1)) * univec30.xy, jet.y * univec30.z, 1.0);

	vec3 norm1 = normalize(cross(vec3(p0 - p1), vec3(p0 - p2)));
	vec3 norm2 = normalize(cross(vec3(p3 - p2), vec3(p3 - p1)));



	gl_Position = unimat40 * p0;
	worldpos = vec3(p0);
	fragposition = gl_Position.xyz;
	fragh = vec3(jet.w*5.0, 1.0-jet.w*5.0, 0.0);
	sspace = unimat41 * p0;
	norm = norm1;
	EmitVertex(); //bottom left


	gl_Position = unimat40 * p1;
	worldpos = vec3(p1);
	fragposition = gl_Position.xyz;
	fragh = vec3(jet.z*5.0, 1.0-jet.z*5.0, 0.0);
	sspace = unimat41 * p1;
	norm = norm1;
	EmitVertex(); //bottom right



	gl_Position = unimat40 * p2;
	worldpos = vec3(p2);
	fragposition = gl_Position.xyz;
	fragh = vec3(jet.x*5.0, 1.0-jet.x*5.0, 0.0);
	sspace = unimat41 * p2;
	norm = norm1;
	EmitVertex();// top left


	gl_Position = unimat40 * p3;
	worldpos = vec3(p3);
	fragposition = gl_Position.xyz;
	fragh = vec3(jet.y*5.0, 1.0-jet.y*5.0, 0.0);
	sspace = unimat41 * p3;
	norm = norm2;
	EmitVertex(); //top right
	EndPrimitive();

}
