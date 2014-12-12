#include <GL/glew.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "globaldefs.h"
#include "texturemanager.h"


texture_t loadtexture(const char * filepath){
	texture_t tex = {0};
	int x=0, y=0, n=0;
	unsigned char *imagedata = stbi_load(filepath, &x, &y, &n, 0);
	if(!imagedata) return tex;
	GLint texformat = GL_RGB;
	switch(n){
		case(1): texformat = GL_RED; break;
		case(2): texformat = GL_RG; break;
		case(3): texformat = GL_RGB; break;
		case(4): texformat = GL_RGBA; break;
		default: return tex; break;
	}
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	if(!tex.id) return tex;
	tex.texformat1 = texformat;
	tex.texformat2 = texformat; //todo maybe
	tex.width = x;
	tex.height = y;
	unsigned char level;
	for(level = 0; level < 255; level++){
		if(1<<level > tex.width && 1<<level > tex.height) break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, texformat, tex.width, tex.height, 0, texformat, GL_UNSIGNED_BYTE, imagedata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	glGenerateMipmap(GL_TEXTURE_2D);
	printf("loaded texture %s\n", filepath);
	stbi_image_free(imagedata);
	imagedata = 0;
	return tex;
}
