#define STB_IMAGE_IMPLEMENTATION

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb_image.h>
#include<game.h>
#include<cmath>
#include<sstream>
#include<irrKlang.h>

GLFWwindow* window;
GLuint vao;

GLuint proj_location;
GLuint mv_location;
GLuint shadow_location;

extern float camYaw, camPitch, mouse_sensitivity;
extern GLfloat vertex_positions[50][144];
extern GLfloat texture_positions[50][72];

//vec3 camAngle;
//vec3 playerPos;


void InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1024, 768, "Hello World", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//setting up mouse
	glfwSetCursorPosCallback(window, mouse_callback);

	extern irrklang::ISoundEngine* soundEngine;
	soundEngine = irrklang::createIrrKlangDevice();
	//soundEngine->play2D("18 - Sweden.mp3", GL_TRUE);
}

//enabling all the things for rendering
GLuint InitShaders() {
	extern const GLchar *vss;
	extern const GLchar *fss;
	//std::cout << vss;

	GLuint program = glCreateProgram();
	//std::cout << "PROGRAM:" << (int)program << std::endl;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vss, NULL);
	glCompileShader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fss, NULL);
	glCompileShader(fs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	//glUseProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	extern GLfloat vertex_positions[50][144];
	extern GLfloat texture_positions[50][72];
	GLuint v_buffer;
	glCreateBuffers(1, &v_buffer);
	glNamedBufferStorage(v_buffer, sizeof(vertex_positions), vertex_positions, GL_MAP_WRITE_BIT);
	glVertexArrayVertexBuffer(vao, 0, v_buffer, 0, sizeof(vec4));
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 0);

	GLuint t_buffer;	//stores texture coordinate
	glCreateBuffers(1, &t_buffer);
	glNamedBufferStorage(t_buffer, sizeof(texture_positions), texture_positions, GL_MAP_WRITE_BIT);
	glVertexArrayVertexBuffer(vao, 1, t_buffer, 0, 8);	//8 is the size of vec2
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 1);





	//setting up shadow buffer
	GLuint shadow_buffer;
	GLuint shadow_tex;
	glGenFramebuffers(1, &shadow_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
	glGenTextures(1, &shadow_tex);
	glBindTexture(GL_TEXTURE_2D, shadow_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, 4096, 4096);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);




	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("terrain.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	
	
	proj_location = glGetUniformLocation(program, "proj_matrix");
	mv_location = glGetUniformLocation(program, "mv_matrix");

	shadow_location = glGetUniformLocation(program, "shadow_matrix");
	//cout << "Location: " << mv_location << endl;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//line mode for debugging purpose

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);

	return program;
}

float deg2rad(float DEG) {
	return DEG / rev_pi;
}

float norm3fv(vec3 in) {
	return sqrtf(in.x * in.x + in.y * in.y + in.z * in.z);
}

float norm2fv(vec2 in) {
	return sqrtf(in.x * in.x + in.y * in.y);
}

float smooth(float t) {		//perlin2d interpolation func
	return 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;		//6t^5 - 15t^4 + 10t^3
}

vec3 cross(vec3 a, vec3 b) {
	vec3 result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;

	return result;
}

mat4 translate4fv(float T_x, float T_y, float T_z) {
	mat4 result;
	result.data[0].x = 1.0;
	result.data[1].y = 1.0;
	result.data[2].z = 1.0;
	result.data[3].w = 1.0;		//making it	a identity matrix

	result.data[3].x = T_x;
	result.data[3].y = T_y;
	result.data[3].z = T_z;

	return result;
}

mat4 frustum4fv(float left, float right, float bottom, float top, float near, float far) {
	vec4 dat[4];

	dat[0].x = (2.0 * near) / (right - left);
	dat[1].y = (2.0 * near) / (top - bottom);
	dat[2].x = (right + left) / (right - left);
	dat[2].y = (top + bottom) / (top - bottom);
	dat[2].z = (near + far) / (near - far);
	dat[2].w = -1.0;
	dat[3].z = (2.0 * near * far) / (near - far);

	return mat4(dat[0], dat[1], dat[2], dat[3]);
}

mat4 projection4fv(float fovy, float aspect, float near, float far) {
	float top, bottom, left, right;
	top = near * tanf(deg2rad(fovy) / 2);
	bottom = -top;
	right = top * aspect;
	left = -right;

	return frustum4fv(left, right, bottom, top, near, far);
}

mat4 rotation4fv(float angle_x, float angle_y, float angle_z) {
	vec4 data[4];

	angle_x = deg2rad(angle_x);
	angle_y = deg2rad(angle_y);
	angle_z = deg2rad(angle_z);

	data[0].x = cosf(angle_y) * cosf(angle_z);
	data[0].y = -cosf(angle_y) * sinf(angle_z);
	data[0].z = sinf(angle_y);

	data[1].x = cosf(angle_x) * sinf(angle_z) + sinf(angle_x) * sinf(angle_y) * cosf(angle_z);
	data[1].y = cosf(angle_x) * sinf(angle_z) - sinf(angle_x) * sinf(angle_y) * cosf(angle_z);
	data[1].z = -sinf(angle_x) * cosf(angle_y);

	data[2].x = sinf(angle_x) * sinf(angle_z) - cosf(angle_x) * sinf(angle_y) * cosf(angle_z);
	data[2].y = sinf(angle_x) * cosf(angle_z) + cosf(angle_x) * sinf(angle_y) * sinf(angle_z);
	data[2].z = cosf(angle_x) * cosf(angle_y);

	data[3].w = 1.0;

	return mat4(data[0], data[1], data[2], data[3]);
}

vec3 D, R;

mat4 lookat4fv(float yaw, float pitch, float x, float y, float z) {
	float Dy = sinf(deg2rad(pitch));
	float tmp = Dy / tanf(deg2rad(pitch));	//the shadow length of the vector in a ball
	//if (tmp < 0)
	//	tmp = -tmp;
	float Dx = cosf(deg2rad(yaw)) * cosf(deg2rad(pitch));
	float Dz = sinf(deg2rad(yaw)) * cosf(deg2rad(pitch));
	//float Dx = 

	//cout << Dx << ' ' << Dy << ' ' << Dz << endl;

	
	D.x = Dx;
	D.y = Dy;
	D.z = Dz;
	//vec3 D(Dx, 0, Dz);
	//tmp = norm3fv(D);
	//D.x /= tmp;
	//D.y /= tmp;
	//cout << D.x << ' ' << D.y << ' ' << D.z << endl;
	vec3 U(0, 1, 0);
	R = cross(D, U);
	float tmpR = norm3fv(R);
	R.x /= tmpR; R.y /= tmpR; R.z /= tmpR;				//fix fov increase when looking up or down	https://stackoverflow.com/questions/30409318/lookat-matrix-distorts-when-looking-up-or-down
	U = cross(R, D);
																		//D.y
	//mat4 left(vec4(R.x, R.y, R.z, 0), vec4(U.x, U.y, U.z, 0), vec4(D.x, D.y, D.z, 0), vec4(0, 0, 0, 1));	//rotation
	mat4 left(vec4(R.x, U.x, -D.x, 0), vec4(R.y, U.y, -D.y, 0), vec4(R.z, U.z, -D.z, 0), vec4(0, 0, 0, 1));
	//mat4 right(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(-x, -y, -z, 1));
	mat4 right = translate4fv(-x, -y, -z);	//translation
	mat4 result = right * left;
	
	return result;
}

float lastX = ScreenWidth / 2;
float lastY = ScreenHeight / 2;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//cout << "CamY:" << camYaw << " camP:" << camPitch << endl;
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos;

	offsetX *= mouse_sensitivity;
	offsetY *= mouse_sensitivity;

	camYaw += offsetX;
	camPitch += offsetY;

	if (camPitch > 89.0f)
		camPitch = 89.0f;
	if (camPitch < -89.0f)
		camPitch = -89.0f;
	

	lastX = xpos;
	lastY = ypos;
}

extern vec3 camPos;
extern int FOV_realtime;
mat4 proj_mat = projection4fv(FOVgame, 1.25, 0.1, 1000);
void drawBlock(int x, int y, int z, int id) {
	if (!id)
		return;
	mat4 fin_mat = lookat4fv(camYaw, camPitch, camPos.x, camPos.y, camPos.z);
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, translate4fv(x, y, z) *fin_mat);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_mat);
	glUniformMatrix4fv(shadow_location, 1, GL_FALSE, proj_mat);
	glDrawArrays(GL_TRIANGLES, 36 * id, 36);
}

extern int framecount;
extern double lastTime;
void showFPS(GLFWwindow* window) {
	// Measure speed
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	framecount++;
	if (delta >= 1.0) { // If last cout was more than 1 sec ago
		//cout << 1000.0 / double(framecount) << endl;

		double fps = double(framecount) / delta;

		std::stringstream ss;
		ss << "Hello World" << " " << " [" << fps << " FPS]" << "X:" << camPos.x << " Y:" << camPos.y << " Z:" << camPos.z ;

		glfwSetWindowTitle(window, ss.str().c_str());

		framecount = 0;
		lastTime = currentTime;
	}
}