
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