#include<iostream>
#include<fstream>
#include<game.h>
#include<cmath>
#include "ppm.h"
#include "PerlinNoise.h"
#include<irrKlang.h>
using namespace std;

extern int noiseMap[20][20];
extern chunk firstchunk;
extern chunk* lastChunkPtr;

int hMap[X_LIMIT][Z_LIMIT];
int noiseMap[20][20];
int CanBeSeen[X_LIMIT][Y_LIMIT][Z_LIMIT];

int HEIGHT_GEN_MAX = 30;
int perlin2d(int x, int z, int seed) {
	int tx = x / 16;	//the noise map is devided by 16
	int tz = z / 16;										//3		4
	if (x < 0)												//1		2
		tx--;
	if (z < 0)
		tz--;
	srand(x << 2 * z << 3 + seed + 1);
	int h1 = rand() % HEIGHT_GEN_MAX;
	int h2 = rand() % HEIGHT_GEN_MAX;
	int h3 = rand() % HEIGHT_GEN_MAX;
	int h4 = rand() % HEIGHT_GEN_MAX;

	vec2 g1, g2, g3, g4;	//gradient of 4 points

	g1.x = float((rand() % 255)) / 255;
	g1.y = float((rand() % 255)) / 255;
	//cout << 'A' << g1.x << endl;
	float tmp1 = norm2fv(g1);
	g1.x /= tmp1;	g1.y /= tmp1;

	g2.x = float((rand() % 255)) / 255;
	g2.y = float((rand() % 255)) / 255;
	float tmp2 = norm2fv(g2);
	g2.x /= tmp2;	g2.y /= tmp2;

	g3.x = float((rand() % 255)) / 255;
	g3.y = float((rand() % 255)) / 255;
	float tmp3 = norm2fv(g3);
	g3.x /= tmp3;	g3.y /= tmp3;

	g4.x = float((rand() % 255)) / 255;
	g4.y = float((rand() % 255)) / 255;
	float tmp4 = norm2fv(g4);
	g4.x /= tmp4;	g4.y /= tmp4;

	int ix = x - tx * 16, iz = z - tz * 16;
	float u = (ix + 0.5) / 16, v = (iz + 0.5) / 16;
	vec2 v1(u, v), v2(1 - u, v), v3(u, 1 - v), v4(1 - u, 1 - v);
	float dot1 = v1 * g1, dot2 = v2 * g2, dot3 = v3 * g3, dot4 = v4 * g4;
	//float avg1 = dot1 * (1 - smooth(u)) + dot3 * smooth(u);
	//float avg2 = dot2 * (1 - smooth(u)) + dot4 * smooth(u);

	//int result = avg1 * (1 - smooth(v)) + avg2 * smooth(v);
	//cout << g1.x << g1.y << endl;
	int result = smooth(dot1) * h1 + smooth(dot2) * h2 + smooth(dot3) * h3 + smooth(dot4) * h4;
	float amplifier = 0.5;
	return result * amplifier;
}

void generateWorldPerlin(int seed) {	//not actually using perlin here
	cout << "Generating level, wait." << endl;
	for (int x = 0; x < X_LIMIT; x++)		// 10000 / 200 per segment
		for (int z = 0; z < Z_LIMIT; z++) {
			srand(seed * x * z);
			//hMap[x][z] = rand() % 50;
			hMap[x][z] = 49;
			//noiseMap[x][z] = rand() % 20;
			//hMap[i * 50][j * 50] = noiseMap[i][j];
		}
	//for (int x = 0; x < 1000; x++) {
	//	for (int z = 0; z < 1000; z++) {
	//		int prev = x - x % 50;
	//		int diff = noiseMap[x / 50][z / 50 + 1] - noiseMap[x / 50][z / 50];
	//		hMap[x][z] = noiseMap[x / 50][z / 50] + (int)(diff * (prev / 50.0));
	//	}
	//}

	for (int i = 0; i < X_LIMIT; i++)
		for (int k = 1; k < Y_LIMIT; k++)
			for (int j = 0; j < Z_LIMIT; j++)
				CanBeSeen[i][k][j] = isCanBeSeen(i, k, j);

	cout << "level generated." << endl;
}

bool isCanBeSeen(int x, int y, int z) {
	if (hMap[x][z] > y && hMap[x + 1][z] > y && hMap[x - 1][z] > y && hMap[x][z + 1] > y && hMap[x][z - 1] > y && x && z && x != X_LIMIT && z != Z_LIMIT)
		return false;
	else
		return true;
}

int heightRng(int x, int z) {	//result between +/-31
	srand(x << 2 + SEED - z + x * z);		//random number generator for height using cmath lib
	int result;
	//result = rand();
	//srand(result * SEED + SEED);
	result = (rand() + 500) % 500;
	result = sqrt(result);
	//result %= 20;
	return (result % 2) ? result : -result;
}

bool isChunkExist(int x, int z) {
	chunk* chunkPtr = &firstchunk;
	while (chunkPtr != chunkPtr->next) {
		if (chunkPtr->id == -1)
			return false;
		if (chunkPtr->x == x && chunkPtr->z == z)
			return true;
		chunkPtr = chunkPtr->next;
	}
	if (chunkPtr->id == -1)
		return false;
	if (chunkPtr->x == x && chunkPtr->z == z)
		return true;
	return false;
}

chunk* getChunk(int x, int z) {
	//cout << "getting X:" << x << ' ' << z << endl;
	chunk* chunkPtr = &firstchunk;
	if (chunkPtr->x == x && chunkPtr->z == z) {
		//cout << "FirstCHunkFound: x: " << chunkPtr->x << " z: " << chunkPtr->z << endl;
		return chunkPtr;
	}

	while (chunkPtr != chunkPtr->next) {
		chunkPtr = chunkPtr->next;
		if (chunkPtr->x == x && chunkPtr->z == z) {
			//cout << "Found: x: " << chunkPtr->x << " z: " << chunkPtr->z << endl;
			return chunkPtr;
		}
	}
	chunk* tmp = new chunk;
	*tmp = generateChunk(x, z);
	tmp->x = x;
	tmp->z = z;
	tmp->id = chunkPtr->id + 1;
	tmp->before = chunkPtr;
	chunkPtr->next = tmp;
	tmp->next = tmp;
	//checkChunkVisibility(tmp);	//this is disabled because it is possible to access a pointer before it's ready


	cout << "Generated ID: " << tmp->id << endl;

	return tmp;
}

chunk currentChunkLoad;
chunk *currentChunkLoadPtr;
void LoadSave() {
	ifstream savefile;
	savefile.open("savefile", ios::in | ios::binary);
	if (!savefile) {		//no savefile was read
		firstchunk = generateChunk(0, 0);
		firstchunk.id = 0;
	}
	else {
		savefile.read((char*)&firstchunk, sizeof(chunk));
		firstchunk.before = &firstchunk;
		//currentChunkLoad = &firstchunk;
		currentChunkLoadPtr = &firstchunk;
		while (savefile.read((char*)&currentChunkLoad, sizeof(chunk))) {
			static chunk a = *new chunk;
			currentChunkLoadPtr->next = &a;
			a = currentChunkLoad;
			a.before = currentChunkLoadPtr;
			currentChunkLoadPtr = &a;
		}
		currentChunkLoadPtr->next = currentChunkLoadPtr;		//the last member of the list should be itself
	}
}

int levelGenerateBasis = 60;	//avg height of the level
chunk generateChunk(int x, int z) {

	int sx = x, sz = z;

	cout << "called : " << x << ' ' << z << endl;
	int h00, h01, h10, h11;		//height for 4 vertices of the chunk

	int hNorth, hEast, hWest, hSouth, hCenter;		//get all surrounding vertices' height to make the level smoother
/*
	{
	hCenter = heightRng(x, z);
	hNorth = heightRng(x, z + 1);
	hEast = heightRng(x + 1, z);
	hSouth = heightRng(x, z - 1);
	hWest = heightRng(x - 1, z);
	h00 = hCenter + hNorth + hEast + hSouth + hWest;
	h00 /= 4;
	//h00 = hCenter;
	h00 += levelGenerateBasis;

	x += 1;
	hCenter = heightRng(x, z);
	hNorth = heightRng(x, z + 1);
	hEast = heightRng(x + 1, z);
	hSouth = heightRng(x, z - 1);
	hWest = heightRng(x - 1, z);
	h01 = hCenter + hNorth + hEast + hSouth + hWest;
	h01 /= 4;
	//h01 = hCenter;
	h01 += levelGenerateBasis;

	z += 1;
	hCenter = heightRng(x, z);
	hNorth = heightRng(x, z + 1);
	hEast = heightRng(x + 1, z);
	hSouth = heightRng(x, z - 1);
	hWest = heightRng(x - 1, z);
	h11 = hCenter + hNorth + hEast + hSouth + hWest;
	h11 /= 4;
	//h11 = hCenter;
	h11 += levelGenerateBasis;

	x -= 1;
	hCenter = heightRng(x, z);
	hNorth = heightRng(x, z + 1);
	hEast = heightRng(x + 1, z);
	hSouth = heightRng(x, z - 1);
	hWest = heightRng(x - 1, z);
	h10 = hCenter + hNorth + hEast + hSouth + hWest;
	h10 /= 4;
	//h10 = hCenter;
	h10 += levelGenerateBasis;
}

	


	z--;		//always remember

	*/
	

	//h00 = perlin2d(x, z, SEED) + levelGenerateBasis;
	//h01 = perlin2d(x + 1, z, SEED) + levelGenerateBasis;
	//h10 = perlin2d(x, z + 1, SEED) + levelGenerateBasis;
	//h11 = perlin2d(x + 1, z + 1, SEED) + levelGenerateBasis;

	//h00 = x;
	//h01 = 2 * x;
	//h10 = z;
	//h11 = 2 * z;

	PerlinNoise pn(SEED);
	h00 = 10 + 80 * pn.noise(sx, sz, 0.8);
	h01 = 10 + 80 * pn.noise(sx + 1, sz, 0.8);
	h10 = 10 + 80 * pn.noise(sx, sz + 1, 0.8);
	h11 = 10 + 80 * pn.noise(sx + 1, sz + 1, 0.8);


	chunk result;
	result.x = sx;
	result.z = sz;

	//height map using bilinear interpolation
	//h00 = 10 * x;
	//h01 = 40;
	//h10 = 40;
	//h11 = 40;

	cout << h00 << ' ' << h01 << ' ' << h10 << ' ' << h11 << endl;

	int height_map_chunk[16][16];
	height_map_chunk[0][0] = h00;
	height_map_chunk[15][0] = h10;
	height_map_chunk[0][15] = h01;
	height_map_chunk[15][15] = h11;
	for (int i = 0; i < 16; i++) {
		height_map_chunk[0][i] = ((15.5f - i) * h00 + (i + 0.5f) * h10) / 16.0f;
		height_map_chunk[15][i] = ((15.5f - i) * h01 + (i + 0.5f) * h11) / 16.0f;
	}
	for (int ix = 1; ix < 15; ix++)
		for (int iz = 0; iz < 16; iz++) {
			height_map_chunk[ix][iz] = (15.5f - ix) / 16.0f * (float)height_map_chunk[0][iz] + ((ix + 0.5) / 16.0f) * (float)height_map_chunk[15][iz];
			//height_map_chunk[ix][iz] %= 20;
		}

	//for (int ix = 0; ix < 16; ix++) {
	//	for (int iz = 0; iz < 16; iz++)
	//		cout << height_map_chunk[ix][iz] << ' ';
	//	cout << endl;

	//}

	//for (int ix = 0; ix < 16; ix++)
	//	for (int iy = 0; iy < 256; iy++)
	//		for (int iz = 0; iz < 16; iz++)
	//			result.data[ix][iy][iz] = 0;
	memset(&result.data, 0, sizeof(result.data));		//shit compiler
	memset(&result.visibility, 0, sizeof(result.visibility));
	
	float tempMap[16][16];		//biome

	tempMap[0][0] = pn.noise(sx, sz, 0.1);
	tempMap[15][0] = pn.noise(sx + 1, sz, 0.1);
	tempMap[0][15] = pn.noise(sx, sz + 1, 0.1);
	tempMap[15][15] = pn.noise(sx + 1, sz + 1, 0.1);
	for (int i = 1; i < 15; i++) {
		tempMap[0][i] = ((15.5f - i) * tempMap[0][0] + (i + 0.5f) * tempMap[0][15]) / 16.0f;
		tempMap[15][i] = ((15.5f - i) * tempMap[15][0] + (i + 0.5f) * tempMap[15][15]) / 16.0f;
	}
	for (int ix = 1; ix < 15; ix++)
		for (int iz = 0; iz < 16; iz++) {
			tempMap[ix][iz] = (15.5f - ix) / 16.0f * (float)tempMap[0][iz] + ((ix + 0.5) / 16.0f) * (float)tempMap[15][iz];
			//tempMap[ix][iz] %= 20;
		}
	for (int ix = 0; ix < 16; ix++)
		for (int iz = 0; iz < 16; iz++) {
			if (tempMap[ix][iz] > 0.49)
				tempMap[ix][iz] = 2;
			else
				tempMap[ix][iz] = 12;
		}
	//for (int ix = 0; ix < 16; ix++) {
	//	for (int iz = 0; iz < 16; iz++)
	//		cout << tempMap[ix][iz] << ' ';
	//	cout << 'T' << endl;

	//}

	for (int inner_x = 0; inner_x < 16; inner_x++)
		for (int inner_z = 0; inner_z < 16; inner_z++) {
			int height = height_map_chunk[inner_x][inner_z];
			for (int inner_y = 0; inner_y < height - 3; inner_y++)
				result.data[inner_x][inner_y][inner_z] = 1;

			for (int inner_y = height - 3; inner_y <= height; inner_y++)
				result.data[inner_x][inner_y][inner_z] = tempMap[inner_x][inner_z];
		}
	//result.data[0][156][0] = 2;
	//for (int ix = 0; ix < 16; ix++)
	//	for (int iy = 0; iy < 256; iy++) {
	//		result.visibility[ix][iy][0] = true;
	//		result.visibility[ix][iy][15] = true;
	//	}
	//for (int ix = 0; ix < 16; ix++)
	//	for (int iz = 0; iz < 16; iz++) {
	//		result.visibility[ix][0][iz] = true;
	//		result.visibility[ix][255][iz] = true;
	//	}
	//for (int iz = 0; iz < 16; iz++)
	//	for (int iy = 0; iy < 256; iy++) {
	//		result.visibility[0][iy][iz] = true;
	//		result.visibility[15][iy][iz] = true;
	//	}
	//for (int ix = 1; ix < 15; ix++)
	//	for (int iy = 1; iy < 255; iy++)
	//		for (int iz = 1; iz < 15; iz++) {
	//			if (result.data[ix + 1][iy][iz] && result.data[ix - 1][iy][iz] && result.data[ix][iy + 1][iz] && result.data[ix][iy - 1][iz] && result.data[ix][iy][iz + 1] && result.data[ix][iy][iz - 1])
	//				result.visibility[ix][iy][iz] = false;
	//			else
	//				result.visibility[ix][iy][iz] = true;
	//			if (!result.data[ix][iy][iz])
	//				result.visibility[ix][iy][iz] = false;
	//		}

	for (int ix = 0; ix < 16; ix++)
		for (int iy = 0; iy < 256; iy++)
			for (int iz = 0; iz < 16; iz++)
				result.visibility[ix][iy][iz] = 0;

	for (int ix = 1; ix < 15; ix++)
		for (int iy = 1; iy < 255; iy++)		//it's safe to throw 0 and 255
			for (int iz = 1; iz < 15; iz++) {
				if (!isTransparent(result.data[ix + 1][iy][iz]) && !isTransparent(result.data[ix - 1][iy][iz]) && !isTransparent(result.data[ix][iy + 1][iz]) && !isTransparent(result.data[ix][iy - 1][iz]) && !isTransparent(result.data[ix][iy][iz + 1]) && !isTransparent(result.data[ix][iy][iz - 1]))
					result.visibility[ix][iy][iz] = false;
				else
					result.visibility[ix][iy][iz] = true;
				if (!result.data[ix][iy][iz])
					result.visibility[ix][iy][iz] = false;
			}

	for (int ix = 0; ix < 16; ix++)
		for (int iy = 1; iy < 256; iy++) {
			if (!isTransparent(result.data[ix][iy][0]) && isTransparent(result.data[ix][iy + 1][0]))
				result.visibility[ix][iy][0] = true;
			if (!isTransparent(result.data[ix][iy][15]) && isTransparent(result.data[ix][iy + 1][15]))
				result.visibility[ix][iy][15] = true;
		}
	for (int iz = 0; iz < 16; iz++)
		for (int iy = 1; iy < 256; iy++) {
			if (!isTransparent(result.data[0][iy][iz]) && isTransparent(result.data[0][iy + 1][iz]))
				result.visibility[0][iy][iz] = true;
			if (!isTransparent(result.data[15][iy][iz]) && isTransparent(result.data[15][iy + 1][iz]))
				result.visibility[15][iy][iz] = true;
		}

//#pragma omp parallel for
//	for (int ix = 0; ix < 16; ix++)		//shouldn't be performed here cause the chunk is not added to the list yet.
//		for (int iy = 0; iy < 256; iy++)	//shouldn't use getBlockID func to avoid running in to chunk gen loop.
//			for (int iz = 0; iz < 16; iz++) {
//				if (getBlockID(16 * x + ix + 1, iy, 16 * z + iz) && getBlockID(16 * x + ix - 1, iy, 16 * z + iz) && getBlockID(16 * x + ix, iy + 1, 16 * z + iz) && getBlockID(16 * x + ix, iy - 1, 16 * z + iz)&& getBlockID(16 * x + ix, iy, 16 * z + iz + 1) && getBlockID(16 * x + ix, iy, 16 * z + iz - 1))
//					result.visibility[ix][iy][iz] = false;
//				else
//					result.visibility[ix][iy][iz] = true;
//				if (!getBlockID(16 * x + ix, iy, 16 * z + iz))
//					result.visibility[ix][iy][iz] = false;
//			}


	cout << x << ' ' << z << "done." << endl;
	return result;
}

void renderChunk(int x, int z) {
	//cout << endl << "Rendering: " << x << ' ' << z << endl;
	chunk* currentChunk = getChunk(x, z);
	//cout << "X: " << x << " Z: " << z << " ID: " << currentChunk->id << endl;
	//outputHeightMap(x, z);

	int baseX = 16 * x, baseZ = 16 * z;
//#pragma omp parallel for
	for (int i = 0; i < 16; i++)
//#pragma omp parallel for				//disable multi_threading to avoid heap overflow
		for (int j = 0; j < 256; j++)
			for (int k = 0; k < 16; k++)
				if (currentChunk->visibility[i][j][k])
					drawBlock(baseX + i, j, baseZ + k, currentChunk->data[i][j][k]);
				//drawBlock(baseX + i, j, baseZ + k, test.data[i][j][k]);
	//cout << "CHUNKID: " << currentChunk->id << endl;
}

void deleteList() {		//return all memory to the system
	chunk* current = &firstchunk;
	while (current->next != current) {
		current = current->next;
		delete current->before;
	}
	delete current;
	cout << "returned list memory" << endl;
}

int getBlockID(int x, int y, int z) {		//get specified block id(what block it is)
	if (y < 0 || y > 255)
		return -1;
	int ix = (x - (x / 16) * 16 + 16) % 16, iz = (z - (z / 16) * 16 + 16) % 16;
	int tx = x / 16, tz = z / 16;
	if (x < 0)
		tx--;
	if (z < 0)
		tz--;
	if (isChunkExist(tx, tz)) {
		chunk* tmp = getChunk(tx, tz);
		int id = tmp->data[ix][y][iz];
		return id;
	}

	{
		x /= 16;
		z /= 16;
		//x = tx;
		//z = tz;
		int h00, h01, h10, h11;		//height for 4 vertices of the chunk

		int hNorth, hEast, hWest, hSouth, hCenter;		//get all surrounding vertices' height to make the level smoother

		hCenter = heightRng(x, z);
		hNorth = heightRng(x, z + 1);
		hEast = heightRng(x + 1, z);
		hSouth = heightRng(x, z - 1);
		hWest = heightRng(x - 1, z);
		h00 = hCenter + hNorth + hEast + hSouth + hWest;
		h00 /= 4;
		//h00 = hCenter;
		h00 += levelGenerateBasis;

		x += 1;
		hCenter = heightRng(x, z);
		hNorth = heightRng(x, z + 1);
		hEast = heightRng(x + 1, z);
		hSouth = heightRng(x, z - 1);
		hWest = heightRng(x - 1, z);
		h01 = hCenter + hNorth + hEast + hSouth + hWest;
		h01 /= 4;
		//h01 = hCenter;
		h01 += levelGenerateBasis;

		z += 1;
		hCenter = heightRng(x, z);
		hNorth = heightRng(x, z + 1);
		hEast = heightRng(x + 1, z);
		hSouth = heightRng(x, z - 1);
		hWest = heightRng(x - 1, z);
		h11 = hCenter + hNorth + hEast + hSouth + hWest;
		h11 /= 4;
		//h11 = hCenter;
		h11 += levelGenerateBasis;

		x -= 1;
		hCenter = heightRng(x, z);
		hNorth = heightRng(x, z + 1);
		hEast = heightRng(x + 1, z);
		hSouth = heightRng(x, z - 1);
		hWest = heightRng(x - 1, z);
		h10 = hCenter + hNorth + hEast + hSouth + hWest;
		h10 /= 4;
		//h10 = hCenter;
		h10 += levelGenerateBasis;

		int squarex1 = ix * ix, squarex2 = (16 - ix) * (16 - ix), squarez1 = iz * iz, squarez2 = (16 - iz) * (16 - iz);
		int square_sum = squarex1 + squarex2 + squarez1 + squarez2;
		int height = 0;
		height += h00 * (float)squarex1 / square_sum + h01 * (float)squarex2 / square_sum + h10 * (float)squarez1 / square_sum + h11 * (float)squarez2 / square_sum;
		height /= 4;

		if (height >= y)
			return 2;
	}
			return 0;
	

}

void checkChunkVisibility(chunk* target) {
	int x_start = target->x * 16;
	int z_start = target->z * 16;
	for (int ix = 0; ix < 16; ix++)
		for (int iz = 0; iz < 16; iz++)
			for (int iy = 0; iy < 256; iy++)
				checkBlockVisibility(x_start + ix, iy, z_start + iz);

}

void addBlock(int x, int y, int z, int id) {
	int ix = (x - (x / 16) * 16 + 16) % 16, iz = (z - (z / 16) * 16 + 16) % 16;
	int tx = x / 16, tz = z / 16;
	if (x < 0)
		tx--;
	if (z < 0)
		tz--;
	chunk* tmp = getChunk(tx, tz);
	tmp->data[ix][y][iz] = id;
	tmp->visibility[ix][y][iz] = true;
	checkBlockVisibility(x + 1, y, z);
	checkBlockVisibility(x - 1, y, z);
	checkBlockVisibility(x, y + 1, z);
	checkBlockVisibility(x, y - 1, z);
	checkBlockVisibility(x, y, z + 1);
	checkBlockVisibility(x, y, z - 1);
}

void deleteBlock(int x, int y, int z) {
	int ix = (x - (x / 16) * 16 + 16) % 16, iz = (z - (z / 16) * 16 + 16) % 16;
	int tx = x / 16, tz = z / 16;
	if (x < 0)
		tx--;
	if (z < 0)
		tz--;
	chunk* tmp = getChunk(tx, tz);
	cout << "DELETING: " << tmp->id << endl;
	tmp->data[ix][y][iz] = 0;
	if (getBlockID(x + 1, y, z) && getBlockID(x + 1, y, z) != -1)
		changeBlockVisibility(x + 1, y, z, true);
	if (getBlockID(x - 1, y, z) && getBlockID(x - 1, y, z) != -1)
		changeBlockVisibility(x - 1, y, z, true);
	if (getBlockID(x, y + 1, z) && getBlockID(x, y + 1, z) != -1)
		changeBlockVisibility(x, y + 1, z, true);
	if (getBlockID(x, y - 1, z) && getBlockID(x, y - 1, z) != -1)
		changeBlockVisibility(x, y - 1, z, true);
	if (getBlockID(x, y, z + 1) && getBlockID(x, y, z + 1) != -1)
		changeBlockVisibility(x, y, z + 1, true);
	if (getBlockID(x, y, z - 1) && getBlockID(x, y, z - 1) != -1)
		changeBlockVisibility(x, y, z - 1, true);

}

void changeBlockVisibility(int x, int y, int z, bool vis) {

	cout << "checking Block:" << x << ' ' << y << ' ' << z << endl;
	int ix = (x - (x / 16) * 16 + 16) % 16, iz = (z - (z / 16) * 16 + 16) % 16;
	int tx = x / 16, tz = z / 16;
	if (x < 0)
		tx--;
	if (z < 0)
		tz--;
	chunk* tmp = getChunk(tx, tz);
	tmp->visibility[ix][y][iz] = vis;
	cout << "changed\n";
}

bool isTransparent(int id) {
	if (!id || id == 18 || id == 20)
		return true;
	return false;
}

void checkBlockVisibility(int x, int y, int z) {
	if (isTransparent(getBlockID(x + 1, y, z)) || isTransparent(getBlockID(x - 1, y, z)) || isTransparent(getBlockID(x, y + 1, z)) || isTransparent(getBlockID(x, y - 1, z)) || isTransparent(getBlockID(x, y, z + 1)) || isTransparent(getBlockID(x, y, z - 1)))
		changeBlockVisibility(x, y, z, true);
	else
		changeBlockVisibility(x, y, z, false);
	if (!getBlockID(x, y, z))	//air block
		changeBlockVisibility(x, y, z, false);
}

void outputHeightMap(int x, int z) {
	chunk* tmp = getChunk(x, z);
	for (int ix = 0; ix < 16; ix++) {
		for (int iz = 0; iz < 16; iz++) {
			for (int y = 0; y < 256; y++)
				if (!tmp->data[ix][y][iz]) {
					cout << y;
					break;
				}
			cout << ' ';
		}
		cout << endl;
	}
}