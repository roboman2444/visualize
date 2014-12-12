#version 150

uniform float unifloat0;
uniform sampler2D texture0;
in vec3 fragposition;
in vec2 fragtexcoord;
in vec3 frageh;
in vec4 sspace;
out vec4 fragColor;
out vec4 normColor;

void main(){
		fragColor = vec4(frageh, 1.0);
		normColor.r = fragposition.z;
//		vec2 shadowtc = (sspace.xy*0.5) +vec2(1.0);
		vec2 shadowtc = sspace.xy/sspace.w;
		float def = texture(texture0, shadowtc).r;// + 0.11;
		float fed = step(sspace.z,def);
		float dis = distance(shadowtc, vec2(0.5));
		float feff = step(dis, 0.45);
		float foff = clamp(1.0f - dis * dis / (0.4 * 0.4), 0.0, 1.0f);
		fed *= foff * feff;
		float attenuation = clamp(1.0f - sspace.z * sspace.z / (9.0 * 9.0), 0.0, 1.0f);
		fed*= attenuation * 2.0;
		fragColor *= min(fed+0.5, 1.0);
//		if(sspace.z / sscreen.w < def) fragColor *=0.1;
//		fragColor = vec4(abs(def - sspace.z));
//		fragColor = vec4(def + 0.1);
}
