#include <GLFW/glfw3.h>

#include "globaldefs.h"
#include "glfwmanager.h"

//temp
extern int glResizeViewports(int width, int height);


GLFWwindow * window;


const char ** glfw_extensions = 0;
unsigned int glfw_extensions_count = 0;


int glfwerror = 0;

int winwidth = 0;
int winheight = 0;

extern int otherinit(const unsigned int width, const unsigned int height, int second);



int glfw_resizeWindow(int width, int height, int bpp, int debugmode){
//	if(debugmode) console_printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(debugmode) printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(height <1) height =1;
	if(width <1) width =1;
	glfwSetWindowSize(window, width, height);
//	int r = glResizeViewports(width, height);
//	if(!r){
//		console_printf("ERROR -- GL framebuffers resize failed\n");
//		return FALSE;
//	}
	winwidth = width;
	winheight = height;
//	otherinit(width, height, 1);
	return TRUE;
}


void glfw_handleError(int error, const char * desc){
	//console_printf("ERROR -- GLFW %i-- %s\n", error, desc);
	printf("ERROR -- GLFW %i-- %s\n", error, desc);
	glfwerror = error;
}


int glfw_init(int width, int height, int bpp, int debugmode){
//	if(debugmode) console_printf("DEBUG -- GLFW window init\n");
	if(debugmode) printf("DEBUG -- GLFW window init\n");
	glfwSetErrorCallback(glfw_handleError);
	if(!glfwInit()){
		printf("ERROR -- GLFW init failed\n");
		return FALSE;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	window = glfwCreateWindow(width, height, "ballinengine", NULL, NULL);
	if(!window){
		glfwTerminate();
		printf("ERROR -- GLFW window creation failed\n");
		return FALSE;
	}
	glfwMakeContextCurrent(window);
	return glfw_resizeWindow(width, height, bpp, debugmode);
}

void glfw_swapBuffers(void){
	glfwSwapBuffers(window);
}

int glfw_checkEvent(void){
	//todo
	glfwPollEvents();
	if(glfwWindowShouldClose(window)){
		glfwTerminate();
		exit(0);
	}
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	if(iw != winwidth || ih != winheight) glfw_resizeWindow(iw, ih, 24, 0);
	return TRUE;
}


int glfw_shutdown(void){
	//TODO so much TODO
	printf("glfw shutting down\n");
	glfwTerminate();
	return TRUE;
}
