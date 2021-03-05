#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<game.h>
#include<stb_image.h>
#include<time.h>
#include"omp.h"
#include<sstream>
#include<irrKlang.h>
#include<PerlinNoise.h>
#include<ppm.h>

#include<iostream>
using namespace std;

extern GLFWwindow* window;

short playerInventory[41] = { 0, 1, 2, 3, 4, 5, 7, 12, 14, 17, 18 };		//not using 0
int currentItemSlot = 1;
vec3 camPos;
float camYaw = 0.01;	//in degree
float camPitch = 0.01;
float mouse_sensitivity = 0.05;
int FOV_realtime = FOVgame;

int levelMap[X_LIMIT][Y_LIMIT][Z_LIMIT];

extern float test[200];

GLfloat vertex_positions[50][144];
GLfloat texture_positions[50][72];

chunk firstchunk;	//first chunk generated.(begining of the list)
chunk *lastChunkPtr;

irrklang::ISoundEngine* soundEngine;

int framecount = 0;
double lastTime = 0;
int main()
{

	PerlinNoise pn(SEED);
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++)
			cout << 10 + 50 * pn.noise(i * 16, k * 16, 0.8) << ' ';
		cout << endl;

	}
	//cin >> SEED;
	InitBlocks();
	omp_set_num_threads(8);
	firstchunk = generateChunk(0, 0);
	firstchunk.id = 0;
	firstchunk.before = &firstchunk;
	//lastChunkPtr = &firstchunk;
	firstchunk.next = &firstchunk;
	InitWindow();
	//LoadSave();
	cout << firstchunk.x << firstchunk.z << endl;


	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLuint program = InitShaders();
	camPos.x = 0;
	camPos.y = 90;
	camPos.z = 0;

	//generateWorldPerlin(SEED);


	static const GLfloat vanilla[] = { 0.5f, 0.8f, 1.0f, 1.0f };

	vec4 test(0.0f, 0.25f, 0.25f, 1.0f);

	vec4 test_1(1.0, 0, 0, 0);
	vec4 test_2(0, 1.0, 0, 0);
	vec4 test_3(0, 0, 1.0, 0);
	vec4 test_4(0, 0, 4.0, 1.0);
	mat4 test_mat(test_1, test_2, test_3, test_4);
	//mat4 proj_mat = projection4fv(FOVgame, aspect_ratio, 0.1, 1000);

	mat4 rot_mat = rotation4fv(camPitch, 45, camYaw);
	//test_mat = rot_mat * test_mat;

	extern GLuint mv_location;
	extern GLuint proj_location;

	while (!glfwWindowShouldClose(window))
	{
		//cout << getBlockID(-31, 34, 23) << endl;
		//cout << getBlockID(8, 15, 21);
		showFPS(window);
		//cout << &firstchunk << firstchunk.next <<  endl;
		glClearBufferfv(GL_COLOR, 0, vanilla);
		glClear(GL_DEPTH_BUFFER_BIT);


		//mat4 fin_mat = translate4fv(-camPos.x, -camPos.y, -camPos.z);
		//mat4 fin_mat = translate4fv(camPos.x, camPos.y, camPos.z);
		
		
		glUseProgram(program);
		onUpdate();

		for (int i = 0; i < DRAW_DISTANCE; i++)
			for (int j = 0; j < DRAW_DISTANCE; j++) {
				renderChunk(camPos.x / 16 - DRAW_DISTANCE / 2 + i, camPos.z / 16 - DRAW_DISTANCE / 2 + j);
			}
		//cout << "second chunk id: " << firstchunk.next->id << endl;
		//renderChunk(0, 0);

		//renderChunk(-1, -1);
		//renderChunk(-1, 0);
		//renderChunk(-1, 1); 
		//renderChunk(0, -1);
		//renderChunk(0, 0);
		//renderChunk(0, 1);
		//renderChunk(1, -1);
		//renderChunk(1, 0);
		//renderChunk(1, 1);

		//glVertexAttrib4fv(0, test);
		//glPointSize(5.0f);

		//drawBlock(0, 0, 1, 2);					//old draw
		//for (int x = 0; x <= 50; x++)
		//	for (int z = 0; z <= 50; z++)
		//		for (int y = 0; y <= 30; y++)
		//			//if (CanBeSeen[x][y][z])
		//				drawBlock(x, y, z, y % 25 + 1);
		//if(CanBeSeen[5][18][5])
		//	drawBlock(5, 18, 5, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//FOV_realtime = FOVgame;		//revert FOV when releasing SPRINT

		glfwSwapBuffers(window);
		glfwPollEvents();


	}
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 10; j++)
			cout << perlin2d(i, j, SEED) << ' ';
		cout << endl;
	}


	//deleteList();

	return 0;
}