#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform float uGhostDispersal = 0.3;
uniform float uHaloWidth = 0.8;
uniform vec3 uBias = vec3(-0.5);

out vec4 fragColor;
vec3 textureDistorted(
      in sampler2D tex,
      in vec2 texcoord,
      in vec2 direction, // direction of distortion
      in vec3 distortion // per-channel distortion factor
   ) {
      return vec3(
	texture(tex, texcoord + direction * distortion.r).r,
	texture(tex, texcoord + direction * distortion.g).g,
	texture(tex, texcoord + direction * distortion.b).b
      );
   }
void main(){
	vec2 ftc = -fragtexcoord + vec2(1.0);
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	vec2 ghostVec = (vec2(0.5) - fragtexcoord) * uGhostDispersal;
	vec2 texelSize = 1.0 / vec2(textureSize(texture0, 0));
	// ghost vector to image centre:
	vec2 direction = normalize(ghostVec);
	vec3 distortion = vec3(-0.05, 0.0, 0.05);
	for(float i = 0; i < 4.0; i+= 1.0){
		vec2 offset = fragtexcoord + ghostVec * i;
		float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
		weight = pow(1.0 - weight, 10.0);

//		fragColor.rgb += max(vec3(0.0), texture(texture0, offset).rgb + uBias) * weight * 0.2;
		fragColor.rgb += max(vec3(0.0), textureDistorted(texture0, offset, direction, distortion).rgb + uBias) * weight;
	}
	vec2 haloVec = normalize(ghostVec) * uHaloWidth;
	float weight = length(vec2(0.5) - fract(fragtexcoord + haloVec)) / length(vec2(0.5));
	weight = pow(1.0 - weight, 5.0);
	fragColor.rgb += max(textureDistorted(texture0, fragtexcoord + haloVec, direction, distortion).rgb +uBias, vec3(0.0)) * weight * 30.0;
	fragColor.rgb *= texture(texture1, vec2(length(vec2(0.5) - fragtexcoord) / length(vec2(0.5)), 0.0)).rgb;
}
