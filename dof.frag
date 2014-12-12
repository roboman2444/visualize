#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 fragColor;



#define PI    3.14159265
#define SD 0.5
//uniform sampler2D Texture_ScreenNormalMap;
vec2 as = textureSize(texture0, 0).xy; //texture aspect
float readDepth(in vec2 coord){ return texture(texture1, coord).r;}

//dof only stuff
const float blurclamp = 10.0;   //max blur amount
const float bias = 0.55;        //aperture - bigger values for shallower depth of field
uniform float coc = 0.03;       //circle of confusion.... also 8===>
uniform float blursize = 10.0;   //size of blur. gonna make something to do width via pixels?
vec2 aspectcorrect = vec2(.001 * blursize , blursize* .001 * as.x/as.y); //maybe move this into main?
float diffarea = 0.4; //self-shadowing reduction
float gdisplace = 0.4; //gauss bell center
float garea = 2.0;





void main(){
//	fragColor = texture(texture0, fragtexcoord);
	float depth = readDepth(fragtexcoord);
	float factor = max(abs(depth - readDepth(fragtexcoord + (vec2(0.5)-fragtexcoord))) - coc, 0);
        vec2 dofblur = vec2 (clamp( factor * bias, -blurclamp, blurclamp ));

        vec4 col = texture(texture0, fragtexcoord);
        col += texture(texture0, fragtexcoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur);

//        col += texture(texture0, fragtexcoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur);
//        col += texture(texture0, fragtexcoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur);
        col += texture(texture0, fragtexcoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur*0.9);
//        col += texture(texture0, fragtexcoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(texture0, fragtexcoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur*0.9);
//        col += texture(texture0, fragtexcoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
        col += texture(texture0, fragtexcoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur*0.9);
//        col += texture(texture0, fragtexcoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(texture0, fragtexcoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur*0.9);
//        col += texture(texture0, fragtexcoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.7);
//        col += texture(texture0, fragtexcoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.7);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.7);
//        col += texture(texture0, fragtexcoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.7);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.7);
//        col += texture(texture0, fragtexcoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.7);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.7);
//        col += texture(texture0, fragtexcoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.7);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.4);
//        col += texture(texture0, fragtexcoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.4);
        col += texture(texture0, fragtexcoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.4);
//        col += texture(texture0, fragtexcoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.4);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.4);
//        col += texture(texture0, fragtexcoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.4);
        col += texture(texture0, fragtexcoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.4);
//        col += texture(texture0, fragtexcoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.4);

        fragColor = col/21.0;

}
