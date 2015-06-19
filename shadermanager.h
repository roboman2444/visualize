#ifndef SHADERH
#define SHADERH


typedef struct shader_s {
	GLuint programid;
	GLuint texturepos[16];
	GLuint unimat40;
	GLuint unimat41;
	GLuint univec40;
	GLuint univec30;
	GLuint univec31;
	GLuint univec32;
	GLuint univec20;
	GLuint uniint0;
	GLuint uniscreensizefix;
	GLuint uniscreentodepth;
	GLuint unifloat0;
	GLuint uniblock0;
	GLuint uniblock1;
}shader_t;



shader_t compileShader(const char * filename);

char states_useProgram(const GLuint id);

#endif
