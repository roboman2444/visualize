#version 150

in vec2 fragposition;
in vec2 fragtexcoord;
uniform sampler2D texture0;
in vec2 blurTexCoords[14];

out vec4 fragColor;

void main(){
//		fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, fragtexcoord).a);
//		fragColor = texture(texture0, fragtexcoord);
//		fragColor.b = 0.0;
//		fragColor = vec4(1.0, 0.5, 0.5, 1.0);
//		fragColor = vec4(fragposition, 1.0, 1.0);
//		fragColor = vec4(1.0, 0.0, 0.0, 1.0);


    fragColor = texture(texture0, blurTexCoords[ 0])*0.0044299121055113265;
    fragColor += texture(texture0, blurTexCoords[ 1])*0.00895781211794;
    fragColor += texture(texture0, blurTexCoords[ 2])*0.0215963866053;
    fragColor += texture(texture0, blurTexCoords[ 3])*0.0443683338718;
    fragColor += texture(texture0, blurTexCoords[ 4])*0.0776744219933;
    fragColor += texture(texture0, blurTexCoords[ 5])*0.115876621105;
    fragColor += texture(texture0, blurTexCoords[ 6])*0.147308056121;
    fragColor += texture(texture0, fragtexcoord         )*0.159576912161;
    fragColor += texture(texture0, blurTexCoords[ 7])*0.147308056121;
    fragColor += texture(texture0, blurTexCoords[ 8])*0.115876621105;
    fragColor += texture(texture0, blurTexCoords[ 9])*0.0776744219933;
    fragColor += texture(texture0, blurTexCoords[10])*0.0443683338718;
    fragColor += texture(texture0, blurTexCoords[11])*0.0215963866053;
    fragColor += texture(texture0, blurTexCoords[12])*0.00895781211794;
    fragColor += texture(texture0, blurTexCoords[13])*0.0044299121055113265;

}
