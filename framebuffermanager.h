#ifndef FBMANGERH
#define FBMANGERH

typedef struct framebuffer_s {
	GLuint id;
	GLuint rbid;
	texture_t textureid[8];
	GLuint tid;
	GLuint width;
	GLuint height;
} framebuffer_t;


char states_BindFramebuffer(framebuffer_t fb);


#endif
