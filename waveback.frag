#version 330

uniform sampler2DShadow texture0;
uniform sampler2D texture1;
uniform vec3 univec31;
uniform vec3 univec32;
in vec3 fragposition;
in vec3 worldpos;
//in vec2 fragtexcoord;
in vec3 fragh;
in vec4 sspace;
flat in vec3 norm;
out vec4 fragColor;
out vec4 normColor;

#define N 512

layout (std140) uniform uniblock0{
	float data[N];
} uniblock0_t;

void main(){
//		fragColor = vec4(fragh, 1.0);
		fragColor = vec4(1.0);
		normColor.r = fragposition.z;
		vec3 shadowtc = sspace.xyz/sspace.w;
		float fed = texture(texture0, shadowtc);
		float dis = distance(shadowtc.xy, vec2(0.5));
		float feff = step(dis, 0.45);
		float foff = clamp(1.0f - dis * dis / (0.4 * 0.4), 0.0, 1.0f);
		fed *= foff * feff;
		float attenuation = clamp(1.0f - sspace.z * sspace.z / (9.0 * 9.0), 0.0, 1.0f);
		fed*= attenuation * 2.0;


		vec3 lightdelta = univec31 - worldpos;
		vec3 camdelta = univec32 - worldpos;
		vec3 lightnorm = normalize(lightdelta);
		vec3 camnorm = normalize(camdelta);
		vec3 vhalf = normalize(lightnorm + camnorm);

		float diffuse = clamp(dot(lightnorm, norm), 0.0, 1.0);
		float spec = clamp(pow(dot(norm, vhalf), 100.0), 0.0, 1.0);
		fragColor = (diffuse+spec) * vec4(fragh,1.0) * max(fed,0.0);
		fragColor += 0.1 * vec4(fragh,1.0);

}
