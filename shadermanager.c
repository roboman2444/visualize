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
	char * geomname = malloc(filenamesize + 6);
	char * fragname = malloc(filenamesize + 6);
	sprintf(vertname, "%s.vert", filename);
	sprintf(geomname, "%s.geom", filename);
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
	FILE *fg = fopen(geomname, "r");
	int glength = 0;
	if(fg){
		fseek(fg, 0, SEEK_END);
		glength = ftell(fg);
		rewind(fg);
//		if(!glength) fclose(fg);
	}
	char *vertstring = malloc(vlength+1);
//	vertstring[vlength] = 0;
	memset(vertstring, 0, vlength+1);
	fread(vertstring, 1, vlength, fv);
	if(fv)fclose(fv);
	char *fragstring = malloc(flength+1);
//	fragstring[flength] = 0;
	memset(fragstring, 0, flength+1);
	fread(fragstring, 1, flength, ff);
	if(ff)fclose(ff);
	char *geomstring = 0;
	if(glength){
		geomstring = malloc(glength+1);
		memset(geomstring, 0, glength+1);
		fread(geomstring, 1, glength, fg);
	}
	if(fg)fclose(fg);



	printf("shader lengths %i, %i, %i\n", vlength, flength, glength);


	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertid, 1, (const GLchar **) &vertstring, &vlength);
	glCompileShader(vertid);
//	printf("vert shader contents :\n%s\n\n\n\n", vertstring);
	if(vertstring) free(vertstring);

	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragid, 1, (const GLchar **) &fragstring, &flength);
	glCompileShader(fragid);
//	printf("frag shader contents :\n%s\n\n\n\n", fragstring);
	if(fragstring) free(fragstring);



	GLuint geomid = 0;
	if(glength) {
		geomid = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geomid, 1, (const GLchar **) &geomstring, &glength);
		glCompileShader(geomid);
	}
	if(geomstring) printf("Geometry shader length %i,contents :\n%s\n\n\n\n", glength ,geomstring);
	if(geomstring) free(geomstring);

	printf("shader ids %i, %i, %i\n", vertid, fragid, geomid);


	int fail = 0;

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
	if(glength){
		glGetShaderiv(geomid, GL_COMPILE_STATUS, &status);
		if(status == GL_FALSE){
			fail = TRUE;
			printf("geom shader compile problem\n");
			shader_printShaderLogStatus(geomid);
		}
	}
	GLuint programid = glCreateProgram();
	glAttachShader(programid, vertid);
	if(glength)glAttachShader(programid, geomid);
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
	if(glength)glDeleteShader(geomid);
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
	s.univec31 = glGetUniformLocation(programid, "univec31");
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
