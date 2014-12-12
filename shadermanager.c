#include <GL/glew.h>
#include <GL/gl.h>
#include "globaldefs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shadermanager.h"


GLuint shadercurrentbound = 0;

char states_useProgram(const GLuint shaderid){
	if(shaderid !=shadercurrentbound){
		glUseProgram(shaderid);
		shadercurrentbound = shaderid;
		return 2;
	}
	return 1;
}
int shader_printProgramLogStatus(const int id){
	int blen= 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetProgramInfoLog(id, blen, 0, log);
		printf("program log: %s\n", log);
		free(log);
		return blen;
	}
	return FALSE;
}
int shader_printShaderLogStatus(const int id){
	int blen= 0;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &blen);
	if(blen > 1){
		GLchar *log = (GLchar *) malloc(blen);
		glGetShaderInfoLog(id, blen, 0, log);
		printf("shader log: %s\n", log);
		free(log);
		return blen;
	}
	return FALSE;
}

shader_t compileShader(const char * filename){
	shader_t s = {0};
	unsigned int filenamesize = strlen(filename);
	char * vertname = malloc(filenamesize + 6);
	char * fragname = malloc(filenamesize + 6);
	sprintf(vertname, "%s.vert", filename);
	sprintf(fragname, "%s.frag", filename);

	FILE *ff = fopen(fragname, "r");
	FILE *fv = fopen(vertname, "r");
	if(!ff || !fv){
		if(fv) fclose(fv);
		if(ff) fclose(ff);
		printf("shader file not avaliable %s\n", filename);
		return s;
	}
	fseek(ff, 0, SEEK_END);
	int flength = ftell(ff);
	rewind(ff);
	fseek(fv, 0, SEEK_END);
	int vlength = ftell(fv);
	rewind(fv);
	if(!vlength || !flength){
		if(fv) fclose(fv);
		if(ff) fclose(ff);
		return s;
	}
	char *vertstring = malloc(vlength+1);
//	vertstring[vlength] = 0;
	memset(vertstring, 0, vlength+1);
	fread(vertstring, 1, vlength, fv);
	char *fragstring = malloc(flength+1);
//	fragstring[flength] = 0;
	memset(fragstring, 0, flength+1);
	fread(fragstring, 1, flength, ff);
	fclose(fv);
	fclose(ff);

	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertid, 1, (const GLchar **) &vertstring, &vlength);
	glShaderSource(fragid, 1, (const GLchar **) &fragstring, &flength);
//	printf("vertex shader:\n%s\n", vertstring);
//	printf("fragment shader:\n%s\n", fragstring);
	if(vertstring) free(vertstring);
	if(fragstring) free(fragstring);

	glCompileShader(vertid);
	glCompileShader(fragid);
	int fail = 0;
	printf("shader ids %i, %i\n", vertid, fragid);
	printf("shader lengths %i, %i\n", vlength, flength);

	int status = 0;
	glGetShaderiv(fragid, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		fail = TRUE;
		printf("frag shader compile problem\n");
		shader_printShaderLogStatus(fragid);
	}
	glGetShaderiv(vertid, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		fail = TRUE;
		printf("vert shader compile problem\n");
		shader_printShaderLogStatus(vertid);
	}
	GLuint programid = glCreateProgram();
	glAttachShader(programid, vertid);
	glAttachShader(programid, fragid);
//	if(!fail){
		glBindFragDataLocation(programid, 0, "fragColor");
		glBindFragDataLocation(programid, 1, "normColor");
		glBindFragDataLocation(programid, 2, "specColor");

		glBindAttribLocation(programid, POSATTRIBLOC, "posattrib");
		glBindAttribLocation(programid, NORMATTRIBLOC, "normattrib");
		glBindAttribLocation(programid, TCATTRIBLOC, "tcattrib");
		glBindAttribLocation(programid, TANGENTATTRIBLOC, "tangentattrib");
//	}
	glLinkProgram(programid);
	glDeleteShader(vertid);
	glDeleteShader(fragid);
	glGetProgramiv(programid, GL_LINK_STATUS, &status);
	if(status == GL_FALSE) fail = TRUE;
	if(shader_printProgramLogStatus(programid) || fail){
		fail = TRUE;
	}

	if(fail) return s; //should delte... but whatever
	s.programid = programid;
	glUseProgram(programid);
	char * texstring = malloc(10);
	int i;
	for(i = 0; i < 16; i++){
		sprintf(texstring, "texture%i",i);
		s.texturepos[i] = glGetUniformLocation(programid, texstring);
		glUniform1i(s.texturepos[i], i);
	} free(texstring);

	s.unimat40 = glGetUniformLocation(programid, "unimat40");
	s.unimat41 = glGetUniformLocation(programid, "unimat41");
	printf("Uniform loc %i\n", s.unimat40);
	s.unifloat0 = glGetUniformLocation(programid, "unifloat0");
	s.univec40 = glGetUniformLocation(programid, "univec40");
	s.univec30 = glGetUniformLocation(programid, "univec30");
	s.univec20 = glGetUniformLocation(programid, "univec20");
	s.uniint0 = glGetUniformLocation(programid, "uniint0");
	s.uniscreensizefix = glGetUniformLocation(programid, "uniscreensizefix");
	s.uniscreentodepth = glGetUniformLocation(programid, "uniscreentodepth");
	s.uniblock0 = glGetUniformLocation(programid, "uniblock0");
	if(s.uniblock0 > -1) glUniformBlockBinding(programid, s.uniblock0, 0);
	s.uniblock1 = glGetUniformLocation(programid, "uniblock1");
	if(s.uniblock1 > -1) glUniformBlockBinding(programid, s.uniblock0, 1);
	printf("Shader %s compile successful\n", filename);

	return s;
}
