#include <stdio.h>

#define W 50
#define H 50

#define VIEW_AREA_W 20
#define VIEW_AREA_H 10

#define VIEW_BUF_W VIEW_AREA_W*2
#define VIEW_BUF_H VIEW_AREA_H*2

#define GENCHANCE 10

/*
 1. fill map
 2. display map
*/

int map[W][H];

enum objects {
	EMPTINESS = 0,
	PLAYER = 1,
	GROUND = 2,
	WALL = 3,
	RAT = 4,
	MEDKIT = 5,
	PICK = 6,
	DAGGER = 7,
	GOLD = 8,
	WALL2 = 9, // непробиваемая стена
	BOSS1 = 10,
	BOSS2 = 11,
	CELL = 12, // потайная плита
	DEADRAT = 13 // мертвая крыса
};

void fillMap();
void genCycle(int, int);
void genNewObj(int objNum, int ox, int oy);
void displayMapArea();
void testDivide();
void setObjectTypes();
void genObjectsAtRandomPos();
void fillViewBufWithSpaces();

int viewBuf[VIEW_BUF_W][VIEW_BUF_H];

char objectTypes[15];

void main(){
	setObjectTypes();
	fillViewBufWithSpaces();
	fillMap();
	displayMapArea();
}

void setObjectTypes() {
	objectTypes[EMPTINESS] = ' ';
	objectTypes[DAGGER] = '!';
	objectTypes[WALL] = '#';
	objectTypes[GOLD] = '$';
	objectTypes[MEDKIT] = '+';
	objectTypes[WALL2] = '%';
	objectTypes[GROUND] = '_';
	objectTypes[PLAYER] = '@';
}

void testDivide() {
	int a = 10;
	int b = 5;
	printf("%d", a / 2);
	printf("%d", b / 2);
}

void fillViewBufWithSpaces() {
	for (int h = 0; h < VIEW_BUF_H; h++) {
		for (int w = 0; w < VIEW_BUF_W; w++) {
			viewBuf[w][h] = EMPTINESS;
		}
	}
}

void displayMapArea() {
	int p1x = 0;
	int p1y = 0;

	for (int h = 0; h < VIEW_AREA_H; h++) {
		for (int w = 0; w < VIEW_AREA_W; w++) {
			int x = p1x - VIEW_AREA_W / 2 + w;
			int y = p1y - VIEW_AREA_H / 2 + h;
			printf("x = %d; y = %d; w = %d; h = %d", x, y, (w * 2) + 1, (h * 2) + 1);

			if (x >= 0 && x < W && y >= 0 && y < H) {
				int objectOnMap = map[x][y];
				printf(";map = %d", objectOnMap);
				if (objectOnMap == 0) {
					objectOnMap = GROUND;
				}
				viewBuf[(w * 2) + 1][(h * 2) + 1] = objectOnMap;
				//viewBuf[(w * 2) + 1][(h * 2) + 1] = 1;
			}
			else {
				viewBuf[(w * 2) + 1][(h * 2) + 1] = WALL2; // '%'
			}
			printf("\n");
		}
	}
	viewBuf[VIEW_AREA_W+1][VIEW_AREA_H+1] = PLAYER;

	printf("\n\n");

	for (int h = 0; h < VIEW_BUF_H; h++) {
		for (int w = 0; w < VIEW_BUF_W; w++) {
			printf("%c", objectTypes[viewBuf[w][h]]);
		}
		printf("\n");
	}
}

void fillMap() {
	genObjectsAtRandomPos();
}

void genObjectsAtRandomPos() {
	//genCycle(10, RAT);
	genCycle(100, MEDKIT);
	genCycle(100, DAGGER);
	genCycle(100, WALL);
	genCycle(100, GOLD);
}

// цикл создания случайных координат указанных объектов до максимума
// max - количество циклов генерирования
void genCycle(int max, int objNum) {
	int genCount = 0; // количество попыток создания объекта
	while (1) {
		int nx = rand() % W;
		int ny = rand() % H;
		if (map[nx][ny] != EMPTINESS)
			continue;

		/*
				if (objNum == RAT)
					if (isRatThere(nx, ny) != -1)
						continue;
		*/

		if (rand() % GENCHANCE == 1)
			genNewObj(objNum, nx, ny);

		genCount++;
		if (genCount == max)
			break;
	}
}


void genNewObj(int objNum, int ox, int oy) {
	map[ox][oy] = objNum;
}
