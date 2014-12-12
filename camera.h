#ifndef CAMERAH
#define CAMERAH

typedef struct camera_s {
	vec3_t pos;
	vec3_t angle;
	vec3_t forward;
	vec3_t right;
	vec3_t up;

	matrix4x4_t cam;
	matrix4x4_t view;
	matrix4x4_t proj;
	matrix4x4_t viewproj;
	float fov;
	float near;
	float far;
	float aspect;
	char viewchanged;
	char projchanged;
}camera_t;


camera_t createcam(void);
int recalcCamera(camera_t *v, vec3_t pos, vec3_t angle, float fov, float aspect, float near, float far);

#endif
