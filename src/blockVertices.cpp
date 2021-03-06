#include<iostream>
#include<cstring>
#include <game.h>
using namespace std;

static const GLfloat vertices_of_block_norm[] =
{
	-0.5f,  0.5f, -0.5f, 1.0f,//  1,1/1	1
	-0.5f, -0.5f, -0.5f, 1.0f,//X	  2 * 4		
	 0.5f, -0.5f, -0.5f, 1.0f,//		3
							//		    Z
	 0.5f, -0.5f, -0.5f, 1.0f,//	2,2/1
	 0.5f,  0.5f, -0.5f, 1.0f,//
	-0.5f,  0.5f, -0.5f, 1.0f,//

	 0.5f, -0.5f, -0.5f, 1.0f,//	3,1/2
	 0.5f, -0.5f,  0.5f, 1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,

	 0.5f, -0.5f,  0.5f, 1.0f,//	4,2/2
	 0.5f,  0.5f,  0.5f, 1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,

	 0.5f, -0.5f,  0.5f, 1.0f,//	5
	-0.5f, -0.5f,  0.5f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,

	-0.5f, -0.5f,  0.5f, 1.0f,//	6
	-0.5f,  0.5f,  0.5f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,

	-0.5f, -0.5f,  0.5f, 1.0f,//	7
	-0.5f, -0.5f, -0.5f, 1.0f,
	-0.5f,  0.5f,  0.5f, 1.0f,

	-0.5f, -0.5f, -0.5f, 1.0f,//	8
	-0.5f,  0.5f, -0.5f, 1.0f,
	-0.5f,  0.5f,  0.5f, 1.0f,

	-0.5f, -0.5f,  0.5f, 1.0f,//	9	lowwer face
	 0.5f, -0.5f,  0.5f, 1.0f,
	 0.5f, -0.5f, -0.5f, 1.0f,

	 0.5f, -0.5f, -0.5f, 1.0f,//	10
	-0.5f, -0.5f, -0.5f, 1.0f,
	-0.5f, -0.5f,  0.5f, 1.0f,

	-0.5f,  0.5f, -0.5f, 1.0f,//	11	upper
	 0.5f,  0.5f, -0.5f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,

	 0.5f,  0.5f,  0.5f, 1.0f,//	12
	-0.5f,  0.5f,  0.5f, 1.0f,
	-0.5f,  0.5f, -0.5f, 1.0f
};

static const GLfloat texture_positions_block_norm[] = {
	0, 0,	//1
	0, 1.0 / 16.0,
	1.0 / 16.0, 1.0 / 16.0,

	1.0 / 16.0, 1.0 / 16.0,	//2
	1.0 / 16.0, 0,
	0, 0,

	0, 1.0 / 16.0,	//3
	1.0 / 16.0, 1.0 / 16.0,
	0, 0,

	1.0 / 16.0, 1.0 / 16.0,	//4
	1.0 / 16.0, 0,
	0, 0,

	0, 1.0 / 16.0,	//5
	1.0 / 16.0, 1.0 / 16.0,
	0, 0,

	1.0 / 16.0, 1.0 / 16.0,	//6
	1.0 / 16.0, 0,
	0, 0,

	0, 1.0 / 16.0,	//7
	1.0 / 16.0, 1.0 / 16.0,
	0, 0,

	1.0 / 16.0, 1.0 / 16.0,	//8
	1.0 / 16.0, 0,
	0, 0,						//sideway finished

	1.0 / 16.0, 0 / 16.0,	//9
	0, 0,
	0, 1.0 / 16.0,

	0, 1.0 / 16.0,	//10
	1.0 / 16.0, 1.0 / 16.0,
	1.0 / 16.0, 0,			//bottom finished

	1.0 / 16.0, 0,
	0, 0,
	0, 1.0 / 16.0,

	0, 1.0 / 16.0,
	1.0 / 16.0, 1.0 / 16.0,
	1.0 / 16.0, 0,			//top finished
};


void InitBlocks() {
	fillBlockTexture(1, 1, 0, 1, 0, 1, 0);		//stone
	fillBlockTexture(2, 3, 0, 0, 0, 2, 0);		//grass block
	fillBlockTexture(3, 2, 0, 2, 0, 2, 0);		//dirt
	fillBlockTexture(4, 0, 1, 0, 1, 0, 1);		//cobblestone
	fillBlockTexture(5, 4, 0, 4, 0, 4, 0);		//oak plank
	//6 is oak sapling not implemented yet
	fillBlockTexture(7, 1, 1, 1, 1, 1, 1);		//bedrock
	fillBlockTexture(12, 2, 1, 2, 1, 2, 1);		//sand
	fillBlockTexture(13, 3, 1, 3, 1, 3, 1);		//gravel
	fillBlockTexture(14, 0, 2, 0, 2, 0, 2);		//gold ore
	fillBlockTexture(15, 1, 2, 1, 2, 1, 2);		//iron ore
	fillBlockTexture(16, 2, 2, 2, 2, 2, 2);		//coal ore
	fillBlockTexture(17, 4, 1, 5, 1, 5, 1);		//oak log
	fillBlockTexture(18, 4, 3, 4, 3, 4, 3);		//oak leaves
	fillBlockTexture(19, 0, 3, 0, 3, 0, 3);		//sponge
	fillBlockTexture(20, 1, 3, 1, 3, 1, 3);		//glass
	fillBlockTexture(21, 0, 10, 0, 10, 0, 10);	//lapis ore
	fillBlockTexture(22, 0, 9, 0, 9, 0, 9);		//lapis block
	fillBlockTexture(23, 14, 2, 14, 2, 14, 2);	//dispenser(not implemented)
	fillBlockTexture(24, 0, 12, 0, 11, 0, 13);	//sandstone
	fillBlockTexture(25, 10, 4, 11, 4, 10, 4);	//note block
	//27	powered rail
	//28	detector rail
	fillBlockTexture(29, 12, 6, 10, 6, 13, 6);	//sticky piston
	//30	cobweb
	//31	grass
	//32	fern
	//33	piston
	fillBlockTexture(35, 0, 4, 0, 4, 0, 4);	//white wool

	fillBlockTexture(49, 13, 13, 13, 13, 13, 13);	//selected block edge
}



float test[200];

extern GLfloat vertex_positions[128][144];
extern GLfloat texture_positions[128][72];

void fillBlockTexture(int id, int sideOffsetX, int sideOffsetY, int topOffsetX, int topOffsetY, int bottomOffsetX, int bottomOffsetY) {	//offsetX is the horizontal offset starting from left, offsetY is the vertical offset starting from up
	memcpy(vertex_positions + id, vertices_of_block_norm, 576);		//size of 144 floats
	memcpy(texture_positions + id, texture_positions_block_norm, 288);	//size of 72 floats
	float block_dist = 1.0f / 16.0f;	//width of a block in texture file
	for (int i = 0; i < 8; i++) {		//sideway iteration
		texture_positions[id][6 * i] += sideOffsetX * block_dist;
		texture_positions[id][6 * i + 2] += sideOffsetX * block_dist;
		texture_positions[id][6 * i + 4] += sideOffsetX * block_dist;

		texture_positions[id][6 * i + 1] += sideOffsetY * block_dist;
		texture_positions[id][6 * i + 3] += sideOffsetY * block_dist;
		texture_positions[id][6 * i + 5] += sideOffsetY * block_dist;
	}

	for (int i = 8; i < 10; i++) {		//bottom iteration
		texture_positions[id][6 * i] += bottomOffsetX * block_dist;
		texture_positions[id][6 * i + 2] += bottomOffsetX * block_dist;
		texture_positions[id][6 * i + 4] += bottomOffsetX * block_dist;

		texture_positions[id][6 * i + 1] += bottomOffsetY * block_dist;
		texture_positions[id][6 * i + 3] += bottomOffsetY * block_dist;
		texture_positions[id][6 * i + 5] += bottomOffsetY * block_dist;
	}

	for (int i = 10; i < 12; i++) {		//top iteration
		texture_positions[id][6 * i] += topOffsetX * block_dist;
		texture_positions[id][6 * i + 2] += topOffsetX * block_dist;
		texture_positions[id][6 * i + 4] += topOffsetX * block_dist;

		texture_positions[id][6 * i + 1] += topOffsetY * block_dist;
		texture_positions[id][6 * i + 3] += topOffsetY * block_dist;
		texture_positions[id][6 * i + 5] += topOffsetY * block_dist;
	}

	cout << "Finished:" << id << endl;
	//for (int i = 0; i < 50; i++)
	//	cout << vertices_of_block_norm[i] << ' ';
}