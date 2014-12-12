#ifndef TEXTUREH
#define TEXTUREH

typedef struct texture_s {
	GLuint id;
	GLuint width;
	GLuint height;
	GLenum texformat1;
	GLenum texformat2;
} texture_t;

texture_t loadtexture(const char * filepath);

#endif
