#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#define FOVgame 90
#define ScreenWidth 1024
#define ScreenHeight 768
#define PI 3.1415926

#define X_LIMIT 64
#define Z_LIMIT 64
#define Y_LIMIT 64
#define DRAW_DISTANCE 10

#define DEV_MODE
#define NO_VERBOSE

//#define SEED 123
static int SEED = 123;

const float rev_pi = 57.29578049;	//the number for 180 / pi
#define aspect_ratio ScreenWidth / ScreenHeight

#include<glad/glad.h>
#include<GLFW/glfw3.h>


using namespace std;

struct vec2;
struct vec3;
struct vec4;
struct mat4;
struct chunk;



struct chunk {
	int id = -1;
	int x, z;				//x = (int)camPos.x / 16
	short data[16][256][16];
	bool visibility[16][256][16];
	chunk* before;
	chunk* next;

	chunk(int _id) {
		id = _id;
	};

	chunk() {

	};
};

struct vec2 {
	float x, y;
	vec2(float _x, float _y) {
		x = _x;
		y = _y;
	};

	operator GLfloat* () {
		return &x;
	}

	vec2() {

	};

	float operator*(vec2 sec) {
		float result = 0;
		result += this->x * sec.x;
		result += this->y * sec.y;
		return result;
	}

	vec2 operator+(vec2 sec) {
		vec2 result;
		result.x = this->x + sec.x;
		result.y = this->y + sec.y;
		return result;
	}

	vec2 operator-(vec2 sec) {
		vec2 result;
		result.x = this->x - sec.x;
		result.y = this->y - sec.y;
		return result;
	}

	vec2 operator*(float sec) {
		vec2 result;
		result.x = this->x * sec;
		result.y = this->y * sec;
		return result;
	}
};

struct vec3 {
	float x,y,z;
	vec3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	};

	operator GLfloat* () {
		return &x;
	}

	vec3() {

	};

	float operator*(vec3 sec) {
		float result = 0;
		result += this->x * sec.x;
		result += this->y * sec.y;
		result += this->z * sec.z;
		return result;
	}

	vec3 operator+(vec3 sec) {
		vec3 result;
		result.x = this->x + sec.x;
		result.y = this->y + sec.y;
		result.z = this->z + sec.z;
		return result;
	}

	vec3 operator-(vec3 sec) {
		vec3 result;
		result.x = this->x - sec.x;
		result.y = this->y - sec.y;
		result.z = this->z - sec.z;
		return result;
	}

	vec3 operator*(float sec) {
		vec3 result;
		result.x = this->x * sec;
		result.y = this->y * sec;
		result.z = this->z * sec;
		return result;
	}
};

struct vec4 {
	float x,y,z,w;
	vec4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	};

	vec4() {
		x = y = z = w = 0;
	};

	operator GLfloat* () {
		return &x;
	}

	float operator*(vec4 sec) {
		float result = 0;
		result += this->x * sec.x;
		result += this->y * sec.y;
		result += this->z * sec.z;
		result += this->w * sec.w;
		return result;
	}
};

//Column-Major
struct mat4 {
	vec4 data[4];
	mat4(vec4 col_1, vec4 col_2, vec4 col_3, vec4 col_4) {
		data[0] = col_1;
		data[1] = col_2;
		data[2] = col_3;
		data[3] = col_4;
	};

	mat4() {	//default func

	};

	operator GLfloat*() {
		return &data[0].x;
	}

	mat4 operator*(mat4 sec) {
		mat4 result;
		for (int i = 0; i < 4; i++) {
			result.data[i].x = this->data[i] * vec4(sec.data[0].x, sec.data[1].x, sec.data[2].x, sec.data[3].x);
			result.data[i].y = this->data[i] * vec4(sec.data[0].y, sec.data[1].y, sec.data[2].y, sec.data[3].y);
			result.data[i].z = this->data[i] * vec4(sec.data[0].z, sec.data[1].z, sec.data[2].z, sec.data[3].z);
			result.data[i].w = this->data[i] * vec4(sec.data[0].w, sec.data[1].w, sec.data[2].w, sec.data[3].w);
		}
		return result;
	}
};

void InitWindow();
GLuint InitShaders();
mat4 translate4fv(float T_x, float T_y, float T_z);
mat4 frustum4fv(float left, float right, float bottom, float top, float near, float far);
mat4 projection4fv(float fovy, float aspect, float near, float far);
mat4 rotation4fv(float angle_x, float angle_y, float angle_z);
mat4 lookat4fv(float yaw, float pitch, float x, float y, float z);
void onUpdate();
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
vec3 cross(vec3 a, vec3 b);
float norm3fv(vec3 in);
float norm2fv(vec2 in);
float deg2rad(float DEG);
void drawBlock(int x, int y, int z, int id);
void generateWorldPerlin(int seed);
bool isCanBeSeen(int x, int y, int z);
void fillBlockTexture(int id, int sideOffsetX, int sideOffsetY, int topOffsetX, int topOffsetY, int bottomOffsetX, int bottomOffsetY);
void InitBlocks();
void LoadSave();
chunk generateChunk(int x, int z);
int heightRng(int x, int z);
chunk* getChunk(int x, int z);
void renderChunk(int x, int z);
void deleteList();
int getBlockID(int x, int y, int z);
bool isChunkExist(int x, int z);
void showFPS(GLFWwindow* window);
void addBlock(int x, int y, int z, int id);
void deleteBlock(int x, int y, int z);
void checkChunkVisibility(chunk* target);
void changeBlockVisibility(int x, int y, int z, bool vis);
bool isTransparent(int id);
void checkBlockVisibility(int x, int y, int z);
int perlin2d(int x, int z, int seed);
float smooth(float t);
void outputHeightMap(int x, int z);


#endif