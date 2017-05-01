#include <stdio.h>
#include <windows.h>
#include <locale.h>

#define W 50
#define H 50

#define VIEW_AREA_W 9
#define VIEW_AREA_H 9

#define VIEW_BUF_W VIEW_AREA_W*2
#define VIEW_BUF_H VIEW_AREA_H*2

#define GENCHANCE 10

#define RATSMAX 700


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

struct ratStat {
	int life;
	int power;
	int x;
	int y;
};

struct ratStat ratsStats[RATSMAX];

int map[W][H];
int ratsMap[W][H];
int viewBuf[VIEW_BUF_W][VIEW_BUF_H];

void fillMap();
void genCycle(int, int);
void genNewObj(int objNum, int ox, int oy);
void displayMapArea();
void testDivide();
void setObjectTypes();
void genObjectsAtRandomPos();
void fillViewBufWithSpaces();

void showRatsOnMap(int ratNum);
int isRatThere(int x, int y);

char objectTypes[15];

int ratsCount = 0;

HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO coninfo;
WORD saved_attributes;


void main(){
	setlocale(LC_ALL, "");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


	setObjectTypes();
	fillViewBufWithSpaces();
	fillMap();
	displayMapArea();
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

				int rat = ratsMap[x][y];
				if (rat != 0) {
					showRatsOnMap(rat);
				}
			}
			else {
				viewBuf[(w * 2) + 1][(h * 2) + 1] = WALL2; // '%'
			}
			printf("\n");
		}
	}

	int p1_cx = VIEW_AREA_W / 2 * 2;
	int p1_cy = VIEW_AREA_H / 2 * 2;
	p1_cx++;
	p1_cy++;
	/*	if ((p1_cx % 2) != 0) {
	p1_cx++;
	}
	if ((p1_cy % 2) != 0) {
	p1_cy++;
	}*/
	viewBuf[p1_cx][p1_cy] = PLAYER;

	printf("\n\n");

	//	GetConsoleScreenBufferInfo(hConsole, &coninfo);
	//	coninfo.dwCursorPosition.Y = 6;    // move up one line
	//	coninfo.dwCursorPosition.X = 0;    // move to the right the length of the word
	//	SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);

	printf("+");
	for (int w = 0; w < VIEW_BUF_W; w++) {
		printf("-");
	}
	printf("+");
	for (int h = 0; h < VIEW_BUF_H; h++) {
		printf("|");
		for (int w = 0; w < VIEW_BUF_W; w++) {
			printf("%c", objectTypes[viewBuf[w][h]]);
		}
		printf("|");
		printf("\n");
	}
	printf("+");
	for (int w = 0; w < VIEW_BUF_W; w++) {
		printf("-");
	}
	printf("+\n");

	for (int h = 0; h < VIEW_BUF_H; h++) {
		printf("| ");
		for (int w = 0; w < VIEW_BUF_W; w++) {
			printf("%d ", viewBuf[w][h]);
		}
		printf(" |");
		printf("\n");
	}
}


// Возвращает номер крысы в массиве ratsStats с указанными координатами
int isRatThere(int x, int y) {
	int i = 0;
	while (i < ratsCount) {
		if (ratsStats[i].x == x && ratsStats[i].y == y)
			return i;
		i++;
	}
	return -1;
}

void showRatsOnMap(int ratNum) {
	switch (ratNum) {
	case DEADRAT:
		printf("D");
		break;
	case RAT:
		printf("R");
		break;
	case 0:
		printf("0");
		break;
	default:
		printf("r");
		break;
	}
}

void setObjectTypes() {
	objectTypes[EMPTINESS] = ' ';
	objectTypes[DAGGER] = '!';
	objectTypes[WALL] = '#';
	objectTypes[GOLD] = '$';
	objectTypes[MEDKIT] = '+';
	objectTypes[WALL2] = '%%';
	objectTypes[GROUND] = '_';
	objectTypes[PLAYER] = '@';
	objectTypes[RAT] = 'R';
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


void fillMap() {
	genObjectsAtRandomPos();
}

void genObjectsAtRandomPos() {
	genCycle(100, MEDKIT);
	genCycle(100, DAGGER);
	genCycle(100, WALL);
	genCycle(100, GOLD);
	genCycle(100, RAT);
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

		if (objNum == RAT)
			if (isRatThere(nx, ny) != -1)
				continue;

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