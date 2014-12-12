#include <stdio.h>
#include <stdlib.h>
#include "globaldefs.h"
#include <GL/glew.h>
#include <GL/gl.h>

#include "texturemanager.h"
#include "framebuffermanager.h"

framebuffer_t currentfb;

char states_BindFramebuffer(framebuffer_t fb){
	char ret = 1;
	if(currentfb.id != fb.id){
		glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
		currentfb.id = fb.id;
		ret = 2;
	}
	if(currentfb.width != fb.width || currentfb.height != fb.height){
		glViewport(0, 0, fb.width, fb.height);
		currentfb.width = fb.width;
		currentfb.height = fb.height;
		ret = 3;
	}
	return ret;
}
