/*
 * * * * * * * * * * Random Rogue-Like * * * * * * * * * * * * * * *
 * Текстовый квест с окружением, которое генерится на ходу и случайно.
 * Баги:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <conio.h>
#include <windows.h>
// using namespace std;

#include "objects.h"

int doAction();
void changeLook(char);
void describeEnv();
void enumerateObjs(int,int,int);

void genNewRandomObj(int,int);
void genAllEnv(int, int);
void genNewObj(int,int,int);
void genSecretCells();
void genObjectsAtRandomPos();
void genCycle(int,int);

void triggerSecretCell();

void goForward();
void takeItem();
void showInventory();
void attack();
void breakTheWall();
void displayAbsMap();
void initStatusMsgs();

int secretColHere(int,int);

void ratsAttack();
void ratAttacked(int,int);
void bossAttacked();
void decayDeadRats();
void generateDeadRats();
void ratLogic();
void fillRatsMap();
void killAllRatsButOne();

void colorprintf(char*);
void createBossRoom();
void activateBossRoom();
void activateBoss2Room();
void boss1Action();
void boss2Action();
void heal();
void processKeys();
int playerIsNear(int,int);

void markGround();

// преобразуем реальные координаты в относительные (относительно центра)
int AbsToRelX(int);
int AbsToRelY(int);

void help();

struct ratStat{
  int life;
  int power;
  int x;
  int y;
};

struct ratStat ratsStats[RATSMAX];
int ratsCount = 0;

struct secretCol{
  int x;
  int y;
  char* pos;
};

struct secretCell{
  int x;
  int y;
};

int secretCount = 0;
struct secretCol secretCols[7];
struct secretCell secretCells[7];
int cellsEnabled = 0; // при нажатии, если это первая плита, увеличивается на 1, и т.д. соответственно порядковому номеру
int weapon2 = 0; // флаг того, что мощное оружие найдено

struct bossStat{
  int life;
  int power;
  int power_range; // разброс силы
  int x;
  int y;
  int type; // 1 - BOSS1, 2 - BOSS2
  int bossAct;
  int actCounter;
};

struct bossStat boss;

struct playerStat{
  int life;
  int power;
  int x;
  int y;
};

struct playerStat gamer;

// названия всяческих объектов
// у игрока номер = 1
// 0 это пустота
  char obj2[] = "пол";
  char obj3[] = "стена";
  int obj3_life = 30;
  char obj4[] = "мутировавшая крыса";
  int obj4_life = 5;
  char obj5[] = "лечилка"; // лечит на 10 пунктов
  char obj6[] = "кирка чтобы ломать стенки";
  int obj6_power = 10;
  char obj7[] = "кинжал";
  int obj7_power = 2;
  char obj8[] = "золото";

// окружение
  int env_u = 0;
  int env_l = 0;
  int env_r = 0;
  int env_d = 0;
  
#define W 5//50
#define H 5//50
  int map[W][H];
  int ratsMap[W][H];
  
// направления
// [0] - x, [1] - y
  int up_[2];
  int down_[2];
  int left_[2];
  int right_[2];

// игрок
  int life = 10000;
  int weapon = 1;
  int kirka = 1; // вместо кирки теперь отбойник. работает на аккумуляторах
  int medkits = 0;
  int gold = 0;
  int p_x = 2;//W/2;
  int p_y = 2;//H/2;
  char look = 'U';
  int power_range = 2;
  int power = 60;
//char action;
  wchar_t action[1];
  int actionInt;
  char* whatWasDone;
  int notAction;

// состояние
// 0, 1, 2, 3
// стоит, идет, атакует, крушит стенку
  int state = 0;
  int isExit;

  HANDLE hConsole;
  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  WORD saved_attributes;
  
int main() {
  setlocale( LC_ALL, "" );
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  srand(time(NULL));
  whatWasDone = (char*)malloc(512);

  printf( "Добро пожаловать в Random Roguelike!!\n");
  printf( " * * * \n");
  getch();
  
  system("cls");
  help();
  getch();
  
  system("cls");
  
  // первоначально герой смотрит "вверх" (или вперед)
  up_[0] = 0; up_[1] = -1;
  down_[0] = 0; down_[1] = 1;
  left_[0] = -1; left_[1] = 0;
  right_[0] = 1; right_[1] = 0;

  system("cls");
  printf("\n\n\nВы открываете глаза и обнаруживаете себя в кромешной тьме. Глаза привыкают, но видимость лишь на расстоянии вытянутой руки...\n\n[Нажмите английскую p]");
    while( getch()!=112 )
          {}
      
  system("cls");
    
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);

  // под игроком в начале игры пол
  map[ mx ][ my ] = GROUND;

  isExit = 0;
  notAction = 1;
  
  //map[ mx+2 ][ my+1 ] = MEDKIT;
  
  //createBossRoom();
  //genSecretCells();
  printf("\n\ngenObjectsAtRandomPos();  BEFORE");
  genObjectsAtRandomPos();
  printf("\n\ngenObjectsAtRandomPos();  AFTER");
  
  while( isExit == 0 )
  {
    markGround();
    decayDeadRats();

    // игрок не совершал действий (например, открыл инвентарь)
    if(notAction != 1){
      ratsAttack();
      boss1Action();
      //boss2Action();
      //ratLogic();
    }
  
    notAction = 0;
    
    if( life <= 0 ){
      printf( "\n\n\nВы были смертельно убиты крысой. Ваша умерщвленная тушка ещё долго будет лежать здесь, в назидание потомкам. Игра окончена.\n[Нажмите английскую p]");
      while( getch()!=112 )
      {}

      break;
    }

    // генерируем окружение случайным образом (слева, справа, впереди)    
    // genAllEnv(p_x, p_y);
    printf( "%s", whatWasDone );
    sprintf( whatWasDone, "" );
    displayAbsMap();

    actionInt = getch();
    
    action[0] = toupper(action[0]);
		system( "cls" );

    processKeys();

    printf( "\n\n" );
  }

  free(whatWasDone);
  system("color");
  
  return 0;
}

void processKeys(){
    // W
    if( actionInt == 119 ){
      state = 1;
      goForward();
    }

    // A
    if( actionInt == 97 ){
      changeLook('L'); // поворот не считается за ход
			//notAction = 1;
    }

    // S
    if( actionInt == 115 ){
      changeLook('D');
      //notAction = 1;
    }

    // D
    if( actionInt == 100 ){
      changeLook('R');
      //notAction = 1;
    }
    
    // E
    if( actionInt == 101 ){
      //state=0;
      takeItem();
    }

    // T
    if( actionInt == 116 ){
      attack();
    }

    // K
    if( actionInt == 107 ){
      breakTheWall();
    }

    // Q
    if( actionInt == 113 ){
      isExit = 1;
			notAction = 1;
    }

    // I
    if( actionInt == 105 ){
      showInventory();
			notAction = 1;
    }

    // ?
    if( actionInt == 63 ){
      help();
			notAction = 1;
      getch();
      system("cls");
    }
    
    // H
    if( actionInt == 104 ){
      heal();
			notAction = 0;
    }
    
    
    
// для отладки    
    // B
    /*
    if( actionInt == 98 ){
      createBossRoom();
      sprintf( whatWasDone, "boss room created!" );
    }
    */
    
    // N
    /*
    if( actionInt == 110 ){
      activateBossRoom();
    }
    */
    
    // P
    /*
    if( actionInt == 112 ){
      generateDeadRats();
    }
    */
    
    // M
/*    
    if( actionInt == 109 ){
      //activateBoss2Room();
      killAllRatsButOne();
    }
  */  
}

// идти вперед
void goForward(){
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  
  int newx = UX();
  int newy = UY();
  
  printf("newx = %d, newy = %d\n",newx,newy);
  
  if( newx < W && newx >=0 && newy >= 0 && newy < H ){
  
    int onTheWay = map[ UX() ][ UY() ];
    int ratThere = isRatThere(UX(), UY());
    int ratLife = 0;
    if( ratThere!=-1)
      ratLife = ratsStats[ratThere].life;
    
    // ограничение в сарае первого босса
    if( UY() == BRY+7 && boss.bossAct && boss.type==1){
      sprintf(whatWasDone, "Что-то мешает вам пройти дальше. Попробуйте избавиться от местного обитателя.\n");
    }
    else{
      // если впереди нет препятствий (в виде крыс или стен)
      if( (onTheWay != WALL) && (ratLife<=0) &&
          (onTheWay != WALL2) && (onTheWay != BOSS1)// &&
      //    (onTheWay != BOSS2)
      ){
        
        // идем
        p_x += up_[0];
        p_y += up_[1];
        
        int sCol = secretColHere(p_x, p_y);
        if( sCol!= -1 ){
          sprintf(whatWasDone, "%s Здесь есть какие-то цифры: %s", whatWasDone, secretCols[sCol].pos);
        }        
        
        triggerSecretCell();       
        takeItem();
      }
    }
  }
  else{
    notAction = 1;
  }
}

// создать объекты вокруг
// x и y - точка, вокруг которой будут создаваться объекты (относительные координаты)
void genAllEnv(int x, int y){
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(x);
  int my = AbsToRelY(y);
  int relx = 0;
  int rely = 0;
  
  relx = mx + up_[0];    rely = my + up_[1];
  genNewRandomObj(relx, rely); // голова
    
  relx = mx + left_[0];    rely = my + left_[1];
  genNewRandomObj(relx, rely); // левый бок

  relx = mx + right_[0];    rely = my + right_[1];
  genNewRandomObj(relx, rely); // правый бок

  relx = mx + down_[0];    rely = my + down_[1];
  genNewRandomObj(relx, rely); // спина
}

// Сгенерировать новый объект по указанным координатам
void genNewRandomObj(int relx, int rely){
  // вероятность создания предмета
  if( rand()%GENCHANCE == 1 ){
    int newObj = rand()%7+2;
    if( map[ relx ][ rely ] == 0 ){
      map[ relx ][ rely ] = newObj; // голова
      if(newObj==RAT){
        ratsStats[ratsCount].x = relx;
        ratsStats[ratsCount].y = rely;
        ratsStats[ratsCount].life = 12+rand()%7;
        ratsStats[ratsCount].power = rand()%10+4;
        ratsCount++;
      }
    }
  }
  else{
    map[ relx ][ rely ] = GROUND;
  }
}

// Создает объект с указанным номером и по указанным координатам
void genNewObj(int objNum, int ox, int oy){
  map[ ox ][ oy ] = objNum;
  if( objNum == RAT ){
    map[ ox ][ oy ] = EMPTINESS;
    ratsStats[ratsCount].x = ox;
    ratsStats[ratsCount].y = oy;
    ratsStats[ratsCount].life = 12+rand()%7;
    ratsStats[ratsCount].power = rand()%10+4;
    ratsCount++;
    //printf("\nRAT - x:%d y:%d",ox,oy);
  }
}

// создаёт все игровые предметы: крысы, лечилки, колонны, аккумуляторы. Выполняется перед началом игры.
void genObjectsAtRandomPos(){
  genCycle(80, RAT);
/*
  genCycle(RATSMAX, RAT);
  genCycle(MEDSMAX, MEDKIT);
  genCycle(BATTMAX, DAGGER);
  genCycle(COLSMAX, WALL);
  genCycle(GOLDMAX, GOLD);
  */
}

// цикл создания случайных координат указанных объектов до максимума
// max - количество циклов генерирования
void genCycle(int max, int objNum){
  int genCount = 0; // количество попыток создания объекта
  while(1){
    int nx = rand()%W;
    int ny = rand()%H;
    if( map[ nx ][ ny ] != EMPTINESS )
      continue;
    
    if(objNum==RAT)
      if(isRatThere(nx,ny)!=-1)
        continue;
    
    if( rand()%GENCHANCE == 1 )
      genNewObj(objNum, nx, ny);
    
    genCount++;
    if( genCount == max )
      break;
  }
}

// сменить направление, куда смотрит герой
void changeLook(char where){
  // повернул налево
  if(where == 'L'){
    // если смотрел "вверх"
    if(look == 'U'){
      look = 'L';
    }
    else
      if(look == 'L'){
        look = 'D';
      }
      else
        if(look == 'D'){
          look = 'R';
        }
        else
          if(look == 'R'){
            look = 'U';
          }
  }
  // направо
  if(where == 'R'){
    if(look == 'U'){
      look = 'R';
    }
    else
      if(look == 'R'){
        look = 'D';
      }
      else
        if(look == 'D'){
          look = 'L';
        }
        else
          if(look == 'L'){
            look = 'U';
          }
  }

  // развернулся
  if(where == 'D'){
    if(look == 'U'){
      look = 'D';
    }
    else
      if(look == 'D'){
        look = 'U';
      }
      else
        if(look == 'R'){
          look = 'L';
        }
        else
          if(look == 'L'){
            look = 'R';
          }
  }

  switch(look){
    case 'U':
      up_[0] = 0; up_[1] = -1;
      down_[0] = 0; down_[1] = 1;
      left_[0] = -1; left_[1] = 0;
      right_[0] = 1; right_[1] = 0;
      //sprintf(whatWasDone, "Вы идёте вперёд.\n");
      break;
    case 'L':
      up_[0] = -1; up_[1] = 0;
      down_[0] = 1; down_[1] = 0;
      left_[0] = 0; left_[1] = 1;
      right_[0] = 0; right_[1] = -1;
      //sprintf(whatWasDone, "Вы повернули налево.\n");
      break;
    case 'D':
      up_[0] = 0; up_[1] = 1;
      down_[0] = 0; down_[1] = -1;
      left_[0] = 1; left_[1] = 0;
      right_[0] = -1; right_[1] = 0;
      //sprintf(whatWasDone, "Вы развернулись назад.\n");
      break;
    case 'R':
      up_[0] = 1; up_[1] = 0;
      down_[0] = -1; down_[1] = 0;
      left_[0] = 0; left_[1] = -1;
      right_[0] = 0; right_[1] = 1;
      //sprintf(whatWasDone, "Вы повернули направо.\n");
      break;
    default:
      break;
  }
}

// будет пробегать по массиву крыс и РАЗЛАГАТЬ их
void decayDeadRats(){
  int i=0;
  for(i=0; i<ratsCount; i++){
    if( ratsStats[i].life <= 0 ){
      if( ratsStats[i].life > -50 ){
        ratsStats[i].life -= rand()%3 + 1;
      }
    }
  }
}

// Программа крыс.
// Если через 2 клетки появился игрок, то крыса движется к нему и атакует.
// TODO: Она проходит 2 клетки, останавливается и потом снова идет.
void ratLogic(){
  int i;
  for(i=0; i<ratsCount; i++){
    // впереди игрока
    if( p_x == ratsStats[i].x &&
        (p_y - ratsStats[i].y)<=3 &&
        p_y > ratsStats[i].y
        ){
        ratsStats[i].y++;
    }
    
    // сзади игрока
    if( p_x == ratsStats[i].x &&
        (ratsStats[i].y - p_y)<=3 &&
        p_y < ratsStats[i].y
        ){
        ratsStats[i].y--;
    }
    
    // слева игрока
    if( p_y == ratsStats[i].y &&
        (p_x - ratsStats[i].x)<=3 &&
        p_x > ratsStats[i].x
        ){
        ratsStats[i].x++;
    }
    
    // справа игрока
    if( p_y == ratsStats[i].y &&
        (ratsStats[i].x - p_x)<=3 &&
        p_x < ratsStats[i].x
        ){
        ratsStats[i].x--;
    }    
  }
}

// заполнение карты крыс.
// Сначала рисуется обычная карта, а потом карта крыс.
void fillRatsMap(){
  int i;
  int j;
  
  for(i=0; i<W; i++){
    for(j=0; j<H; j++){
      ratsMap[i][j] = 0;
    }
  }
  
  for(i=0; i<ratsCount; i++){
    if( ratsStats[i].life <= 0 ){
      ratsMap[ ratsStats[i].x ][ ratsStats[i].y ] = DEADRAT;
    }
    else{
      ratsMap[ ratsStats[i].x ][ ratsStats[i].y ] = RAT;
    }
  }
}

// Атака окружающих крыс
void ratsAttack(){
	int attackPower = 0;
	int i;
  int charge = 0;
  
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelX(p_y);
  
  for(i=0; i<ratsCount; i++){
    if( ratsStats[i].life <= 0 )
      continue;
    
    // ahead
    if( UX() == ratsStats[i].x && UY() == ratsStats[i].y ){
      charge = 1;
    }
    // left
    if( LX() == ratsStats[i].x && LY() == ratsStats[i].y ){
      charge = 1;
    }
    // right
    if( RX() == ratsStats[i].x && RY() == ratsStats[i].y ){
      charge = 1;
    }
    // behind
    if( DX() == ratsStats[i].x && DY() == ratsStats[i].y ){
      charge = 1;
    }
    // under feet
    if( CX() == ratsStats[i].x && CY_() == ratsStats[i].y ){
      charge = 1;
    }
    
    //printf("DEBUG: charge = %d\n", charge);
    
    if(charge){
      attackPower += rand()%ratsStats[i].power + 1;
      charge = 0;
    }
  }

	if( attackPower > 0 ){
		life -= attackPower;
		printf( "Вы получили урон %d от крыс.\n", attackPower );
	}
}

// Написать, что впереди
int whatsAhead(){
    // преобразуем в реальные (матричные) координаты
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // Впереди
//    printf( "Перед вами ");
    return map[ mx+up_[0] ][ my+up_[1] ];
}

// что слева
int whatsLeft(){
    // преобразуем в реальные (матричные) координаты
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // Слева
//    printf("Слева от вас ");
	return map[ mx+left_[0] ][ my+left_[1] ];
}

// что справа
int whatsRight(){
    // преобразуем в реальные (матричные) координаты
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // Справа
//    printf("Справа от вас ");
    return map[ mx+right_[0] ][ my+right_[1] ];
}

// что под ногами
int whatsUnderFeet(){
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // Справа
  //    printf("Под ногами: ");
  return map[ mx ][ my ];
}

int whatsBehind(){
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // Справа
  //    printf("Сзади вас ");
  return map[ mx+down_[0] ][ my+down_[1] ];
}

// Пишет название объекта по его номеру
void enumerateObjs(int n, int lookx, int looky){
  int ratHere = isRatThere(lookx, looky);
  
  if( ratHere!=-1 ){
    if( ratsStats[ratHere].life <= 0 ){
      printf("мертвая крыса. ");
      if( ratsStats[ratHere].life <= -30 ){
        printf("Видны палёные печатные платы");
      }
    }
    else{
      printf("Дрон");
    }        
  }
  else{
  
    switch(n){
      case EMPTINESS: printf( "Пустота" );
        break;
      case GROUND: printf("%s",obj2);
        break;
      case WALL: printf("%s",obj3);
        break;
        
      case RAT:
          ratHere = isRatThere(lookx, looky);
        break;
      
      case DEADRAT:
        break;
      
      case MEDKIT: printf("%s",obj5);
        break;
      case PICK: printf("%s",obj6);
        break;
      case WALL2: printf("непробиваемая стена");
        break;    
      case DAGGER: printf("%s",obj7);
        break;
      case GOLD: printf("%s",obj8);
        break;
      case BOSS1:
        printf("Дрон RAT-1000");
        break;
      case BOSS2:
        printf("Дрон RAT-S");
        break;
      default:
        printf("нечто");
        break;
    }
  }
}

// взять предмет под ногами
void takeItem(){
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // берем что под ногами
  switch( map[mx][my] ){
/*    case GROUND:
      sprintf(whatWasDone, "Вы попытались унести с собой кусок пола, но ничего из этой затеи не вышло.\n");
      break;*/
    case WALL:
      sprintf(whatWasDone, "Что вы делаете, стоя на стене??\n");
      break;
    case RAT:
      sprintf(whatWasDone, "Как вам удалось забраться на крысу?!\n");
      break;
    case MEDKIT:
      sprintf(whatWasDone, "Вы взяли лечилку\n");
      medkits++;
      map[mx][my] = GROUND;
      break;
    case PICK:
      sprintf(whatWasDone, "Вы взяли кирку\n");
      kirka++;
      map[mx][my] = GROUND;
      break;
    case DAGGER:
      sprintf(whatWasDone, "Вы взяли аккумулятор\n");
      weapon++;
      map[mx][my] = GROUND;
      break;
    case GOLD:
      sprintf(whatWasDone, "Вы разжились 10-ю кусками золота.\n");
      gold += 10;
      map[mx][my] = GROUND;
      break;
    default:
//      sprintf(whatWasDone, "Тут ничего не взять :(\n");
      break;
  }
}

// крушить стенку впереди
void breakTheWall(){
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);

  if( map[ mx+up_[0]] [my+up_[1] ] == WALL ){
    if(state!=3){
      state = 3;
      obj3_life = 30;
    }

    map[ UX() ][ UY() ] = GROUND;
    sprintf(whatWasDone, "Колонна рассыпалась в пыль.\n");
    
    int i;
    for(i=0; i<secretCount; i++){
      if( UX() == secretCols[i].x && UY() == secretCols[i].y ){
        sprintf(whatWasDone, "%sЗдесь есть какие-то цифры:\n%s",whatWasDone, secretCols[i].pos);
      }
    }
  }
  else{
    sprintf(whatWasDone, "Кирка нужна только для долбления стенок.\n");
  }
}

// атаковать впереди стоящего
void attack(){
  // преобразуем в относительные координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  int newx = UX();
  int newy = UY();
  int ratThere = isRatThere(newx,newy);
  
  if( ratThere!=-1 || 
      map[ newx ][ newy ] == BOSS1 /*||
      map[ newx ][ newy ] == BOSS2*/ ){
    
    ratAttacked(newx, newy);
    bossAttacked();
    
  }
  else{
    sprintf(whatWasDone, "Вы ткнули ножиком воображаемого врага.\n");
  }
}

// атака на босса
void bossAttacked(){
  if( boss.x == UX() && boss.y == UY() ){
    if( boss.life > 0 ){
      int attackPow = rand()%power_range+power+1;
      boss.life -= attackPow;
      sprintf(whatWasDone, "Вы ударили дрона ножиком, с уроном %d. Дрон: %d\n", attackPow, boss.life);
      if( boss.life <= 0){
        boss.life = 0;
        boss.bossAct = 0;
        if( boss.type == 1 ){
        /*
          sprintf(whatWasDone, "Враг побежден! Путь вперед свободен.\n", attackPow);
          map[ boss.x ][ boss.y ] = EMPTINESS;
          */
          //boss.type = 2;
          
          system("cls");
          
          printf("\n\n\nВнутри скоростного дрона что-то взорвалось с резким хлопком. Он остановился и застыл. Сквозь обшивку пробился черный дым, и тут же пропал. Стены подводной ловушки стали опускаться, сквозь расширяющуюся щель между потолком и стенами стал пробиваться свет. На поверхности был солнечный день. Пора отправляться домой.\n\n[Нажмите английскую p]");
          
          while( getch()!=112 )
          {}
          
          isExit = 1;
        }
        else{
        // ну не успел я сделать второго босса :(
        /* 
          sprintf(whatWasDone, "Внутри скоростного дрона что-то взорвалось с резким хлопком. Он остановился и застыл. Сквозь обшивку пробился черный дым, и тут же пропал. Стены подводной ловушки стали опускаться, сквозь расширяющуюся щель между потолком и стенами стал пробиваться свет. На поверхности был солнечный день. Пора отправляться домой.\n", attackPow);
          */
        }
      }
    }
  }
}

// Возвращает номер крысы в массиве ratsStats с указанными координатами
int isRatThere(int x, int y){
  int i=0;
  while(i<ratsCount){
    if(ratsStats[i].x==x && ratsStats[i].y==y)
      return i;
    i++;
  }
  return -1;
}

// атака на крысу
void ratAttacked(int newx, int newy){
    // находим ЖИВУЮ крысу
    int ratHere = isRatThere(newx, newy);
    // TODO: рассчитать вероятность удара (случайное число)
    // можно промахнуться
    if(state!=2){
      state = 2;
    }

    if( ratHere >= 0 ){
      // нашли крысу
      if( ratsStats[ratHere].life > 0 ){
        int attackPow = rand()%power_range+power+1;
        ratsStats[ratHere].life -= attackPow;
        sprintf(whatWasDone, "Вы ударили крысу ножиком, с уроном %d.\n", attackPow);
        if( ratsStats[ratHere].life <= 0 ){
          ratsStats[ratHere].life = 0;
          //map[ newx ][ newy ] = DEADRAT;
          sprintf(whatWasDone, "Крыса была повержена.\n", attackPow);
        }
        
        // проверяем количество мертвых крыс и активируем босса
        int i;
        int liveRats=0;
        for(i=0; i<ratsCount; i++){
          if( ratsStats[i].life > 0 )
            liveRats++;
        }
        
        if( liveRats == 0 ){
          activateBossRoom();
          system("cls");
          printf("\n\n\nПоследняя крыса деактивирована. Открылась новая комната. Интересно, что же там хранится...\n\n[Нажмите английскую p]");
          
          while( getch()!=112 )
          {}

          system("cls");
        }
        
        // TODO: крыса разлагается в decayDeadRats()
      }
      else{
        sprintf(whatWasDone, "Крыса уже не работает.\n");
      }
    }
}

void killAllRatsButOne(){
  int i=0;
  for(i=0; i<ratsCount-1; i++){
    ratsStats[i].life = 0;
  }
  ratsStats[ratsCount-1].x = p_x;
  ratsStats[ratsCount-1].y = p_y - 2;
}

// применить лечилку
void heal()
{
  if(medkits>0){
    life += 20;
    medkits--;
    sprintf(whatWasDone, "Вы чувствуете мощный прилив сил. Костюм восстановлен.");
  }
  else{
    sprintf(whatWasDone, "Лечилки кончились:(\n");
  }
}

// показать инвентарь
void showInventory(){
  system("cls");
  printf("----------------------------------\n");
  printf("У вас в бауле мирно соседствуют\n");
  printf("Лечилки : %d\n", medkits );
  printf("Аккумуляторы : %d\n", weapon );
  //printf("Кирки : %d\n", kirka );
  printf("Золото : %d\n", gold );
  //printf("И еще мешочек семок.\n");
  printf("----------------------------------\n");
  printf("[Нажмите английскую p]");
          
          while( getch()!=112 )
          {}
          
  system("cls");
}

int AbsToRelX(int x){
  return x;//+25;
}

int AbsToRelY(int y){
  return y;//+25;
}

void help(){
  printf( "---------------------------------------\n");
  printf( "Вы - глубоководник. Прочный водолазный костюм, с возможностью восстановления, также служит вам броней. Вы находитесь в вязкой среде с плохой видимостью. Передвижение затруднено. Вам нужно собрать золото и найти выход наружу. Вам будут мешать спятившие подводные дроны. При наличии аккумуляторов при себе, есть возможность вывести их из строя. Разбивайте стены отбойником. Собирайте золото, аккумуляторы и лечилки. Останавливайте дронов. Поворот и разворот считаются за действие. Удачи!\n\n");
  printf( "Управление:\n");
  printf( "? - эта справка\t\t\ti - инвентарь \n");
  printf( "w - идти вперед\t\t\ta - развернуться налево\n");
  printf( "s - развернуться назад\t\td - развернуться направо\n");
  printf( "e - взять предмет\t\tt - бить разрядом\n");
  printf( "k - ломать стенку отбойником\th - использовать лечилку\n");
  printf( "q - выход\n" );
  printf( "---------------------------------------\n");
  printf( "\n" );
}

int isLookUp(){
  if( (up_[0]==0)&&(up_[1]==-1) )
    return 1;
  return 0;
}

int isLookDown(){
  if( (up_[0]==0)&&(up_[1]==1) )
    return 1;
  return 0;
}

int isLookLeft(){
  if( (up_[0]==-1)&&(up_[1]==0) )
    return 1;
  return 0;
}

int isLookRight(){
  if( (up_[0]==1)&&(up_[1]==0) )
    return 1;
  return 0;
}

// новая версия карты (игрок всегда в центре, карта занимает 80на8 клеток)
// центр 39:4
// используется буфер экрана(двумерный массив)
void displayMap(){
  GetConsoleScreenBufferInfo(hConsole, &coninfo);
  coninfo.dwCursorPosition.Y = 6;    // move up one line
  coninfo.dwCursorPosition.X = 0;    // move to the right the length of the word
  SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);
  // экранные координата начала рисования карты
  int centerX = 39;
  int centerY = 4;
  int mapMaxX = 80;
  int mapMaxY = 8;
  int scrBuf[mapMaxX][mapMaxY];
  // начало области отображения на экране (левый верхний угол)
  int scrSX = centerX - p_x - 1;
  int scrSY = centerY - p_y;
  // видимая область карты (с левого верхнего угла)
  int mapSX=0;
  int mapSY=0;
  
  if(scrSX<0)
    mapSX=mapMaxX-scrSX;
  if(scrSY<0)
    mapSY=mapMaxY-scrSY;
  
  int i=0;
  int j=0;
  for(i=0;i<mapMaxX;i++){
    for(j=0;j<mapMaxY;j++){
      scrBuf[i][j]=0;
    }
  }
  
}

// Карта отображается в абсолютных координатах (не поворачивается за игроком)
void displayAbsMap(){
  fillRatsMap();
  // преобразуем в реальные (матричные) координаты
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // какой ужасный код...
  int i = 0;
  int j = 0;
    
  GetConsoleScreenBufferInfo(hConsole, &coninfo);
  coninfo.dwCursorPosition.Y = 6;    // move up one line
  coninfo.dwCursorPosition.X = 0;    // move to the right the length of the word
  SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);
  
  for(i = 0; i < 5; i++){
  
    if( (i==2) && (isLookUp()==1) ){
      printf("     ^");
    }
        
    if( (i==3) && (isLookDown()==1) ){
      printf("     v");
    }

    printf( "\n" );
    for(j = 0; j < 5; j++){
  
      if( (i == 2) && (j == 2) && isLookLeft() ){
        printf( "<" );
			}
			else{
        if( (i == 2) && (j == 3) && isLookRight() ){
          printf( ">" );
        }
        else{
          printf( " " );
        }
      }
      
      if( (
            ( (mx - 2 + j) < W ) &&
            ( (mx - 2 + j) > 0  )
          )
          &&
          ( 
            ( (my - 2 + i) < H ) &&
            ( (my - 2 + i) > 0  )
          )
        ){
          if( (i == 2) && (j == 2) ){// рисуем игрока
            printf( "@" );
        }
        else{
          if( ratsMap[ mx - 2 + j][ my - 2 + i]!= 0 ){
              // крысы
              switch( ratsMap[ mx - 2 + j][ my - 2 + i] ){
                case DEADRAT:
                  printf( "D" );
                  break;
                case RAT:
                  printf( "R" );
                  break;
                case 0:
                  printf( "0" );
                  break;
                default:
                  printf( "r" );
                  break;
              }
          }
          else{
        
            // предметы
            switch( map[ mx - 2 + j][ my - 2 + i] ){
              case EMPTINESS: // пустота
                printf( " " );
                break;
              case GROUND: // пол
                printf( "_" );
                break;
              case WALL: // стена
                printf( "#" );
                break;
              case WALL2:
                printf( "%%" );
                break;
                
              case RAT: // крыса
                break;
              case DEADRAT:
                break;
                
              case BOSS1:
                printf( "B" );
                break;
              case BOSS2:
                printf( "S" );
                break;
              case MEDKIT: // лечилка
                printf( "+" );
                break;
              case PICK: // кирка
                printf( "?" );
                break;
              case DAGGER: // кинжал
                printf( "!" );
                break;
              case GOLD: // золото
                printf( "$" );
                break;
              case CELL:
                printf( "." );
                break;
              default:
                printf( "%d", map[ mx - 2 + j ][ my - 2 + i ] );
                break;
            }
          }
        }
      }
      else{
        printf( "%%" );
      }
            //printf( " " );
    }
		
    printf("  ");
// пишем сюда статус игрока
    switch( i ){
      case 0:
        printf( "Впереди:\t " );
        enumerateObjs( whatsAhead(), UX(),  UY() );
        break;
      case 1:
        printf( "Слева:\t " );
        enumerateObjs( whatsLeft(), LX(), LY() );
        break;
      case 2:
        printf( "Справа:\t " );
        enumerateObjs( whatsRight(), RX(), RY() );
        break;
      case 3: 
        printf("Жизни:\t %d", life);
        break;
      case 4:
        printf( "Под ногами:\t " );
        enumerateObjs( whatsUnderFeet(), CX(), CY_() );
        break;
      default:
        break;
    }

    printf( "\n" );
  }
//  printf("\nDEBUG: ratsCount = %d\n", ratsCount);
//  printf("x: %d  y: %d\n", p_x, p_y);
  printf("\nВведите букву действия: ");
}

// программа босса 1.
void boss1Action(){
  if( boss.bossAct == 0 ){
    if( p_y == (BRY+8) && (p_x >= (BRX+1) && 
        p_x <= (BRX+3)) &&
        boss.type == 1 
      ){
//      printf("boss1 activated!\n");
      boss.bossAct = 1;
      int i = 0;
      // закрываем путь к отступлению
      for(i=0; i<5; i++){
        map[ BRX+i ][ BRY+9 ] = WALL2;
      }
      
      system("cls");
      
      printf("\n\n\nНа месте пространства сзади вас резко и с грохотом опустилась длинная непробиваемая стена. Это ловушка!\n\n[Нажмите английскую p]");
          
          while( getch()!=112 )
          {}
      system("cls");
    }
  }
  
  if( boss.bossAct && boss.type == 1 ){
    map[ boss.x ][ boss.y ] = GROUND;
    // идти к игроку или атаковать
    if( boss.actCounter >= 3 ){
      // супер-атака ("каток")
      switch( boss.actCounter ){
        case 3:
          boss.y-=1;
          break;
        case 4:
          boss.y-=2;
          break;
        case 5:
          boss.y+=2;
          break;
        case 6:
          boss.y+=2;
          // если на этом месте игрок, он теряет предметы
          // и жизни.
          if( boss.x == p_x && boss.y == p_y ){
            life -= rand()%boss.power_range+40+1;
            kirka -= rand()%2+2+1;
            weapon -= rand()%2+2+1;
            medkits -= 1;
          }
          break;
        case 7:
          boss.y-=1;
          boss.actCounter = 0;
          break;
        default:
          break;
      }

    }
    else{
      if( boss.x == p_x ){
        // атака
        life -= rand()%boss.power_range+boss.power+1;
      }
      else{
        // идти в сторону игрока
        boss.x += p_x - boss.x;
      }
    }
    
    map[ boss.x ][ boss.y ] = BOSS1;
    boss.actCounter++;
  }
}

// программа босса 2. Жрать других, драться и двигаться прыжками через клетку.
void boss2Action(){
  if( boss.bossAct == 0 && boss.type == 2){
    if( p_y == (BRY+2) && p_x == (BRX+2) &&
        boss.type == 2 ){
        boss.bossAct = 1;
        printf("boss2 activated!\n");

    }
  }
  
  if( boss.bossAct == 1 && boss.type == 2){
    if( playerIsNear( boss.x, boss.y ) ){
      life -= rand()%boss.power_range+boss.power+1;
    }
  }
}

// создание дохлых крыс по всей карте.
void generateDeadRats(){
  int i=0;
  int j=0;
  for(i=0; i<W; i++){
    for(j=0; j<H; j++){
      if( (rand()%30+1) == 3 && ratsCount < RATSMAX){
        map[ i ][ j ] = RAT;
        ratsStats[ratsCount].x = i;
        ratsStats[ratsCount].y = j;
        ratsStats[ratsCount].life = 0;
        ratsStats[ratsCount].power = 4;
        ratsCount++;
      }
    }
  }
  
  printf("ratsCount = %d\n", ratsCount);
}

// Игрок на соседней клетке
int playerIsNear(int ex, int ey){
  if( UX() == ex && UY() == ey ){
      return 1;
  }
  
  if( DX() == ex && DY() == ey ){
      return 1;
  }
  
  if( LX() == ex && LY() == ey ){
      return 1;
  }
  
  if( RX() == ex && RY() == ey ){
      return 1;
  }
  
  if( CX() == ex && CY_() == ey ){
      return 1;
  }
  
  return 0;
}

// создает на карте сарай с боссом. Выполняется только один раз,
// в начале игры.
// Если крысы живы, то на карте это просто стены, непробиваемые.
// Если крысы убиты, то открывается вход в этот сарай.
void createBossRoom(){
  int i=0;
  int j=0;
  
  for(i=0; i<5; i++){
    for(j=0; j<10; j++){
      map[ BRX+i ][ BRY+j ] = WALL2;
    }
  }
}

// открывает комнату с боссом, создаёт босса и пол
void activateBossRoom(){
  // крест
  map[ BRX+2 ][ BRY+1 ] = GROUND;
  map[ BRX+1 ][ BRY+2 ] = GROUND;
  map[ BRX+2 ][ BRY+2 ] = GROUND;
  map[ BRX+2 ][ BRY+3 ] = GROUND;
  map[ BRX+3 ][ BRY+2 ] = GROUND;
  
  // комната
  int i=0;
  int j=0;
  for(i=0; i<3; i++){
    for(j=0; j<5; j++){
      map[ BRX+1+i ][ BRY+4+j ] = GROUND;
    }
  }
  
  for(i=0; i<5; i++){
    map[ BRX+i ][ BRY+9 ] = GROUND;
  }
  
  map[ BRX+2 ][ BRY+7 ] = BOSS1;
  boss.x = BRX+2;
  boss.y = BRY+7;
  boss.type = 1;
  boss.life = 300;
  boss.power = 20;
  boss.power_range = 10;
  boss.bossAct = 0;
  boss.actCounter = 0;
}

// открывает второго босса
void activateBoss2Room(){
  // убираем сарай
  int i=0;
  int j=0;        
  for(i=0; i<5; i++){
    for(j=0; j<10; j++){
      map[ BRX+i ][ BRY+j ] = GROUND;
    }
  }
  
  map[ p_x ][ p_y ] = PLAYER;
  map[ BRX+2 ][ BRY+1 ] = BOSS2;

  boss.x = BRX+2;
  boss.y = BRY+1;
  boss.type = 2;
  boss.life = 600;
  boss.power = 20;
  boss.power_range = 15;
  boss.bossAct = 0;
  boss.actCounter = 0;
}

// Создает несколько плит-триггеров, пройдя по которым в нужном порядке, можно открыть мощное оружие (на последней плите). Координаты нужных плит и порядковые номера спрятаны в нескольких колоннах (их нужно разбить)
void genSecretCells(){
  int i;
  int c = rand()%3+3;

  int k=0;
  while(1){
    int sx = rand()%W;
    int sy = rand()%H;
        
    k++;
    int ratThere = isRatThere(sx,sy);
    
    if( map[ sx ][ sy ] != EMPTINESS && ratThere>=0)
      continue;

    secretCells[secretCount].x = sx;
    secretCells[secretCount].y = sy;
    
    
    map[ sx ][ sy ] = CELL;
    
    secretCount++;
    if( secretCount == c )
      break;
  }
  
  k = 0;
  int cCount = 0;
  
  while(1){
    int cx = rand()%W;
    int cy = rand()%H;
  
    k++;
    int ratThere1 = isRatThere(cx,cy);
    
    if( map[ cx ][ cy ] != EMPTINESS && ratThere1>=0)
      continue;

    secretCols[cCount].x = cx;
    secretCols[cCount].y = cy;
    secretCols[cCount].pos = (char*)malloc(7);
    sprintf(secretCols[cCount].pos, "%d;%d;%d",
    secretCells[cCount].x,secretCells[cCount].y,cCount+1);

    //printf("pos = %s\n", secretCols[cCount].pos);
    
    map[ cx ][ cy ] = WALL;

    cCount++;
    if( cCount == c )
      break;
  }
  
  //getch();
}

int secretColHere(int x, int y){
  int i=0;
  while(i<secretCount){
    if(secretCols[i].x==x && secretCols[i].y==y)
          return i;
    i++;
  }
  return -1;
}

// обработка события, когда игрок наступил на плиту
void triggerSecretCell(){
  if( cellsEnabled != secretCount ){
    int i;
    for( i=0; i<secretCount; i++ ){
      if( secretCells[i].x == p_x &&
          secretCells[i].y == p_y ){

          sprintf(whatWasDone, "Плита под ногами ощутимо опустилась по щиколотку.\n");
        
        if( i == cellsEnabled ){
          cellsEnabled++;
          break;
        }
        else{
          cellsEnabled = 0;
        }
        
      }
    }
    
    if( cellsEnabled == secretCount ){
      system("cls");
      printf( "\n\n\nЧасть пола перед вами выросла в небольшой параллелепипед. Это импульсная пушка! Она не помешает в борьбе с дронами.\n\n[Нажмите английскую p]");
          
          while( getch()!=112 )
          {}
      power = 50;
      power_range = 30;
      system("cls");
    }
  }
}

// координата впереди по x
int UX(){
  return AbsToRelX(p_x)+up_[0];
}

// координата впереди по y
int UY(){
  return AbsToRelY(p_y)+up_[1];
}

int DX(){
  return AbsToRelX(p_x)+down_[0];
}

int DY(){
  return AbsToRelY(p_y)+down_[1];
}

int LX(){
  return AbsToRelX(p_x)+left_[0];
}

int LY(){
  return AbsToRelY(p_y)+left_[1];
}

int RX(){
  return AbsToRelX(p_x)+right_[0];
}

int RY(){
  return AbsToRelY(p_y)+right_[1];
}

// под ногами
int CX(){
  return AbsToRelX(p_x);
}

int CY_(){
  return AbsToRelY(p_y);
}

// отмечает клетки вокруг героя как GROUND
void markGround(){
  if( UY() >= 0 )
    if( map[ p_x ][ UY() ] == EMPTINESS )
      map[ p_x ][ UY() ] = GROUND;
    
  if( DY() < H )
    if( map[ p_x ][ DY() ] == EMPTINESS )
      map[ p_x ][ DY() ] = GROUND;
    
  if( LX() >= 0 )
    if( map[ LX() ][ p_y ] == EMPTINESS )
      map[ LX() ][ p_y ] = GROUND;
    
  if( RX() < W )
    if( map[ RX() ][ p_y ] == EMPTINESS )
      map[ RX() ][ p_y ] = GROUND;
}