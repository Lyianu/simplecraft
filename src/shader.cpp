#include<glad/glad.h>
#include<GLFW/glfw3.h>

const GLchar *vss = {
	"#version 450 core				\n"
	"								\n"
	"layout (location = 0) in vec4 data;								\n"
	"layout (location = 1) in vec2 sTexCoord;								\n"		//s for source
	"layout (location = 2) in vec4 bright;								\n"
	"								\n"
	"out vec2 TexCoord;								\n"
	"out vec4 vBright;								\n"

	//"out VS_OUT { vec4 shadow_coord;} vs_out;								\n"		
	"								\n"	
	"uniform mat4 mv_matrix;								\n"
	"uniform mat4 proj_matrix;								\n"
	//"uniform mat4 shadow_matrix;								\n"		
	"								\n"	
	"								\n"		
	"void main(void)				\n"
	"{								\n"
	"	gl_Position = proj_matrix * mv_matrix * data;							\n"
	"	TexCoord = sTexCoord;							\n"	
	"	vBright = bright;							\n"
	//"	vs_out.shadow_coord = proj_matrix * data;							\n"	
	"								\n"		
	"}								\n"
	"								\n"
	"								\n"
	"								\n"
};

const GLchar* fss = {
		"#version 450 core								\n"
		"								\n"
		"out vec4 FragColor;								\n"
		//"out vec4 color;								\n"

		"in vec2 TexCoord;								\n"
		"in vec4 vBright;								\n"
		//"in VS_OUT {vec4 shadow_coord;} fs_in;								\n"
		"								\n"
		"uniform sampler2D sTexture;								\n"
		//"layout (binding = 0) uniform sampler2DShadow shadow_tex;								\n"
		"								\n"
		"								\n"
		"void main(void)								\n"
		"{								\n"
		//"	color = vec4(1.0, 0, 0, 1.0);								\n"
		"	vec4 tex = texture(sTexture, TexCoord);								\n"
		//"	distance = vec4(gl_FragCoord.z / 10.0, gl_FragCoord.z / 10.0, gl_FragCoord.z / 10.0, 1.0); 						\n"
		"	if (tex.a < 0.1)							\n"		//allow transparency(no blending needed)
		"		discard;						\n"

		//"	color = textureProj(shadow_tex, fs_in.shadow_coord) * vec4(1.0)								\n"
		"	FragColor = tex - vBright;							\n"
		//"FragColor = tex;"
		"}								\n"
		"								\n"
		"								\n"
		"								\n"
		"								\n"
		"								\n"
		"								\n"
		"								\n"
	


	//"#version 450 core \n"
	//" \n"
	//"out vec4 color; \n"
	//" \n"
	//"void main(void) \n"
	//"{ \n"
	//" color = vec4(0.0, 0.8, 1.0, 1.0); \n"
	//"} \n"

};
