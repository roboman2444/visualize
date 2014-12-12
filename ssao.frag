#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;

#define PI    3.14159265
#define SD 10.0
float width = textureSize(texture0, 0).x; //texture width
float height = textureSize(texture0, 0).y; //texture width
float readDepth(in vec2 coord){ return texture(texture1, coord).r;}

//these are the random vectors inside a unit sphere... i think
//const vec3 pSphere[16]  = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));
const vec3 pSphere[32]  = vec3[](vec3(0.74766022, -0.23212385, 0.62219185), vec3(0.50445920, 0.69581580, -0.51123500), vec3(-0.42762125, 0.69609535, -0.57670730), vec3(0.38132653, -0.15970179, 0.91054124), vec3(-0.28635111, 0.02837963, 0.95770437), vec3(0.85159028, 0.27768418, 0.44461834), vec3(-0.58615518, 0.17494646, -0.79108530), vec3(-0.47725669, -0.67337513, 0.56461656), vec3(-0.66725743, -0.19251841, -0.71951658), vec3(-0.56385201, 0.71913582, -0.40609664), vec3(0.03616812, 0.94839561, 0.31502002), vec3(-0.82507598, 0.55641502, 0.09824442), vec3(-0.01242788, 0.91562790, -0.40183467), vec3(0.70726579, 0.06970797, 0.70350248), vec3(-0.21761429, 0.85397637, -0.47261861), vec3(-0.27301651, 0.56942487, 0.77538210), vec3(-0.69100028, 0.72164774, 0.04175022), vec3(-0.76508874, -0.56887800, 0.30169061), vec3(0.64586323, -0.25009316, -0.72132802), vec3(-0.73794395, -0.06503175, -0.67172140), vec3(-0.38938627, 0.70020789, 0.59840387), vec3(0.82903844, -0.55124581, 0.09393275), vec3(-0.43196797, 0.90084457, 0.04339096), vec3(0.34137559, 0.06432997, -0.93772298), vec3(-0.10170624, 0.70427561, 0.70260352), vec3(0.56626922, -0.55282509, 0.61132938), vec3(0.50713050, -0.52876836, 0.68060464), vec3(-0.65556043, -0.11755112, 0.74593711), vec3(0.71732247, -0.36969191, -0.59057283), vec3(0.85439140, -0.32501256, 0.40544060), vec3(0.92989451, 0.18057333, 0.32045203), vec3(0.64208001, -0.10819609, 0.75896436));

float diffarea = 0.4; //self-shadowing reduction
float gdisplace = 0.4; //gauss bell center
float garea = 2.0;


out vec4 fragColor;
void main(){
//		fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, fragtexcoord).a);
		fragColor = texture(texture0, fragtexcoord);
//		fragColor.b = 0.0;
//		fragColor = vec4(1.0, 0.5, 0.5, 1.0);
//		fragColor = vec4(fragposition, 1.0, 1.0);
//		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
//|SSAO|
//| by |
//|ROBO|
	float depth = readDepth(fragtexcoord);


	float diff = 0;

	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[0].x/100) ,fragtexcoord.y + (pSphere[0].y/100))), pSphere[0].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[1].x/100) ,fragtexcoord.y + (pSphere[1].y/100))), pSphere[1].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[2].x/100) ,fragtexcoord.y + (pSphere[2].y/100))), pSphere[2].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[3].x/100) ,fragtexcoord.y + (pSphere[3].y/100))), pSphere[3].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[4].x/100) ,fragtexcoord.y + (pSphere[4].y/100))), pSphere[4].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[5].x/100) ,fragtexcoord.y + (pSphere[5].y/100))), pSphere[5].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[6].x/100) ,fragtexcoord.y + (pSphere[6].y/100))), pSphere[6].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[7].x/100) ,fragtexcoord.y + (pSphere[7].y/100))), pSphere[7].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[8].x/100) ,fragtexcoord.y + (pSphere[8].y/100))), pSphere[8].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[9].x/100) ,fragtexcoord.y + (pSphere[9].y/100))), pSphere[9].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[10].x/100) ,fragtexcoord.y + (pSphere[10].y/100))), pSphere[10].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[11].x/100) ,fragtexcoord.y + (pSphere[11].y/100))), pSphere[11].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[12].x/100) ,fragtexcoord.y + (pSphere[12].y/100))), pSphere[12].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[13].x/100) ,fragtexcoord.y + (pSphere[13].y/100))), pSphere[13].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[14].x/100) ,fragtexcoord.y + (pSphere[14].y/100))), pSphere[14].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[15].x/100) ,fragtexcoord.y + (pSphere[15].y/100))), pSphere[15].z/SD);

	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[16].x/100) ,fragtexcoord.y + (pSphere[16].y/100))), pSphere[16].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[17].x/100) ,fragtexcoord.y + (pSphere[17].y/100))), pSphere[17].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[18].x/100) ,fragtexcoord.y + (pSphere[18].y/100))), pSphere[18].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[19].x/100) ,fragtexcoord.y + (pSphere[19].y/100))), pSphere[19].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[20].x/100) ,fragtexcoord.y + (pSphere[20].y/100))), pSphere[20].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[21].x/100) ,fragtexcoord.y + (pSphere[21].y/100))), pSphere[21].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[22].x/100) ,fragtexcoord.y + (pSphere[22].y/100))), pSphere[22].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[23].x/100) ,fragtexcoord.y + (pSphere[23].y/100))), pSphere[23].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[24].x/100) ,fragtexcoord.y + (pSphere[24].y/100))), pSphere[24].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[25].x/100) ,fragtexcoord.y + (pSphere[25].y/100))), pSphere[25].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[26].x/100) ,fragtexcoord.y + (pSphere[26].y/100))), pSphere[26].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[27].x/100) ,fragtexcoord.y + (pSphere[27].y/100))), pSphere[27].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[28].x/100) ,fragtexcoord.y + (pSphere[28].y/100))), pSphere[28].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[29].x/100) ,fragtexcoord.y + (pSphere[29].y/100))), pSphere[29].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[30].x/100) ,fragtexcoord.y + (pSphere[30].y/100))), pSphere[30].z/SD);
	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[31].x/100) ,fragtexcoord.y + (pSphere[31].y/100))), pSphere[31].z/SD);
//	diff += step(depth - readDepth(vec2(fragtexcoord.x + (pSphere[32].x/100) ,fragtexcoord.y + (pSphere[32].y/100))), pSphere[32].z/SD);


	diff /= 32;
//	diff /=16;

	fragColor *= diff;


}
