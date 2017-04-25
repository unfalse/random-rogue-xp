
// координаты сарая с боссом
#define BRX 21
#define BRY 29

// константа вероятности создания нового объекта
#define GENCHANCE 10

#define RATSMAX 700
#define MEDSMAX 700
#define BATTMAX 1000
#define COLSMAX 50
#define GOLDMAX 5000

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

// карта
#define mapMaxX 80
#define mapMaxY 8

#define W 50
#define H 50

#define VIEW_AREA_W 10
#define VIEW_AREA_H 5

#define VIEW_BUF_W VIEW_AREA_W*2
#define VIEW_BUF_H VIEW_AREA_H*2

// системные функции
void enumerateObjs(int, int, int); // система

void genNewRandomObj(int, int); // система
void genObjectsAround(int, int); // система
void genNewObj(int, int, int); // система
void genSecretCells(); // система
void genObjectsAtRandomPos(); // система
void genCycle(int, int); // система

void triggerSecretCell(); // система
void displayAbsMap();
int secretColHere(int, int); // система
void markGround();
void help();

// +- - -- - -- - -- - -- - -- - -- - -- - -- - -- - +

// |					крысы/дроны					 |

// +- - -- - -- - -- - -- - -- - -- - -- - -- - -- - +
void ratsAttack();
void ratAttacked(int, int);
void bossAttacked();
void decayDeadRats();
void generateDeadRats();
void ratLogic();
void fillRatsMap();
void killAllRatsButOne();

// +- - -- - -- - -- - -- - -- - -- - -- - -- - -- - +

// |				босс и его комната				 |

// +- - -- - -- - -- - -- - -- - -- - -- - -- - -- - +
void createBossRoom();
void activateBossRoom();
void activateBoss2Room();
void boss1Action();
void boss2Action();
int playerIsNear(int, int);
