#include <stdio.h>
#include <stdlib.h>
#include "globaldefs.h"
#include "matrixlib.h"

#include "camera.h"


#include "math.h"


vec3_t stockv_forward = {0.0, 0.0, 1.0};
vec3_t stockv_up = {0.0, 1.0, 0.0};
vec3_t stockv_right = {1.0, 0.0, 0.0};
void recalcViewMatrix(camera_t *v){
	Matrix4x4_CreateRotate(&v->view, v->angle[2], 0.0f, 0.0f, 1.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[1], 0.0f, 1.0f, 0.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[0], 1.0f, 0.0f, 0.0f);
	Matrix4x4_Transform(&v->view, stockv_forward, v->forward);
	Matrix4x4_Transform(&v->view, stockv_forward, v->up);
	Matrix4x4_Transform(&v->view, stockv_forward, v->right);
	Matrix4x4_ConcatTranslate(&v->view, -v->pos[0], -v->pos[1], -v->pos[2]);
}
void recalcProjectionMatrix(camera_t *v){
	double sine, cotangent, deltaZ;
	double radians = v->fov / 2.0 * M_PI / 180.0;
	deltaZ = v->far - v->near;
	sine = sin(radians);
	if((deltaZ == 0) || (sine == 0) || (v->aspect == 0)){
		return;
	}
	cotangent = cos(radians)/sine;
	v->proj.m[0][0] = cotangent / v->aspect;
	v->proj.m[1][1] = cotangent;
	v->proj.m[2][2] = -(v->far + v->near) / deltaZ;
	v->proj.m[2][3] = -1.0;
	v->proj.m[3][2] = -2.0 * v->far * v->near / deltaZ;
	v->proj.m[3][3] = 0.0;
}

camera_t createcam(void){
	camera_t v;
	v.fov = 90;
	v.near = 1.0f;
	v.far = 1000.0f;
	v.aspect = 1.0f;
	v.viewchanged = TRUE;
	v.projchanged = TRUE;
	int i;
	for(i = 0; i < 3; i++){
		v.pos[i] = 0.0f;
		v.angle[i] = 0.0f;
	}
	Matrix4x4_CreateIdentity(&v.view);
	Matrix4x4_CreateIdentity(&v.cam);
	Matrix4x4_CreateIdentity(&v.proj);
	Matrix4x4_CreateIdentity(&v.viewproj);
	return v;
}

int recalcCamera(camera_t *v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far){
	if(pos[0] != v->pos[0] ||
	   pos[1] != v->pos[1] ||
	   pos[2] != v->pos[2] ||
	   angle[0] != v->angle[0] ||
	   angle[1] != v->angle[1] ||
	   angle[2] != v->angle[2]){
		v->viewchanged = TRUE;
		v->pos[0] = pos[0];
		v->pos[1] = pos[1];
		v->pos[2] = pos[2];
		v->angle[0] = angle[0];
		v->angle[1] = angle[1];
		v->angle[2] = angle[2];
		recalcViewMatrix(v);
		Matrix4x4_CreateFromQuakeEntity(&v->cam, pos[0], pos[1], pos[2], angle[2], angle[1], angle[0], 1.0);
	}
	if(fov != v->fov || aspect != v->aspect || v->near != near || v->far != far){
		v->projchanged = TRUE;
		v->far = far;
		v->near = near;
		v->fov = fov;
		v->aspect = aspect;
		recalcProjectionMatrix(v);
	}
	if(v->viewchanged || v->projchanged){
		Matrix4x4_Concat(&v->viewproj, &v->proj, &v->view);
		v->viewchanged = FALSE;
		v->projchanged = FALSE;
		return TRUE;
	}
	return FALSE;
}
