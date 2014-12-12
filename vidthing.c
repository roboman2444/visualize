#include "globaldefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "vidthing.h"
#include "types.h"



//start SDL stuff
#include <signal.h>
#include "SDL.h"
void quit_handler(int sig){
	SDL_Quit();
	exit(FALSE);
}
SDL_Surface *sdlsurf;
const SDL_VideoInfo *videoInfo;
int videoFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;
int initSDL(int width, int height){
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		printf("sdl failed to init %s\n", SDL_GetError());
		return FALSE;
	}
	videoInfo = SDL_GetVideoInfo();
	if(videoInfo->hw_available) videoFlags |= SDL_HWSURFACE;
	else			    videoFlags |= SDL_SWSURFACE;
	if(videoInfo->blit_hw)	    videoFlags |= SDL_HWACCEL;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	sdlsurf = SDL_SetVideoMode(width, height, 24, videoFlags);
	if(!sdlsurf){
		printf("Sdl failed to init, resize\n");
		return FALSE;
	}
	signal(SIGINT, &quit_handler);
	signal(SIGTERM, &quit_handler);
	signal(SIGQUIT, &quit_handler);
	return TRUE;
}
//end SDL stuff
//begin GLinit Stuff
#include <GL/glew.h>
#include <GL/gl.h>

int glInit(void){
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("error starting glew"); //todo get glew error
	}
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
//	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
		glClearDepth(1.0);
		glClearColor(0.0,0.0,0.0,0.0);

	return TRUE;
}

//end GLinit Stuff
//begin BASS init stuff
#include "bass.h"
HSTREAM bstream;

int bassinit(const char * filename){
	BASS_Init(-1, 44100, 0, NULL, NULL);
	bstream = BASS_StreamCreateFile(FALSE, filename, 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_PRESCAN);
	if(!bstream) return FALSE;
	return TRUE;
}

//end BASS init stuff
//begin model loading shit
#include "iqm.h"
typedef struct myvao_s {
	GLuint vaoid;
	GLuint posid;
	GLuint normid;
	GLuint tcid;
	GLuint tangentid;
	GLuint faceid;
	unsigned int numverts;
	unsigned int numfaces;
} myvao_t;
myvao_t currentvaobound;
char states_bindVao(myvao_t vb){
	if(vb.vaoid != currentvaobound.vaoid){
		glBindVertexArray(vb.vaoid);
		currentvaobound = vb;
		return 2;
	}
	return 1;
}
myvao_t loadiqmmeshes(const struct iqmheader hdr, unsigned char * buf){
	printf("iqmloadmeshes\n");
	int numfaces = hdr.num_triangles;
	int numverts = hdr.num_vertexes;
	float *pos = 0;
	float *norm = 0;
	float *texcoord = 0;
	float *tangent = 0;
	int i;
	struct iqmvertexarray *var = (struct iqmvertexarray *)&buf[hdr.ofs_vertexarrays];
	for(i = 0; i < (int) hdr.num_vertexarrays; i++){
		struct iqmvertexarray va = var[i];
		switch(va.type){
			case IQM_POSITION:	if(va.format != IQM_FLOAT || va.size == 3) pos = (float *)&buf[va.offset]; break;
			case IQM_NORMAL:	if(va.format != IQM_FLOAT || va.size == 3) norm = (float *)&buf[va.offset]; break;
			case IQM_TEXCOORD:	if(va.format != IQM_FLOAT || va.size == 2) texcoord = (float *)&buf[va.offset]; break;
			case IQM_TANGENT:	if(va.format != IQM_FLOAT || va.size == 4) tangent = (float *)&buf[va.offset]; break;
			default: break;
		}
	}
	myvao_t v = {0};
	if(!pos) return v;
	glGenVertexArrays(1, &v.vaoid);
	glBindVertexArray(v.vaoid);
	if(pos){
		glGenBuffers(1, &v.posid);
		glBindBuffer(GL_ARRAY_BUFFER, v.posid);
		glBufferData(GL_ARRAY_BUFFER, numverts * 3* sizeof(GLfloat), pos, GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSATTRIBLOC);
		glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
	}
	if(norm){
		glGenBuffers(1, &v.normid);
		glBindBuffer(GL_ARRAY_BUFFER, v.normid);
		glBufferData(GL_ARRAY_BUFFER, numverts * 3* sizeof(GLfloat), norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMATTRIBLOC);
		glVertexAttribPointer(NORMATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
	}
	if(texcoord){
		glGenBuffers(1, &v.tcid);
		glBindBuffer(GL_ARRAY_BUFFER, v.tcid);
		glBufferData(GL_ARRAY_BUFFER, numverts * 2* sizeof(GLfloat), texcoord, GL_STATIC_DRAW);
		glEnableVertexAttribArray(TCATTRIBLOC);
		glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *) 0);
	}
	if(tangent){
		glGenBuffers(1, &v.tangentid);
		glBindBuffer(GL_ARRAY_BUFFER, v.tangentid);
		glBufferData(GL_ARRAY_BUFFER, numverts * 4 * sizeof(GLfloat), tangent, GL_STATIC_DRAW);
		glEnableVertexAttribArray(TANGENTATTRIBLOC);
		glVertexAttribPointer(TANGENTATTRIBLOC, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) 0);
	}	GLuint *tris = (GLuint *) &buf[hdr.ofs_triangles];
	//flipping faces, temp fix
	for(i = 0; i <numfaces; i++){
		GLuint temp = tris[i*3];
		tris[i*3] = tris[(i*3)+1];
		tris[(i*3)+1] = temp;
	}
/*
	for(i = 0; i <numverts; i++){
		printf("vert%i: %f, %f, %f\n", i, pos[i*3], pos[(i*3)+1], pos[(i*3) +2]);
	}
*/
	glGenBuffers(1, &v.faceid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v.faceid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces * 3 * sizeof(GLuint), tris, GL_STATIC_DRAW);
	printf("loaded %i verts and %i faces", numverts, numfaces);
	v.numverts = numverts;
	v.numfaces = numfaces;
	//should have buffers n shit done now
	return v;
}
myvao_t loadModelIQM(const char * filename){
	myvao_t v = {0};
	FILE *f = fopen(filename, "rb");
	if(!f) return v;

	unsigned char *buf = NULL;
	struct iqmheader hdr;
	if(fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr) || memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic))) goto error;
	if(hdr.version != IQM_VERSION) goto error;
	if(hdr.filesize > (16<<20)) goto error;
	buf = malloc(hdr.filesize);
	if(!buf) goto error;
	memcpy(buf, &hdr, sizeof(hdr));
	if(fread(buf+sizeof(hdr), 1, hdr.filesize - sizeof(hdr), f) !=hdr.filesize -sizeof(hdr)) goto error;
	if(hdr.num_meshes <= 0) goto error;
	v = loadiqmmeshes(hdr, buf);
	if(!v.numverts)goto error;
	printf(" from model %s\n", filename);
	done:
	if(buf)free(buf);
	if(f)fclose(f);
	return v;
	error:
	printf("error loading model %s\n", filename);
	memset(&v, 0, sizeof(v));
	//todo error checking n shit
	goto done;

}
#include "texturemanager.h"
#include "shadermanager.h"
#include "framebuffermanager.h"
#include "matrixlib.h"
#include "camera.h"
#include "math.h"

shader_t fsquadshader;
shader_t bloomoutshader;
shader_t dofshader;
shader_t fsblurh;
shader_t fsblurv;
shader_t lensflare;
shader_t waveshader;
shader_t stdmodelshader;
shader_t lenscombine;
shader_t ssaoshader;
shader_t shadowshader;
shader_t lightsmodel;
myvao_t unitcube;
myvao_t fsquad;
myvao_t diamond;
myvao_t waveback;
myvao_t lightmodel;
myvao_t lightbulbmodel;
float *wavebuffer;
float *wavebuffert;
unsigned int wavex;
unsigned int wavey;
int waveneedpush = 0;
framebuffer_t screen;
framebuffer_t bloomv;
framebuffer_t bloomh;
framebuffer_t bloomout;
framebuffer_t ssaoout;
framebuffer_t dofout;
framebuffer_t fpscreen;
framebuffer_t lensout;
framebuffer_t lensoutblurv;
framebuffer_t lensoutblurh;
framebuffer_t shadow;
camera_t cam;
camera_t tempcam;
camera_t tempcam2;
camera_t shadowcam;
texture_t lenscolor;
texture_t lensdirt;
texture_t lensstar;
#define MAX_DIA 16
typedef struct diamond_s {
	float angle;
	float dist;
	float vel;
	matrix4x4_t m;
} diamond_t;


int numdiamonds = 0;
diamond_t diamondbuff[MAX_DIA];
int diatest = 0;

void diamondAdd(void){
	if(numdiamonds+1 >= MAX_DIA) return;
	if(numdiamonds > 0)diamondbuff[numdiamonds].angle = diamondbuff[numdiamonds-1].angle;
	else diamondbuff[numdiamonds].angle = 0.0;
	diamondbuff[numdiamonds].vel = 0.0;
	diamondbuff[numdiamonds].dist = 0.5;
//	if(diamondbuff[(numdiamonds - 1)].dist == 0.5) diamondbuff[numdiamonds].dist = 1.5;
//	diatest = !diatest;
	numdiamonds++;
}
void diamondRemove(void){
	if(numdiamonds > 0)numdiamonds--;
	int switcher = rand() % numdiamonds;
	int i;
	for(i = switcher; i < numdiamonds; i++){
		diamondbuff[i] = diamondbuff[i+1];
	}
}
void diamondTick(void){
	//accelerate
	int i;
	if(numdiamonds >1){
		for(i = 0; i < numdiamonds; i++){
			diamond_t *c = &diamondbuff[i];
			diamond_t *l = &diamondbuff[numdiamonds-1];
			if(i) l = &diamondbuff[i-1];
			diamond_t *r = &diamondbuff[0];
			if(i < numdiamonds-1) r = &diamondbuff[i+1];


			float distr = (c->angle - r->angle);
			distr += 360;
			distr = fmod(distr,360.0);
			if(distr > 180.0) distr = 360- distr;
			float stretch = distr  - (360.0 /numdiamonds);
			c->vel += stretch * 0.01;
			float distl = (c->angle - l->angle);
			distl += 360;
			distl = fmod(distl,360.0);
			if(distl > 180.0) distl = 360- distl;

			stretch = distl  - (360.0 /numdiamonds);
			c->vel -= stretch * 0.01;

		}
	}
	//dampen
	for(i = 0; i < numdiamonds; i++){
		diamondbuff[i].vel *= 0.9;
		diamondbuff[i].vel += 0.01;
	}
	//update pos and matrix
	for(i = 0; i < numdiamonds; i++){
		diamondbuff[i].angle += diamondbuff[i].vel;
		diamondbuff[i].angle  = fmod(diamondbuff[i].angle, 360.0);
		Matrix4x4_CreateRotate(&diamondbuff[i].m, diamondbuff[i].angle, 0.0, 0.0, 1.0);
		Matrix4x4_ConcatTranslate(&diamondbuff[i].m, 0.0, diamondbuff[i].dist, 0.5 );
//		Matrix4x4_ConcatTranslate(&diamondbuff[i].m, 0.0, 1.0, 0.0);
//		Matrix4x4_ConcatTranslate(&diamondbuff[i].m, 0.0, 1.0, 0.0);
		Matrix4x4_ConcatScale(&diamondbuff[i].m, 0.05);
	}
}
void diamondRender(camera_t *c){
	int i;
	states_useProgram(stdmodelshader.programid);
	states_bindVao(diamond);
	for(i = 0; i <numdiamonds; i++){
		matrix4x4_t out;
		Matrix4x4_Concat(&out, &c->viewproj, &diamondbuff[i].m);
		GLfloat mout[16];
		Matrix4x4_ToArrayFloatGL(&out, mout);
		glUniformMatrix4fv(stdmodelshader.unimat40, 1, GL_FALSE, mout);
		glDrawElements(GL_TRIANGLES, diamond.numfaces * 3, GL_UNSIGNED_INT, 0);
	}
}
void updateWaveBuffer(float * inbuffer){

	int ix, iy;
	for(iy = wavey; iy > 0; iy--){
	for(ix = wavex; ix > 0; ix--){
		wavebuffer[((((iy) * wavex) + ix) * 3) +2] =
		wavebuffer[((((iy-1) * wavex) + ix) * 3) +2];
	}}

	int i;
	for(i = 0; i < wavex; i++){
//		wavebuffert[(i*3)+2] = inbuffer[i] * 0.1;
		wavebuffer[(i*3)+2] = inbuffer[i] * sqrt(sqrt((float)i));
	}



	waveneedpush = TRUE;
}
void pushWaveBuffer(void){
	states_bindVao(waveback);
	glBindBuffer(GL_ARRAY_BUFFER, waveback.posid);
	glBufferData(GL_ARRAY_BUFFER, waveback.numverts * 3* sizeof(GLfloat), wavebuffer, GL_STREAM_DRAW);
	waveneedpush = FALSE;
}
void renderWave(camera_t *mycam, float smult){
		states_useProgram(waveshader.programid);
		GLfloat mout[16];
		Matrix4x4_ToArrayFloatGL(&mycam->viewproj, mout);
		glUniformMatrix4fv(waveshader.unimat40, 1, GL_FALSE, mout);
//		glUniform1f(waveshader.unifloat0, 1.0);
		glUniform1f(waveshader.unifloat0, smult);
		states_bindVao(waveback);
		if(waveneedpush) pushWaveBuffer();
		glDrawElements(GL_TRIANGLES, waveback.numfaces * 3, GL_UNSIGNED_INT, 0);
}
myvao_t createFSQuad(void){
	myvao_t v;
	unsigned int numverts = 4;
	GLfloat fsquadv[] = {-1.0, -1.0, 0.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 0.0};

	unsigned int numfaces = 2;
	GLuint fsquadf[] =  {0, 2, 1, 0, 3, 2};
	glGenVertexArrays(1, &v.vaoid);
	glBindVertexArray(v.vaoid);
	glGenBuffers(1, &v.posid);
	glBindBuffer(GL_ARRAY_BUFFER, v.posid);
	glBufferData(GL_ARRAY_BUFFER, numverts * 4* sizeof(GLfloat), fsquadv, GL_STREAM_DRAW);
	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) 0);
	glGenBuffers(1, &v.tcid);
	glBindBuffer(GL_ARRAY_BUFFER, v.tcid);
	glBufferData(GL_ARRAY_BUFFER, numverts * 4* sizeof(GLfloat), fsquadv, GL_STREAM_DRAW);
	glEnableVertexAttribArray(TCATTRIBLOC);
	glVertexAttribPointer(TCATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
	glGenBuffers(1, &v.faceid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v.faceid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces * 3 * sizeof(GLuint), fsquadf, GL_STREAM_DRAW);

	v.numverts = numverts;
	v.numfaces = numfaces;
	return v;
}
myvao_t createWaveBuffer(const int x, const int y, const float scalex, const float scaley){
	printf("y, %i\n", y);
	wavex = x;
	wavey = y;
	myvao_t v;
	unsigned int numverts = x * y;
	wavebuffer = malloc(numverts * 3 * sizeof(GLfloat));
	wavebuffert = malloc(numverts * 3 * sizeof(GLfloat));
	int ix;
	int iy;
	for(iy = 0; iy < y; iy++){
	for(ix = 0; ix < x; ix++){
		wavebuffer[((iy * x) + ix) * 3] = (((float) ix/(float)x) - 0.5) * scalex * 2.0;
		wavebuffer[(((iy * x) + ix) * 3)+  1] = (((float) iy/(float)y) - 0.5) * scaley * 2.0;
		wavebuffer[(((iy * x) + ix) * 3) + 2] = 0.0f;
//		wavebuffer[(((iy * x) + ix) * 3) + 2] = sin(iy /30.0);
	}}
	memcpy(wavebuffert, wavebuffer, numverts * 3 * sizeof(GLfloat));

	int fx = x-1;
	int fy = y-1;
	unsigned int numfaces = fy * fx * 2;
	GLuint * facebuffer = malloc(numfaces * 3 * sizeof(GLuint));
	for(iy = 0; iy < fy; iy++){
	for(ix = 0; ix < fx; ix++){
		unsigned int topleft = (iy * x) + ix;
		unsigned int topright = topleft + 1;
		unsigned int bottomright = topright + x;
		unsigned int bottomleft = topleft + x;
		GLuint *fb =&facebuffer[((iy *fx) +ix) * 6];
		fb[0] = bottomleft;
		fb[1] = topleft;
		fb[2] = topright;
		fb[3] = bottomleft;
		fb[4] = topright;
		fb[5] = bottomright;

	}}
	glGenVertexArrays(1, &v.vaoid);
	glBindVertexArray(v.vaoid);
	glGenBuffers(1, &v.posid);
	glBindBuffer(GL_ARRAY_BUFFER, v.posid);
	glBufferData(GL_ARRAY_BUFFER, numverts * 3* sizeof(GLfloat), wavebuffer, GL_STREAM_DRAW);
	glEnableVertexAttribArray(POSATTRIBLOC);
	glVertexAttribPointer(POSATTRIBLOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
	glGenBuffers(1, &v.faceid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v.faceid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces * 3 * sizeof(GLuint), facebuffer, GL_STREAM_DRAW);

	v.numverts = numverts;
	v.numfaces = numfaces;

	return v;
}
GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
matrix4x4_t shadowcorrect;// = {{{0.5, 0.0, 0.0, 0.5},{0.0, 0.5, 0.0, 0.5},{0.0, 0.0, 0.5, 0.5},{0.0, 0.0, 0.5, 1.0}}};

int otherinit(const unsigned int width, const unsigned int height){
	glEnable(GL_TEXTURE_2D);

/*	glGenFramebuffers(1, &shadow.id);
	shadow.width = 512;
	shadow.height = 512;
	states_BindFramebuffer(shadow);
	glGenRenderbuffers(1, &shadow.rbid);
	glBindRenderbuffer(GL_RENDERBUFFER, shadow.rbid);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 1, GL_DEPTH24_STENCIL8, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, shadow.rbid);
	//create texture here
	glGenTextures(1, &shadow.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, shadow.textureid[0].id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_R32F, GL_UNSIGNED_BYTE, NULL);
	shadow.textureid[0].width = width;
	shadow.textureid[0].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, buffers[0], GL_TEXTURE_2D, shadow.textureid[0].id, 0);
*/
	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &fpscreen.id);
	fpscreen.width = width;
	fpscreen.height = height;
	states_BindFramebuffer(fpscreen);
	glBindFramebuffer(GL_FRAMEBUFFER, fpscreen.id);
	glGenRenderbuffers(1, &fpscreen.rbid);
	glBindRenderbuffer(GL_RENDERBUFFER, fpscreen.rbid);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fpscreen.rbid);
	//create texture here
	glGenTextures(1, &fpscreen.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, fpscreen.textureid[0].id);
	fpscreen.textureid[0].width = width;
	fpscreen.textureid[0].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glGenTextures(1, &fpscreen.textureid[1].id);
	glBindTexture(GL_TEXTURE_2D, fpscreen.textureid[1].id);
	fpscreen.textureid[1].width = width;
	fpscreen.textureid[1].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

	glDrawBuffers(2, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fpscreen.textureid[0].id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fpscreen.textureid[1].id, 0);
	printf("framebuffer %i texture %i\n", fpscreen.id, fpscreen.textureid[0].id);





	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &lensout.id);
	lensout.width = width/8;
	lensout.height = height/8;
	states_BindFramebuffer(lensout);
	glBindFramebuffer(GL_FRAMEBUFFER, lensout.id);
//	glGenRenderbuffers(1, &lensout.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, lensout.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width/4, height/4);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, lensout.rbid);
	//create texture here
	glGenTextures(1, &lensout.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, lensout.textureid[0].id);
	lensout.textureid[0].width = width/8;
	lensout.textureid[0].height = height/8;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width/8, height/8, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lensout.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", lensout.id, lensout.textureid[0].id);







	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &ssaoout.id);
	ssaoout.width = width;
	ssaoout.height = height;
	states_BindFramebuffer(ssaoout);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoout.id);
//	glGenRenderbuffers(1, &ssaoout.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, ssaoout.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ssaoout.rbid);
	//create texture here
	glGenTextures(1, &ssaoout.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, ssaoout.textureid[0].id);
	ssaoout.textureid[0].width = width;
	ssaoout.textureid[0].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoout.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", ssaoout.id, ssaoout.textureid[0].id);






	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &bloomout.id);
	bloomout.width = width;
	bloomout.height = height;
	states_BindFramebuffer(bloomout);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomout.id);
//	glGenRenderbuffers(1, &bloomout.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, bloomout.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, bloomout.rbid);
	//create texture here
	glGenTextures(1, &bloomout.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, bloomout.textureid[0].id);
	bloomout.textureid[0].width = width;
	bloomout.textureid[0].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomout.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", bloomout.id, bloomout.textureid[0].id);




	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &bloomv.id);
	bloomv.width = width/4;
	bloomv.height = height/4;
	states_BindFramebuffer(bloomv);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomv.id);
//	glGenRenderbuffers(1, &bloomv.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, bloomv.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width/4, height/4);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, bloomv.rbid);
	//create texture here
	glGenTextures(1, &bloomv.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, bloomv.textureid[0].id);
	bloomv.textureid[0].width = width/4;
	bloomv.textureid[0].height = height/4;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width/4, height/4, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomv.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", bloomv.id, bloomv.textureid[0].id);

	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &bloomh.id);
	bloomh.width = width/4;
	bloomh.height = height/4;
	states_BindFramebuffer(bloomh);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomh.id);
//	glGenRenderbuffers(1, &bloomh.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, bloomh.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width/4, height/4);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, bloomh.rbid);
	//create texture here
	glGenTextures(1, &bloomh.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, bloomh.textureid[0].id);
	bloomh.textureid[0].width = width/4;
	bloomh.textureid[0].height = height/4;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width/4, height/4, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomh.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", bloomh.id, bloomh.textureid[0].id);





	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &dofout.id);
	dofout.width = width;
	dofout.height = height;
	states_BindFramebuffer(dofout);
	glBindFramebuffer(GL_FRAMEBUFFER, dofout.id);
//	glGenRenderbuffers(1, &dofout.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, dofout.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, dofout.rbid);
	//create texture here
	glGenTextures(1, &dofout.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, dofout.textureid[0].id);
	dofout.textureid[0].width = width;
	dofout.textureid[0].height = height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dofout.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", dofout.id, dofout.textureid[0].id);


	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &shadow.id);
	shadow.width = 512;
	shadow.height = 512;
	states_BindFramebuffer(shadow);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.id);
	glGenRenderbuffers(1, &shadow.rbid);
	glBindRenderbuffer(GL_RENDERBUFFER, shadow.rbid);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, shadow.rbid);
	//create texture here
	glGenTextures(1, &shadow.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, shadow.textureid[0].id);
	shadow.textureid[0].width = 512;
	shadow.textureid[0].height = 512;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", shadow.id, shadow.textureid[0].id);


	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &lensoutblurh.id);
	lensoutblurh.width = width/8;
	lensoutblurh.height = height/8;
	states_BindFramebuffer(lensoutblurh);
	glBindFramebuffer(GL_FRAMEBUFFER, lensoutblurh.id);
	glGenRenderbuffers(1, &lensoutblurh.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, lensoutblurh.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width/8, height/8);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, lensoutblurh.rbid);
	//create texture here
	glGenTextures(1, &lensoutblurh.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, lensoutblurh.textureid[0].id);
	lensoutblurh.textureid[0].width = width/8;
	lensoutblurh.textureid[0].height = height/8;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width/8, height/8, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lensoutblurh.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", lensoutblurh.id, lensoutblurh.textureid[0].id);

	states_BindFramebuffer(screen);
	glGenFramebuffers(1, &lensoutblurv.id);
	lensoutblurv.width = width/8;
	lensoutblurv.height = height/8;
	states_BindFramebuffer(lensoutblurv);
	glBindFramebuffer(GL_FRAMEBUFFER, lensoutblurv.id);
//	glGenRenderbuffers(1, &lensoutblurv.rbid);
//	glBindRenderbuffer(GL_RENDERBUFFER, lensoutblurv.rbid);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width/8, height/8);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, lensoutblurv.rbid);
	//create texture here
	glGenTextures(1, &lensoutblurv.textureid[0].id);
	glBindTexture(GL_TEXTURE_2D, lensoutblurv.textureid[0].id);
	lensoutblurv.textureid[0].width = width/8;
	lensoutblurv.textureid[0].height = height/8;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width/8, height/8, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glDrawBuffers(1, buffers);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lensoutblurv.textureid[0].id, 0);
	printf("framebuffer %i texture %i\n", lensoutblurv.id, lensoutblurv.textureid[0].id);


	screen.width = width;
	screen.height = height;
	screen.id = 0;
	shadowshader = compileShader("shadow");
	ssaoshader = compileShader("ssao");
	dofshader = compileShader("dof");
	fsquadshader = compileShader("fsquad");
	bloomoutshader = compileShader("bloomout");
	lenscombine = compileShader("lensout");
	fsblurv = compileShader("fsblurv");
	fsblurh = compileShader("fsblurh");
	stdmodelshader = compileShader("stdmodel");
	lensflare = compileShader("lensflare");
	lightsmodel = compileShader("lightsmodel");
	waveshader = compileShader("waveback");
	unitcube = loadModelIQM("unitcube.iqm");
	diamond = loadModelIQM("diamond.iqm");
	lightmodel = loadModelIQM("light.iqm");
	lightbulbmodel = loadModelIQM("lightbulb.iqm");

	lenscolor = loadtexture("./lenscolor.png");
	lensdirt = loadtexture("./lensdirt.png");
	lensstar = loadtexture("./lensstar.png");
	fsquad = createFSQuad();
	waveback = createWaveBuffer(128, 128, 2.0, 2.0);
	cam = createcam();
	shadowcam = createcam();
	vec3_t newpos = {0.0, 0.0, 4.0};
	shadowcam.angle[2] = 0.0;
	tempcam = cam;
	tempcam.near = 0.1f;
	tempcam.aspect = (float)width/(float)height;
//	tempcam.pos[2] = 0.75;
	tempcam.pos[1] = 0.0;
	tempcam.pos[2] = 3.0;
	tempcam.angle[1] = 180.0;
	diamondAdd();
	diamondAdd();
	diamondAdd();
	diamondAdd();
	diamondAdd();

	Matrix4x4_CreateIdentity(&shadowcorrect);
	shadowcorrect.m[0][0] = 0.5;
	shadowcorrect.m[1][1] = 0.5;
	shadowcorrect.m[2][2] = 0.5;
	shadowcorrect.m[3][3] = 1.0;
	shadowcorrect.m[3][0] = 0.5;
	shadowcorrect.m[3][1] = 0.5;
	shadowcorrect.m[3][2] = 0.39;
//	shadowcorrect.m[3][2] = 0.5;

	glBlendFunc(GL_ONE, GL_ONE);

	recalcCamera(&cam, tempcam.pos, tempcam.angle, tempcam.fov, tempcam.aspect, tempcam.near, tempcam.far);
	recalcCamera(&shadowcam, newpos, shadowcam.angle, 65.0, 1.0, tempcam.near, tempcam.far);
	tempcam2 = shadowcam;
	return TRUE;
}


//end model loading shit

int main(int argc, char ** argv){
		float scount = 0.0;
	int width = 1280;
	int height = 720;
	char * filename = "./song.mp3";
	if(argc < 2){
		printf("no file dumbass, using default ./song.mp3\n");
	} else {
		filename = argv[1];
	}
	//init bass
	if(!bassinit(filename)){
		printf("file %s not found or some other libbass error, try again\n", filename);
		return TRUE;
	}
	//init SDL DONE
	initSDL(width,height);
	//init other shit
	//init GL
	glInit();

	otherinit(width, height);
	//play song
	BASS_ChannelPlay(bstream, FALSE);
	float bpm = 160.0;
	BASS_ChannelGetAttribute(bstream, BASS_ATTRIB_MUSIC_BPM, &bpm);
	float bpt = bpm/60000.0;
	printf("bpm is %f,%f\n", bpm, bpt);
	float tpb = 1.0/bpt;
	//main loop
	unsigned int t, to, framecount = 0, timesincelastfpsupdate = 0;
	unsigned int accum = 0;
	to = SDL_GetTicks();
//	float lowp = 0.2;
//	float highp = 0.4;
//	char ishigh = FALSE;
	srand(to);
	float bpmoffset = 0.0;
	float bpmaccum = bpmoffset;
	float automax = -2.0;
	float smult = 0.0;
	float distthing = 2;
	float myangle = tempcam.angle[1];
	while(TRUE){
		t = SDL_GetTicks();
		unsigned int delta = t-to;
		to = t;
		timesincelastfpsupdate+=delta;
		if(timesincelastfpsupdate > 5000){
			printf("%f fps\n", (float) framecount * 1000.0/(float)timesincelastfpsupdate);
			timesincelastfpsupdate-=5000;
			framecount = 0;
		}
		#define GCTIMESTEP 10
		accum += delta;
		bpmaccum += delta;
		while(accum > GCTIMESTEP){

			float fftdata[256];
			BASS_ChannelGetData(bstream, fftdata, BASS_DATA_FFT512);
			int i;
			float cool =0.0;
			for(i = 0; i < 15; i++){
				if(automax < 1.0)automax += 0.0005 * cool;
				cool+= fftdata[i];
			}
			float cool2 =0.0;
			for(i = 32; i < 48; i++){
				cool2+= fftdata[i];
			}
			if(automax >= 1.0)myangle += pow(cool * automax, 5)/5.0;
			tempcam.angle[2] *= (1.0 - (0.01*automax));
			if(automax >= 0.0) myangle *= (1.0 - (0.01 * automax));
			if(automax >= 1.0){
				tempcam.angle[2] += pow(cool2* 5.0 * automax, 5)/30.0;
			}
			if(automax > -1.0 && automax < 0.0){
				smult = automax+1.0;
				distthing += 0.001;
			}
			myangle = fmod(myangle, 360.0);
			if(automax > 0.0) tempcam.angle[1] = myangle - (45.0 * automax);


			tempcam.angle[1] = fmod(tempcam.angle[1], 360.0);
			tempcam.angle[2] = fmod(tempcam.angle[2], 360.0);
//			tempcam.angle[1] += 0.5;
			tempcam.pos[0] = sin(tempcam.angle[1] * (-M_PI / 180.0)) * distthing;
			tempcam.pos[2] = cos(tempcam.angle[1] * (-M_PI / 180.0)) *distthing;
			recalcCamera(&cam, tempcam.pos, tempcam.angle, tempcam.fov, tempcam.aspect, tempcam.near, tempcam.far);


			if(automax >= 1.0){
				scount += 0.01;
				scount = fmod(scount, M_PI*2.0);
				tempcam2.angle[1] = sin(scount) * 70;
//				printf("scount = %f\n", scount);
				tempcam2.angle[1] = fmod(tempcam2.angle[1], 360.0);
				tempcam2.angle[2] = fmod(tempcam2.angle[0], 360.0);
//				tempcam.angle[1] += 0.5;
				tempcam2.pos[0] = sin(tempcam2.angle[1] * (-M_PI / 180.0)) *4.0;
				tempcam2.pos[2] = cos(tempcam2.angle[1] * (-M_PI / 180.0)) *4.0;
				recalcCamera(&shadowcam, tempcam2.pos, tempcam2.angle, tempcam2.fov, tempcam2.aspect, tempcam2.near, tempcam2.far);
			}

		/*
			if(!ishigh){
				if(cool > highp){
					ishigh = TRUE;
				}
				if(lowp > 0.1){
					lowp-= 0.01;
					highp -= 0.01;
				}

			}else{
				if(lowp < 0.4){
					lowp+= 0.01;
					highp += 0.01;
				}
			 	if(cool <lowp) ishigh = FALSE;
			}
		*/
			if(bpmaccum > tpb){
				bpmaccum -=tpb;
//				if(cool > 0.001){
					if(numdiamonds < 4) diamondAdd();
					else if(numdiamonds == MAX_DIA) diamondRemove();
					else {
						float coolthing = (numdiamonds*2) / MAX_DIA;
						if(rand() / (double)RAND_MAX > coolthing) diamondAdd();
						else diamondRemove();
					}

//				}
			}
			updateWaveBuffer(fftdata);
			diamondTick();
			//whatever
			accum-=GCTIMESTEP;
		}
//		nope
		glEnable(GL_DEPTH_TEST);
		int i;
		GLfloat mout[16];
		states_BindFramebuffer(shadow);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		states_useProgram(shadowshader.programid);
		glUniform1f(shadowshader.unifloat0, 1.0);
		states_bindVao(diamond);
		for(i = 0; i <numdiamonds; i++){
			matrix4x4_t out;
			Matrix4x4_Concat(&out, &shadowcam.viewproj, &diamondbuff[i].m);
			Matrix4x4_ToArrayFloatGL(&out, mout);
			glUniformMatrix4fv(shadowshader.unimat40, 1, GL_FALSE, mout);
			glDrawElements(GL_TRIANGLES, diamond.numfaces * 3, GL_UNSIGNED_INT, 0);
		}
		Matrix4x4_ToArrayFloatGL(&shadowcam.viewproj, mout);
		glUniformMatrix4fv(shadowshader.unimat40, 1, GL_FALSE, mout);
		glUniform1f(shadowshader.unifloat0, smult);
		states_bindVao(waveback);
		if(waveneedpush) pushWaveBuffer();
		glDrawElements(GL_TRIANGLES, waveback.numfaces * 3, GL_UNSIGNED_INT, 0);




		states_BindFramebuffer(fpscreen);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadow.textureid[0].id);

		states_useProgram(stdmodelshader.programid);
		matrix4x4_t sout;
//		Matrix4x4_Concat(&sout, &shadowcam.viewproj, &shadowcorrect);
		Matrix4x4_Concat(&sout, &shadowcorrect, &shadowcam.viewproj);
		GLfloat smout[16];
//		Matrix4x4_ToArrayFloatGL(&shadowcam.viewproj, smout);
		Matrix4x4_ToArrayFloatGL(&sout, smout);
		glUniformMatrix4fv(stdmodelshader.unimat41, 1, GL_FALSE, smout);
		states_bindVao(diamond);
		for(i = 0; i <numdiamonds; i++){
			matrix4x4_t out;
			Matrix4x4_Concat(&out, &cam.viewproj, &diamondbuff[i].m);
			GLfloat mout[16];
			Matrix4x4_ToArrayFloatGL(&out, mout);
			glUniformMatrix4fv(stdmodelshader.unimat40, 1, GL_FALSE, mout);
			glDrawElements(GL_TRIANGLES, diamond.numfaces * 3, GL_UNSIGNED_INT, 0);
		}


		matrix4x4_t out;
		Matrix4x4_Concat(&out, &cam.viewproj, &shadowcam.cam);
		Matrix4x4_ToArrayFloatGL(&out, mout);
		glUniformMatrix4fv(stdmodelshader.unimat40, 1, GL_FALSE, mout);
		states_bindVao(lightbulbmodel);
		glDrawElements(GL_TRIANGLES, lightbulbmodel.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_useProgram(lightsmodel.programid);
		glUniformMatrix4fv(lightsmodel.unimat40, 1, GL_FALSE, mout);
		states_bindVao(lightmodel);
		glDrawElements(GL_TRIANGLES, lightmodel.numfaces * 3, GL_UNSIGNED_INT, 0);


		states_useProgram(waveshader.programid);
		glUniformMatrix4fv(waveshader.unimat41, 1, GL_FALSE, smout);
		Matrix4x4_ToArrayFloatGL(&cam.viewproj, mout);
		glUniformMatrix4fv(waveshader.unimat40, 1, GL_FALSE, mout);
//		glUniform1f(waveshader.unifloat0, 1.0);
		glUniform1f(waveshader.unifloat0, smult);
		states_bindVao(waveback);
		if(waveneedpush) pushWaveBuffer();
		glDrawElements(GL_TRIANGLES, waveback.numfaces * 3, GL_UNSIGNED_INT, 0);


		glDisable(GL_DEPTH_TEST);
//		glEnable(GL_BLEND);

		states_BindFramebuffer(ssaoout);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fpscreen.textureid[1].id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fpscreen.textureid[0].id);
		states_bindVao(fsquad);
		states_useProgram(ssaoshader.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_BindFramebuffer(bloomv);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoout.textureid[0].id);
		states_useProgram(fsblurv.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_BindFramebuffer(bloomh);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bloomv.textureid[0].id);
		states_useProgram(fsblurh.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_BindFramebuffer(bloomout);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloomh.textureid[0].id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoout.textureid[0].id);
		states_useProgram(bloomoutshader.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);


		states_BindFramebuffer(lensout);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glBindTexture(GL_TEXTURE_2D, dofout.textureid[0].id);
//		glBindTexture(GL_TEXTURE_2D, ssaoout.textureid[0].id);
		glBindTexture(GL_TEXTURE_2D, bloomout.textureid[0].id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lenscolor.id);
		glActiveTexture(GL_TEXTURE2);
		states_useProgram(lensflare.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_BindFramebuffer(lensoutblurv);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lensout.textureid[0].id);
		states_useProgram(fsblurv.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

		states_BindFramebuffer(lensoutblurh);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lensoutblurv.textureid[0].id);
		states_useProgram(fsblurh.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);

/*
		states_BindFramebuffer(dofout);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bloomout.textureid[0].id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fpscreen.textureid[1].id);
		states_bindVao(fsquad);
		states_useProgram(dofshader.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);
*/

		states_BindFramebuffer(screen);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glBindTexture(GL_TEXTURE_2D, dofout.textureid[0].id);
//		glBindTexture(GL_TEXTURE_2D, ssaoout.textureid[0].id);
		glBindTexture(GL_TEXTURE_2D, bloomout.textureid[0].id);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, lensdirt.id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lensoutblurh.textureid[0].id);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, lensstar.id);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, bloomh.textureid[0].id);
		states_useProgram(lenscombine.programid);
		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE0);

//		states_useProgram(fsquadshader.programid);
//		glDrawElements(GL_TRIANGLES, fsquad.numfaces * 3, GL_UNSIGNED_INT, 0);
		glDisable(GL_BLEND);
		SDL_GL_SwapBuffers();
		framecount++;
	}
	return FALSE;
}

