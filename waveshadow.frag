#version 330

uniform sampler2DShadow texture0;
in vec3 fragposition;
//in vec2 fragtexcoord;
in vec3 frageh;
in vec4 sspace;
flat in vec3 norm;
out vec4 fragColor;
out vec4 normColor;

#define N 512

layout (std140) uniform uniblock0{
	float data[N];
} uniblock0_t;

void main(){
/*		fragColor = vec4(frageh, 1.0);
		normColor.r = fragposition.z;
		vec3 shadowtc = sspace.xyz/sspace.w;
		float fed = texture(texture0, shadowtc);
		float dis = distance(shadowtc.xy, vec2(0.5));
		float feff = step(dis, 0.45);
		float foff = clamp(1.0f - dis * dis / (0.4 * 0.4), 0.0, 1.0f);
		fed *= foff * feff;
		float attenuation = clamp(1.0f - sspace.z * sspace.z / (9.0 * 9.0), 0.0, 1.0f);
		fed*= attenuation * 2.0;
		fragColor *= min(fed+0.5, 1.0);
*/


		fragColor = vec4(fragposition.z);
}
