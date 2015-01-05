/*
 * * * * * * * * * * Random Rogue-Like * * * * * * * * * * * * * * *
 * ��������� ����� � ����������, ������� ��������� �� ���� � ��������.
 * ����:
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

// ����������� �������� ���������� � ������������� (������������ ������)
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
int cellsEnabled = 0; // ��� �������, ���� ��� ������ �����, ������������� �� 1, � �.�. �������������� ����������� ������
int weapon2 = 0; // ���� ����, ��� ������ ������ �������

struct bossStat{
  int life;
  int power;
  int power_range; // ������� ����
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

// �������� ��������� ��������
// � ������ ����� = 1
// 0 ��� �������
  char obj2[] = "���";
  char obj3[] = "�����";
  int obj3_life = 30;
  char obj4[] = "������������ �����";
  int obj4_life = 5;
  char obj5[] = "�������"; // ����� �� 10 �������
  char obj6[] = "����� ����� ������ ������";
  int obj6_power = 10;
  char obj7[] = "������";
  int obj7_power = 2;
  char obj8[] = "������";

// ���������
  int env_u = 0;
  int env_l = 0;
  int env_r = 0;
  int env_d = 0;
  
#define W 5//50
#define H 5//50
  int map[W][H];
  int ratsMap[W][H];
  
// �����������
// [0] - x, [1] - y
  int up_[2];
  int down_[2];
  int left_[2];
  int right_[2];

// �����
  int life = 10000;
  int weapon = 1;
  int kirka = 1; // ������ ����� ������ ��������. �������� �� �������������
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

// ���������
// 0, 1, 2, 3
// �����, ����, �������, ������ ������
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

  printf( "����� ���������� � Random Roguelike!!\n");
  printf( " * * * \n");
  getch();
  
  system("cls");
  help();
  getch();
  
  system("cls");
  
  // ������������� ����� ������� "�����" (��� ������)
  up_[0] = 0; up_[1] = -1;
  down_[0] = 0; down_[1] = 1;
  left_[0] = -1; left_[1] = 0;
  right_[0] = 1; right_[1] = 0;

  system("cls");
  printf("\n\n\n�� ���������� ����� � ������������� ���� � ��������� ����. ����� ���������, �� ��������� ���� �� ���������� ��������� ����...\n\n[������� ���������� p]");
    while( getch()!=112 )
          {}
      
  system("cls");
    
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);

  // ��� ������� � ������ ���� ���
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

    // ����� �� �������� �������� (��������, ������ ���������)
    if(notAction != 1){
      ratsAttack();
      boss1Action();
      //boss2Action();
      //ratLogic();
    }
  
    notAction = 0;
    
    if( life <= 0 ){
      printf( "\n\n\n�� ���� ���������� ����� ������. ���� ������������ ����� ��� ����� ����� ������ �����, � ��������� ��������. ���� ��������.\n[������� ���������� p]");
      while( getch()!=112 )
      {}

      break;
    }

    // ���������� ��������� ��������� ������� (�����, ������, �������)    
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
      changeLook('L'); // ������� �� ��������� �� ���
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
    
    
    
// ��� �������    
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

// ���� ������
void goForward(){
  // ����������� � �������� (���������) ����������
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
    
    // ����������� � ����� ������� �����
    if( UY() == BRY+7 && boss.bossAct && boss.type==1){
      sprintf(whatWasDone, "���-�� ������ ��� ������ ������. ���������� ���������� �� �������� ���������.\n");
    }
    else{
      // ���� ������� ��� ����������� (� ���� ���� ��� ����)
      if( (onTheWay != WALL) && (ratLife<=0) &&
          (onTheWay != WALL2) && (onTheWay != BOSS1)// &&
      //    (onTheWay != BOSS2)
      ){
        
        // ����
        p_x += up_[0];
        p_y += up_[1];
        
        int sCol = secretColHere(p_x, p_y);
        if( sCol!= -1 ){
          sprintf(whatWasDone, "%s ����� ���� �����-�� �����: %s", whatWasDone, secretCols[sCol].pos);
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

// ������� ������� ������
// x � y - �����, ������ ������� ����� ����������� ������� (������������� ����������)
void genAllEnv(int x, int y){
  // ����������� � �������� (���������) ����������
  int mx = AbsToRelX(x);
  int my = AbsToRelY(y);
  int relx = 0;
  int rely = 0;
  
  relx = mx + up_[0];    rely = my + up_[1];
  genNewRandomObj(relx, rely); // ������
    
  relx = mx + left_[0];    rely = my + left_[1];
  genNewRandomObj(relx, rely); // ����� ���

  relx = mx + right_[0];    rely = my + right_[1];
  genNewRandomObj(relx, rely); // ������ ���

  relx = mx + down_[0];    rely = my + down_[1];
  genNewRandomObj(relx, rely); // �����
}

// ������������� ����� ������ �� ��������� �����������
void genNewRandomObj(int relx, int rely){
  // ����������� �������� ��������
  if( rand()%GENCHANCE == 1 ){
    int newObj = rand()%7+2;
    if( map[ relx ][ rely ] == 0 ){
      map[ relx ][ rely ] = newObj; // ������
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

// ������� ������ � ��������� ������� � �� ��������� �����������
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

// ������ ��� ������� ��������: �����, �������, �������, ������������. ����������� ����� ������� ����.
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

// ���� �������� ��������� ��������� ��������� �������� �� ���������
// max - ���������� ������ �������������
void genCycle(int max, int objNum){
  int genCount = 0; // ���������� ������� �������� �������
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

// ������� �����������, ���� ������� �����
void changeLook(char where){
  // �������� ������
  if(where == 'L'){
    // ���� ������� "�����"
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
  // �������
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

  // �����������
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
      //sprintf(whatWasDone, "�� ���� �����.\n");
      break;
    case 'L':
      up_[0] = -1; up_[1] = 0;
      down_[0] = 1; down_[1] = 0;
      left_[0] = 0; left_[1] = 1;
      right_[0] = 0; right_[1] = -1;
      //sprintf(whatWasDone, "�� ��������� ������.\n");
      break;
    case 'D':
      up_[0] = 0; up_[1] = 1;
      down_[0] = 0; down_[1] = -1;
      left_[0] = 1; left_[1] = 0;
      right_[0] = -1; right_[1] = 0;
      //sprintf(whatWasDone, "�� ������������ �����.\n");
      break;
    case 'R':
      up_[0] = 1; up_[1] = 0;
      down_[0] = -1; down_[1] = 0;
      left_[0] = 0; left_[1] = -1;
      right_[0] = 0; right_[1] = 1;
      //sprintf(whatWasDone, "�� ��������� �������.\n");
      break;
    default:
      break;
  }
}

// ����� ��������� �� ������� ���� � ��������� ��
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

// ��������� ����.
// ���� ����� 2 ������ �������� �����, �� ����� �������� � ���� � �������.
// TODO: ��� �������� 2 ������, ��������������� � ����� ����� ����.
void ratLogic(){
  int i;
  for(i=0; i<ratsCount; i++){
    // ������� ������
    if( p_x == ratsStats[i].x &&
        (p_y - ratsStats[i].y)<=3 &&
        p_y > ratsStats[i].y
        ){
        ratsStats[i].y++;
    }
    
    // ����� ������
    if( p_x == ratsStats[i].x &&
        (ratsStats[i].y - p_y)<=3 &&
        p_y < ratsStats[i].y
        ){
        ratsStats[i].y--;
    }
    
    // ����� ������
    if( p_y == ratsStats[i].y &&
        (p_x - ratsStats[i].x)<=3 &&
        p_x > ratsStats[i].x
        ){
        ratsStats[i].x++;
    }
    
    // ������ ������
    if( p_y == ratsStats[i].y &&
        (ratsStats[i].x - p_x)<=3 &&
        p_x < ratsStats[i].x
        ){
        ratsStats[i].x--;
    }    
  }
}

// ���������� ����� ����.
// ������� �������� ������� �����, � ����� ����� ����.
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

// ����� ���������� ����
void ratsAttack(){
	int attackPower = 0;
	int i;
  int charge = 0;
  
  // ����������� � �������� (���������) ����������
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
		printf( "�� �������� ���� %d �� ����.\n", attackPower );
	}
}

// ��������, ��� �������
int whatsAhead(){
    // ����������� � �������� (���������) ����������
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // �������
//    printf( "����� ���� ");
    return map[ mx+up_[0] ][ my+up_[1] ];
}

// ��� �����
int whatsLeft(){
    // ����������� � �������� (���������) ����������
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // �����
//    printf("����� �� ��� ");
	return map[ mx+left_[0] ][ my+left_[1] ];
}

// ��� ������
int whatsRight(){
    // ����������� � �������� (���������) ����������
    int mx = AbsToRelX(p_x);
    int my = AbsToRelY(p_y);
    // ������
//    printf("������ �� ��� ");
    return map[ mx+right_[0] ][ my+right_[1] ];
}

// ��� ��� ������
int whatsUnderFeet(){
  // ����������� � �������� (���������) ����������
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // ������
  //    printf("��� ������: ");
  return map[ mx ][ my ];
}

int whatsBehind(){
  // ����������� � �������� (���������) ����������
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // ������
  //    printf("����� ��� ");
  return map[ mx+down_[0] ][ my+down_[1] ];
}

// ����� �������� ������� �� ��� ������
void enumerateObjs(int n, int lookx, int looky){
  int ratHere = isRatThere(lookx, looky);
  
  if( ratHere!=-1 ){
    if( ratsStats[ratHere].life <= 0 ){
      printf("������� �����. ");
      if( ratsStats[ratHere].life <= -30 ){
        printf("����� ������ �������� �����");
      }
    }
    else{
      printf("����");
    }        
  }
  else{
  
    switch(n){
      case EMPTINESS: printf( "�������" );
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
      case WALL2: printf("������������� �����");
        break;    
      case DAGGER: printf("%s",obj7);
        break;
      case GOLD: printf("%s",obj8);
        break;
      case BOSS1:
        printf("���� RAT-1000");
        break;
      case BOSS2:
        printf("���� RAT-S");
        break;
      default:
        printf("�����");
        break;
    }
  }
}

// ����� ������� ��� ������
void takeItem(){
  // ����������� � �������� (���������) ����������
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // ����� ��� ��� ������
  switch( map[mx][my] ){
/*    case GROUND:
      sprintf(whatWasDone, "�� ���������� ������ � ����� ����� ����, �� ������ �� ���� ����� �� �����.\n");
      break;*/
    case WALL:
      sprintf(whatWasDone, "��� �� �������, ���� �� �����??\n");
      break;
    case RAT:
      sprintf(whatWasDone, "��� ��� ������� ��������� �� �����?!\n");
      break;
    case MEDKIT:
      sprintf(whatWasDone, "�� ����� �������\n");
      medkits++;
      map[mx][my] = GROUND;
      break;
    case PICK:
      sprintf(whatWasDone, "�� ����� �����\n");
      kirka++;
      map[mx][my] = GROUND;
      break;
    case DAGGER:
      sprintf(whatWasDone, "�� ����� �����������\n");
      weapon++;
      map[mx][my] = GROUND;
      break;
    case GOLD:
      sprintf(whatWasDone, "�� ��������� 10-� ������� ������.\n");
      gold += 10;
      map[mx][my] = GROUND;
      break;
    default:
//      sprintf(whatWasDone, "��� ������ �� ����� :(\n");
      break;
  }
}

// ������� ������ �������
void breakTheWall(){
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);

  if( map[ mx+up_[0]] [my+up_[1] ] == WALL ){
    if(state!=3){
      state = 3;
      obj3_life = 30;
    }

    map[ UX() ][ UY() ] = GROUND;
    sprintf(whatWasDone, "������� ����������� � ����.\n");
    
    int i;
    for(i=0; i<secretCount; i++){
      if( UX() == secretCols[i].x && UY() == secretCols[i].y ){
        sprintf(whatWasDone, "%s����� ���� �����-�� �����:\n%s",whatWasDone, secretCols[i].pos);
      }
    }
  }
  else{
    sprintf(whatWasDone, "����� ����� ������ ��� ��������� ������.\n");
  }
}

// ��������� ������� ��������
void attack(){
  // ����������� � ������������� ����������
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
    sprintf(whatWasDone, "�� ������ ������� ������������� �����.\n");
  }
}

// ����� �� �����
void bossAttacked(){
  if( boss.x == UX() && boss.y == UY() ){
    if( boss.life > 0 ){
      int attackPow = rand()%power_range+power+1;
      boss.life -= attackPow;
      sprintf(whatWasDone, "�� ������� ����� �������, � ������ %d. ����: %d\n", attackPow, boss.life);
      if( boss.life <= 0){
        boss.life = 0;
        boss.bossAct = 0;
        if( boss.type == 1 ){
        /*
          sprintf(whatWasDone, "���� ��������! ���� ������ ��������.\n", attackPow);
          map[ boss.x ][ boss.y ] = EMPTINESS;
          */
          //boss.type = 2;
          
          system("cls");
          
          printf("\n\n\n������ ����������� ����� ���-�� ���������� � ������ �������. �� ����������� � ������. ������ ������� �������� ������ ���, � ��� �� ������. ����� ��������� ������� ����� ����������, ������ ������������� ���� ����� �������� � ������� ���� ����������� ����. �� ����������� ��� ��������� ����. ���� ������������ �����.\n\n[������� ���������� p]");
          
          while( getch()!=112 )
          {}
          
          isExit = 1;
        }
        else{
        // �� �� ����� � ������� ������� ����� :(
        /* 
          sprintf(whatWasDone, "������ ����������� ����� ���-�� ���������� � ������ �������. �� ����������� � ������. ������ ������� �������� ������ ���, � ��� �� ������. ����� ��������� ������� ����� ����������, ������ ������������� ���� ����� �������� � ������� ���� ����������� ����. �� ����������� ��� ��������� ����. ���� ������������ �����.\n", attackPow);
          */
        }
      }
    }
  }
}

// ���������� ����� ����� � ������� ratsStats � ���������� ������������
int isRatThere(int x, int y){
  int i=0;
  while(i<ratsCount){
    if(ratsStats[i].x==x && ratsStats[i].y==y)
      return i;
    i++;
  }
  return -1;
}

// ����� �� �����
void ratAttacked(int newx, int newy){
    // ������� ����� �����
    int ratHere = isRatThere(newx, newy);
    // TODO: ���������� ����������� ����� (��������� �����)
    // ����� ������������
    if(state!=2){
      state = 2;
    }

    if( ratHere >= 0 ){
      // ����� �����
      if( ratsStats[ratHere].life > 0 ){
        int attackPow = rand()%power_range+power+1;
        ratsStats[ratHere].life -= attackPow;
        sprintf(whatWasDone, "�� ������� ����� �������, � ������ %d.\n", attackPow);
        if( ratsStats[ratHere].life <= 0 ){
          ratsStats[ratHere].life = 0;
          //map[ newx ][ newy ] = DEADRAT;
          sprintf(whatWasDone, "����� ���� ���������.\n", attackPow);
        }
        
        // ��������� ���������� ������� ���� � ���������� �����
        int i;
        int liveRats=0;
        for(i=0; i<ratsCount; i++){
          if( ratsStats[i].life > 0 )
            liveRats++;
        }
        
        if( liveRats == 0 ){
          activateBossRoom();
          system("cls");
          printf("\n\n\n��������� ����� ��������������. ��������� ����� �������. ���������, ��� �� ��� ��������...\n\n[������� ���������� p]");
          
          while( getch()!=112 )
          {}

          system("cls");
        }
        
        // TODO: ����� ����������� � decayDeadRats()
      }
      else{
        sprintf(whatWasDone, "����� ��� �� ��������.\n");
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

// ��������� �������
void heal()
{
  if(medkits>0){
    life += 20;
    medkits--;
    sprintf(whatWasDone, "�� ���������� ������ ������ ���. ������ ������������.");
  }
  else{
    sprintf(whatWasDone, "������� ���������:(\n");
  }
}

// �������� ���������
void showInventory(){
  system("cls");
  printf("----------------------------------\n");
  printf("� ��� � ����� ����� �����������\n");
  printf("������� : %d\n", medkits );
  printf("������������ : %d\n", weapon );
  //printf("����� : %d\n", kirka );
  printf("������ : %d\n", gold );
  //printf("� ��� ������� �����.\n");
  printf("----------------------------------\n");
  printf("[������� ���������� p]");
          
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
  printf( "�� - �������������. ������� ���������� ������, � ������������ ��������������, ����� ������ ��� ������. �� ���������� � ������ ����� � ������ ����������. ������������ ����������. ��� ����� ������� ������ � ����� ����� ������. ��� ����� ������ ��������� ��������� �����. ��� ������� ������������� ��� ����, ���� ����������� ������� �� �� �����. ���������� ����� ����������. ��������� ������, ������������ � �������. �������������� ������. ������� � �������� ��������� �� ��������. �����!\n\n");
  printf( "����������:\n");
  printf( "? - ��� �������\t\t\ti - ��������� \n");
  printf( "w - ���� ������\t\t\ta - ������������ ������\n");
  printf( "s - ������������ �����\t\td - ������������ �������\n");
  printf( "e - ����� �������\t\tt - ���� ��������\n");
  printf( "k - ������ ������ ����������\th - ������������ �������\n");
  printf( "q - �����\n" );
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

// ����� ������ ����� (����� ������ � ������, ����� �������� 80��8 ������)
// ����� 39:4
// ������������ ����� ������(��������� ������)
void displayMap(){
  GetConsoleScreenBufferInfo(hConsole, &coninfo);
  coninfo.dwCursorPosition.Y = 6;    // move up one line
  coninfo.dwCursorPosition.X = 0;    // move to the right the length of the word
  SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);
  // �������� ���������� ������ ��������� �����
  int centerX = 39;
  int centerY = 4;
  int mapMaxX = 80;
  int mapMaxY = 8;
  int scrBuf[mapMaxX][mapMaxY];
  // ������ ������� ����������� �� ������ (����� ������� ����)
  int scrSX = centerX - p_x - 1;
  int scrSY = centerY - p_y;
  // ������� ������� ����� (� ������ �������� ����)
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

// ����� ������������ � ���������� ����������� (�� �������������� �� �������)
void displayAbsMap(){
  fillRatsMap();
  // ����������� � �������� (���������) ����������
  int mx = AbsToRelX(p_x);
  int my = AbsToRelY(p_y);
  // ����� ������� ���...
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
          if( (i == 2) && (j == 2) ){// ������ ������
            printf( "@" );
        }
        else{
          if( ratsMap[ mx - 2 + j][ my - 2 + i]!= 0 ){
              // �����
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
        
            // ��������
            switch( map[ mx - 2 + j][ my - 2 + i] ){
              case EMPTINESS: // �������
                printf( " " );
                break;
              case GROUND: // ���
                printf( "_" );
                break;
              case WALL: // �����
                printf( "#" );
                break;
              case WALL2:
                printf( "%%" );
                break;
                
              case RAT: // �����
                break;
              case DEADRAT:
                break;
                
              case BOSS1:
                printf( "B" );
                break;
              case BOSS2:
                printf( "S" );
                break;
              case MEDKIT: // �������
                printf( "+" );
                break;
              case PICK: // �����
                printf( "?" );
                break;
              case DAGGER: // ������
                printf( "!" );
                break;
              case GOLD: // ������
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
// ����� ���� ������ ������
    switch( i ){
      case 0:
        printf( "�������:\t " );
        enumerateObjs( whatsAhead(), UX(),  UY() );
        break;
      case 1:
        printf( "�����:\t " );
        enumerateObjs( whatsLeft(), LX(), LY() );
        break;
      case 2:
        printf( "������:\t " );
        enumerateObjs( whatsRight(), RX(), RY() );
        break;
      case 3: 
        printf("�����:\t %d", life);
        break;
      case 4:
        printf( "��� ������:\t " );
        enumerateObjs( whatsUnderFeet(), CX(), CY_() );
        break;
      default:
        break;
    }

    printf( "\n" );
  }
//  printf("\nDEBUG: ratsCount = %d\n", ratsCount);
//  printf("x: %d  y: %d\n", p_x, p_y);
  printf("\n������� ����� ��������: ");
}

// ��������� ����� 1.
void boss1Action(){
  if( boss.bossAct == 0 ){
    if( p_y == (BRY+8) && (p_x >= (BRX+1) && 
        p_x <= (BRX+3)) &&
        boss.type == 1 
      ){
//      printf("boss1 activated!\n");
      boss.bossAct = 1;
      int i = 0;
      // ��������� ���� � �����������
      for(i=0; i<5; i++){
        map[ BRX+i ][ BRY+9 ] = WALL2;
      }
      
      system("cls");
      
      printf("\n\n\n�� ����� ������������ ����� ��� ����� � � �������� ���������� ������� ������������� �����. ��� �������!\n\n[������� ���������� p]");
          
          while( getch()!=112 )
          {}
      system("cls");
    }
  }
  
  if( boss.bossAct && boss.type == 1 ){
    map[ boss.x ][ boss.y ] = GROUND;
    // ���� � ������ ��� ���������
    if( boss.actCounter >= 3 ){
      // �����-����� ("�����")
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
          // ���� �� ���� ����� �����, �� ������ ��������
          // � �����.
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
        // �����
        life -= rand()%boss.power_range+boss.power+1;
      }
      else{
        // ���� � ������� ������
        boss.x += p_x - boss.x;
      }
    }
    
    map[ boss.x ][ boss.y ] = BOSS1;
    boss.actCounter++;
  }
}

// ��������� ����� 2. ����� ������, ������� � ��������� �������� ����� ������.
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

// �������� ������ ���� �� ���� �����.
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

// ����� �� �������� ������
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

// ������� �� ����� ����� � ������. ����������� ������ ���� ���,
// � ������ ����.
// ���� ����� ����, �� �� ����� ��� ������ �����, �������������.
// ���� ����� �����, �� ����������� ���� � ���� �����.
void createBossRoom(){
  int i=0;
  int j=0;
  
  for(i=0; i<5; i++){
    for(j=0; j<10; j++){
      map[ BRX+i ][ BRY+j ] = WALL2;
    }
  }
}

// ��������� ������� � ������, ������ ����� � ���
void activateBossRoom(){
  // �����
  map[ BRX+2 ][ BRY+1 ] = GROUND;
  map[ BRX+1 ][ BRY+2 ] = GROUND;
  map[ BRX+2 ][ BRY+2 ] = GROUND;
  map[ BRX+2 ][ BRY+3 ] = GROUND;
  map[ BRX+3 ][ BRY+2 ] = GROUND;
  
  // �������
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

// ��������� ������� �����
void activateBoss2Room(){
  // ������� �����
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

// ������� ��������� ����-���������, ������ �� ������� � ������ �������, ����� ������� ������ ������ (�� ��������� �����). ���������� ������ ���� � ���������� ������ �������� � ���������� �������� (�� ����� �������)
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

// ��������� �������, ����� ����� �������� �� �����
void triggerSecretCell(){
  if( cellsEnabled != secretCount ){
    int i;
    for( i=0; i<secretCount; i++ ){
      if( secretCells[i].x == p_x &&
          secretCells[i].y == p_y ){

          sprintf(whatWasDone, "����� ��� ������ ������� ���������� �� ���������.\n");
        
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
      printf( "\n\n\n����� ���� ����� ���� ������� � ��������� ��������������. ��� ���������� �����! ��� �� �������� � ������ � �������.\n\n[������� ���������� p]");
          
          while( getch()!=112 )
          {}
      power = 50;
      power_range = 30;
      system("cls");
    }
  }
}

// ���������� ������� �� x
int UX(){
  return AbsToRelX(p_x)+up_[0];
}

// ���������� ������� �� y
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

// ��� ������
int CX(){
  return AbsToRelX(p_x);
}

int CY_(){
  return AbsToRelY(p_y);
}

// �������� ������ ������ ����� ��� GROUND
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