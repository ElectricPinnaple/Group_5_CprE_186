#include <stdio.h>
#include <time.h>
#include <string.h>

typedef enum Cubelet Cubelet;
enum Cubelet {  WRB, WRG, WOG, WOB, WR, WO, WB, WG, //TODO does this compile to like - enum enum Cubelet { ... ???
				YRB, YRG, YOG, YOB, YR, YO, YB, YG,
				 RB,  RG,  OG,  OB,
				 W, Y, O, R, G, B, N }; //RED X ORANGE --- WHITE X YELLOW --- BLUE X GREEN

char solution[350] = {0};
int solutionLen = 0;

Cubelet cP[3][3][3] = { //Positions of the Cubes
			{ { WOB,  WB, WRB }, //BACK
			  {  OB,   B,  RB },
			  { YOB,  YB, YRB }
			 }, {
			  {  WO,   W,  WR }, //MIDDLE
			  {   O,   N,   R },
			  {  YO,   Y,  YR }
			 }, {
			  { WOG,  WG, WRG }, //FRONT
			  {  OG,   G,  RG },
			  { YOG,  YG, YRG }
			} };
Cubelet cF[12][9] = { //Values of each of the cube faces
			N, N, N, W, W, W, N, N, N,
			N, N, N, W, W, W, N, N, N,
			N, N, N, W, W, W, N, N, N,
			O, O, O, G, G, G, R, R, R,
			O, O, O, G, G, G, R, R, R,
			O, O, O, G, G, G, R, R, R,
			N, N, N, Y, Y, Y, N, N, N,
			N, N, N, Y, Y, Y, N, N, N,
			N, N, N, Y, Y, Y, N, N, N,
			N, N, N, B, B, B, N, N, N,
			N, N, N, B, B, B, N, N, N,
			N, N, N, B, B, B, N, N, N	};


int topBottomEdgeCheck(int i, int j, int k);

void whiteCross();
void findCube(Cubelet c, int* i, int* j, int* k);
void moveEdgeToBottom(int i, int j, int k);
void moveEdgeToColumn(Cubelet column, int i, int j, int k);
void putEdgeInPosition(int i, int j, int k);
int whiteCrossOrientationCheck(int i, int j, int k);

void printCube(int which);
void fixInverse();

void front();
void back();
void down();
void up();
void right();
void left();
void frontInverse();
void backInverse();
void downInverse();
void upInverse();
void rightInverse();
void leftInverse();

int main(int argc, char* argv[]) {



	//Do Stuff
	if (1==2) { //Make A Cool Pattern
		right();
		leftInverse();
		front();
		backInverse();
		up();
		downInverse();
		right();
		leftInverse();
		printCube(1);
	}
	if (1==1) {
		int i;
		srand(time(NULL));
		for (i=0;i<20;i++) {
		//sleep(1);
		int r = rand() % 12;
 		switch (r) {
			case  0:
				right();
				break;
			case  1:
				left();
				break;
			case  2:
				up();
				break;
			case  3:
				down();
				break;
			case  4:
				front();
				break;
			case  5:
				back();
				break;
			case  6:
				rightInverse();
				break;
			case  7:
				leftInverse();
				break;
			case  8:
				upInverse();
				break;
			case  9:
				downInverse();
				break;
			case 10:
				frontInverse();
				break;
			case 11:
				backInverse();
				break;
		}
		} //Randomize Cube State
		strcat(solution, " | ");
		solutionLen+=3;
	}

	if (1==1) {
		printCube(1);
		whiteCross();
		printCube(1);
		fixInverse();
		printf("%s\n", solution);
	}
	return 0;
}
void fixInverse() { //TODO: Deal with four of something in a row & inverse next to regular move
	int i;
	int cnt = 0;
	Cubelet lastC = N;
	for(i=0; i<solutionLen-2; i++) {
		if (solution[i] == solution[i+1] && solution[i] == solution[i+2]) {
			solution[i+1] = 'i';
			int j;
			for (j=i+2; j<solutionLen; j++) {
				solution[j] = solution[j+1];
			}
			solutionLen--;
		}
	}
}
void whiteCross() {
	int i, j, k;
	//WR
	findCube(WR, &i, &j, &k); //TODO: move the findCube into moveEdgeToBottom?
	moveEdgeToBottom(i, j, k);
	findCube(WR, &i, &j, &k); //TODO: this in inneficent
	moveEdgeToColumn(R, i, j, k);
	findCube(WR, &i, &j, &k);
	putEdgeInPosition(i, j, k);
	//WB
	findCube(WB, &i, &j, &k);
	moveEdgeToBottom(i, j, k);
	findCube(WB, &i, &j, &k); //TODO: this in inneficent
	moveEdgeToColumn(B, i, j, k);
	findCube(WB, &i, &j, &k);
	putEdgeInPosition(i, j, k);
	//WO
	findCube(WO, &i, &j, &k);
	moveEdgeToBottom(i, j, k);
	findCube(WO, &i, &j, &k); //TODO: this in inneficent
	moveEdgeToColumn(O, i, j, k);
	findCube(WO, &i, &j, &k);
	putEdgeInPosition(i, j, k);
	//WG
	findCube(WG, &i, &j, &k);
	moveEdgeToBottom(i, j, k);
	findCube(WG, &i, &j, &k); //TODO: this in inneficent
	moveEdgeToColumn(G, i, j, k);
	findCube(WG, &i, &j, &k);
	putEdgeInPosition(i, j, k);
}

int topBottomEdgeCheck(int i, int j, int k) { //Checks the Orientation of Edges On The Top or Bottom Layer : TODO: this can be used for last 2 layers and maybe f2l
	if (j == 0) {
		if (i == 0) return cF[0][4] == cF[1][4];
		else if (i == 2) return cF[1][4] == cF[2][4];
		else {
			if (k == 0) return cF[1][3] == cF[1][4];
			else return cF[1][4] == cF[1][5];
		}
	}
	else if (j == 2) {
		if (i == 0) return cF[7][4] == cF[8][4];
		else if (i == 2) return cF[6][4] == cF[7][4];
		else {
			if (k == 0) return cF[7][3] == cF[7][4];
			else return cF[7][4] == cF[7][5];
		}
	}
	else {
		return -1;
		//printf("topBottomEdgeCheck had invalid location to check\n"); //TODO
	}
}
void putEdgeInPosition(int i, int j, int k) { //TODO: make it so that it is not just a bandaid solution of the last edge.. also why does only that one misbehave?
	if (whiteCrossOrientationCheck(i, j, k)) {
		if (i == 0) { back(); back(); }
		else if (i == 2) { front(); front(); }
		else {
			if (k == 0) { left(); left(); }
			else { right(); right(); }
		}
	}
	else {
		if (i == 0) { down(); left(); backInverse(); leftInverse(); }
		else if (i == 2) { down(); right(); frontInverse(); rightInverse(); }
		else {
			if (k == 0) { down(); front(); leftInverse(); frontInverse(); }
			else { down(); back(); rightInverse(); backInverse(); }
		}
	}
}
int whiteCrossOrientationCheck(int i, int j, int k) {
	if (j != 2) {
		return -1;
	}

	if (i == 0) return cF[9][4] == cF[10][4];
	else if (i == 2) return cF[4][4] == cF[5][4];
	else {
		if (k == 0) return cF[4][1] == cF[5][1];
		else return cF[4][7] == cF[5][7];
	}
}
void moveEdgeToColumn(Cubelet column, int i, int j, int k) {
	Cubelet search = cP[i][j][k];
	while (column != cP[i][j-1][k]) {
		down();
		findCube(search, &i, &j, &k);
	}
}
void moveEdgeToBottom(int i, int j, int k) {
	//int count = 0; //TODO
	if (i == 0) {
		if (j == 0) {
			back();
			back();
		}
		else if (j == 1) {
			if (k == 0) { //TODO: what about the order of the solution makes it so that only the last one (back/blue) is removed from its place?
				back();
				down();
				backInverse();
			}
			else {
				backInverse();
				down();
				back();
			}
		}
	}
	else if (i == 1) {
		if (j == 0) {
			if (k == 0) {
				left();
				left();
			}
			else {
				right();
				right();
			}
		}
	}
	else {
		if (j == 0) {
			front();
			front();
		}
		else if(j == 1) {
			if (k == 0) {
				frontInverse();
			}
			else {
				front();
			}
		}
	}
}
void findCube(Cubelet search, int* i, int* j, int* k) {
	for(*i=0; *i<3; (*i)++) {
		for(*j=0; *j<3; (*j)++) {
			for(*k=0; *k<3; (*k)++) {
				if(cP[*i][*j][*k] == search) return;
			}
		}
	}
}

// which = 0 for position, 1 for faces, 2 for both
void printCube(int which) { //TODO print positions also
	char* enumStrings[27] = {
		"WRB", "WRG", "WOG", "WOB", "WR", "WO", "WB", "WG",
		"YRB", "YRG", "YOG", "YOB", "YR", "YO", "YB", "WG",
		"RB", "RG", "OG", "OB", "W", "Y", "O", "R", "G", "B", " " };

	printf("\n        ---------\n"); //Print Faces
	int i, j, k;
	for (i=0; i<12; i++) {
		if (i>2 && i<6) printf("| ");
		else printf("  ");
		for (j=0; j<9; j++) {
			if (j==3 || j==6) printf("| ");
			printf("%s ", enumStrings[ cF[i][j] ]);
		}
		if (i>2 && i<6) printf("|");
		if (i>1 && i<6) printf("\n-------------------------\n");
		else printf("\n        ---------\n");
	}
	printf("\n");

	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			for (k=0; k<3; k++) {
				printf("%3s ", enumStrings[ cP[i][j][k] ]);
			}
			printf("\n");
		}
		printf("\n");
	}
	//print positions
}

void front() {
	strcat(solution, "F");
	solutionLen++;
	
	Cubelet tmp;
	tmp = cP[2][0][0]; cP[2][0][0] = cP[2][2][0]; cP[2][2][0] = cP[2][2][2]; cP[2][2][2] = cP[2][0][2]; cP[2][0][2] = tmp; //Front Position Corner Moves
	tmp = cP[2][0][1]; cP[2][0][1] = cP[2][1][0]; cP[2][1][0] = cP[2][2][1]; cP[2][2][1] = cP[2][1][2]; cP[2][1][2] = tmp; //Front Position Edge Moves
	tmp = cF[3][3]; cF[3][3] = cF[5][3]; cF[5][3] = cF[5][5]; cF[5][5] = cF[3][5]; cF[3][5] = tmp; //Front Face Corners
	tmp = cF[4][3]; cF[4][3] = cF[5][4]; cF[5][4] = cF[4][5]; cF[4][5] = cF[3][4]; cF[3][4] = tmp; //Front Face Edges
	tmp = cF[2][3]; cF[2][3] = cF[5][2]; cF[5][2] = cF[6][5]; cF[6][5] = cF[3][6]; cF[3][6] = tmp; //Front Edge Corners
	tmp = cF[2][4]; cF[2][4] = cF[4][2]; cF[4][2] = cF[6][4]; cF[6][4] = cF[4][6]; cF[4][6] = tmp; //Front Edge Edges
	tmp = cF[2][5]; cF[2][5] = cF[3][2]; cF[3][2] = cF[6][3]; cF[6][3] = cF[5][6]; cF[5][6] = tmp; //Front Edge Corners Part 2 TODO 
}
void back() {
	strcat(solution, "B");
	solutionLen++;

	Cubelet tmp;
	tmp = cP[0][0][0]; cP[0][0][0] = cP[0][0][2]; cP[0][0][2] = cP[0][2][2]; cP[0][2][2] = cP[0][2][0]; cP[0][2][0] = tmp; //Back Position Corner Moves
	tmp = cP[0][0][1]; cP[0][0][1] = cP[0][1][2]; cP[0][1][2] = cP[0][2][1]; cP[0][2][1] = cP[0][1][0]; cP[0][1][0] = tmp; //Back Position Edge Moves
	tmp = cF[9][3]; cF[9][3] = cF[11][3]; cF[11][3] = cF[11][5]; cF[11][5] = cF[9][5]; cF[9][5] = tmp; //Back Face Corners
	tmp = cF[9][4]; cF[9][4] = cF[10][3]; cF[10][3] = cF[11][4]; cF[11][4] = cF[10][5]; cF[10][5] = tmp; //Back Face Edges
	tmp = cF[8][5]; cF[8][5] = cF[5][0]; cF[5][0] = cF[0][3]; cF[0][3] = cF[3][8]; cF[3][8] = tmp; //Back Edge Corners
	tmp = cF[8][4]; cF[8][4] = cF[4][0]; cF[4][0] = cF[0][4]; cF[0][4] = cF[4][8]; cF[4][8] = tmp; //Back Edge Edges
	tmp = cF[8][3]; cF[8][3] = cF[3][0]; cF[3][0] = cF[0][5]; cF[0][5] = cF[5][8]; cF[5][8] = tmp; //Back Edge Corners Part 2 TODO
}
void down() {
	strcat(solution, "D");
	solutionLen++;
	
	Cubelet tmp;
	tmp = cP[0][2][0]; cP[0][2][0] = cP[0][2][2]; cP[0][2][2] = cP[2][2][2]; cP[2][2][2] = cP[2][2][0]; cP[2][2][0] = tmp; //Down Position Corner Moves
	tmp = cP[0][2][1]; cP[0][2][1] = cP[1][2][2]; cP[1][2][2] = cP[2][2][1]; cP[2][2][1] = cP[1][2][0]; cP[1][2][0] = tmp; //Down Positioin Edge Moves
	tmp = cF[6][3]; cF[6][3] = cF[8][3]; cF[8][3] = cF[8][5]; cF[8][5] = cF[6][5]; cF[6][5] = tmp; //Down Face Corners
	tmp = cF[6][4]; cF[6][4] = cF[7][3]; cF[7][3] = cF[8][4]; cF[8][4] = cF[7][5]; cF[7][5] = tmp; //Down Face Edges
	tmp = cF[5][8]; cF[5][8] = cF[5][5]; cF[5][5] = cF[5][2]; cF[5][2] = cF[9][3]; cF[9][3] = tmp; //Down Edge Corners
	tmp = cF[5][7]; cF[5][7] = cF[5][4]; cF[5][4] = cF[5][1]; cF[5][1] = cF[9][4]; cF[9][4] = tmp; //Down Edge Edges
	tmp = cF[5][6]; cF[5][6] = cF[5][3]; cF[5][3] = cF[5][0]; cF[5][0] = cF[9][5]; cF[9][5] = tmp; //Down Edge Corners Part 2 TODO
}
void up() {
	strcat(solution, "U");
	solutionLen++;
	
	Cubelet tmp;
	tmp = cP[0][0][0]; cP[0][0][0] = cP[2][0][0]; cP[2][0][0] = cP[2][0][2]; cP[2][0][2] = cP[0][0][2]; cP[0][0][2] = tmp; //Up Position Corner Moves
	tmp = cP[0][0][1]; cP[0][0][1] = cP[1][0][0]; cP[1][0][0] = cP[2][0][1]; cP[2][0][1] = cP[1][0][2]; cP[1][0][2] = tmp; //Up Position Edge Moves
	tmp = cF[0][3]; cF[0][3] = cF[2][3]; cF[2][3] = cF[2][5]; cF[2][5] = cF[0][5];  cF[0][5] = tmp; //Up Face Corners
	tmp = cF[0][4]; cF[0][4] = cF[1][3]; cF[1][3] = cF[2][4]; cF[2][4] = cF[1][5];  cF[1][5] = tmp; //Up Face Edges
	tmp = cF[3][0]; cF[3][0] = cF[3][3]; cF[3][3] = cF[3][6]; cF[3][6] = cF[11][5]; cF[11][5] = tmp; //Up Edge Corners
	tmp = cF[3][1]; cF[3][1] = cF[3][4]; cF[3][4] = cF[3][7]; cF[3][7] = cF[11][4]; cF[11][4] = tmp; //Up Edge Edges
	tmp = cF[3][2]; cF[3][2] = cF[3][5]; cF[3][5] = cF[3][8]; cF[3][8] = cF[11][3]; cF[11][3] = tmp; //Up Edge Corners Part 2 TODO: Name this better (and the simmilar ones also)
}
void right() {
	strcat(solution, "R");
	solutionLen++;
	
	Cubelet tmp;
	tmp = cP[0][0][2]; cP[0][0][2] = cP[2][0][2]; cP[2][0][2] = cP[2][2][2]; cP[2][2][2] = cP[0][2][2]; cP[0][2][2] = tmp; //Right Position Corner Moves
	tmp = cP[0][1][2]; cP[0][1][2] = cP[1][0][2]; cP[1][0][2] = cP[2][1][2]; cP[2][1][2] = cP[1][2][2]; cP[1][2][2] = tmp;//Right Position Edge Moves
	tmp = cF[3][6]; cF[3][6] = cF[5][6]; cF[5][6] = cF[5][8]; cF[5][8] = cF[3][8];  cF[3][8] = tmp; //Right Face Corners
	tmp = cF[4][6]; cF[4][6] = cF[5][7]; cF[5][7] = cF[4][8]; cF[4][8] = cF[3][7];  cF[3][7] = tmp; //Right Face Edges
	tmp = cF[0][5]; cF[0][5] = cF[3][5]; cF[3][5] = cF[6][5]; cF[6][5] = cF[9][5];  cF[9][5] = tmp; //Right Edge Corners
	tmp = cF[1][5]; cF[1][5] = cF[4][5]; cF[4][5] = cF[7][5]; cF[7][5] = cF[10][5]; cF[10][5] = tmp; //Right Edge Edges
	tmp = cF[2][5]; cF[2][5] = cF[5][5]; cF[5][5] = cF[8][5]; cF[8][5] = cF[11][5]; cF[11][5] = tmp; //Right Edge Corners Part 2 TODO
}
void left() {
	strcat(solution, "L");
	solutionLen++;
	
	Cubelet tmp;
	tmp = cP[0][0][0]; cP[0][0][0] = cP[0][2][0]; cP[0][2][0] = cP[2][2][0]; cP[2][2][0] = cP[2][0][0]; cP[2][0][0] = tmp; //Left Position Moves
	tmp = cP[0][1][0]; cP[0][1][0] = cP[1][2][0]; cP[1][2][0] = cP[2][1][0]; cP[2][1][0] = cP[1][0][0]; cP[1][0][0] = tmp;//Left Position Edge Moves
	tmp = cF[3][2];  cF[3][2] =  cF[3][0]; cF[3][0] = cF[5][0]; cF[5][0] = cF[5][2];  cF[5][2] = tmp; //Left Face Corners
	tmp = cF[3][1];  cF[3][1] =  cF[4][0]; cF[4][0] = cF[5][1]; cF[5][1] = cF[4][2];  cF[4][2] = tmp; //Left Face Edges
	tmp = cF[9][3];  cF[9][3] =  cF[6][3]; cF[6][3] = cF[3][3]; cF[3][3] = cF[0][3];  cF[0][3] = tmp; //Left Edge Corners
	tmp = cF[10][3]; cF[10][3] = cF[7][3]; cF[7][3] = cF[4][3]; cF[4][3] = cF[1][3];  cF[1][3] = tmp; //Left Edge Edges
	tmp = cF[11][3]; cF[11][3] = cF[8][3]; cF[8][3] = cF[5][3]; cF[5][3] = cF[2][3];  cF[2][3] = tmp; //Left Edge Corners Part 2 TODO
}
void frontInverse() {
	//strcat(solution, "iF ");
	//solutionLen+=3;
	
	front(); front(); front();
}
void backInverse() {
	//strcat(solution, "iB ");
	//solutionLen+=3;
	
	back(); back(); back();
}
void downInverse() {
	//strcat(solution, "iD ");
	//solutionLen+=3;
	
	down(); down(); down(); 
}
void upInverse() {
	//strcat(solution, "iU ");
	//solutionLen+=3;
	
	up(); up(); up();	
}
void rightInverse() {
	//strcat(solution, "iR ");
	//solutionLen+=3;
	
	right(); right(); right();
}
void leftInverse() {
	//strcat(solution, "iL ");
	//solutionLen+=3;
	
	left(); left(); left();
}

/*
To hold the state of the cube there needs to be a record of the position of every piece, and its orientation.

The coord system can be implemented with a simple 3x3x3 array such as

------- ------- ------- 
| | | | | | | | | | | | 
------- ------- -------
| | | | | | | | | | | | 
------- ------- ------- 
| | | | | | | | | | | | 
------- ------- ------- 

	in which the locations each refer to a location on the cube, with the center location (1,1,1) obviously not representing a real location
	on the cube.

The orientation of the pieces is handled seperetly...

     0 1 2   3 4 5   6 7 8
            ------- 
0           | | | | 
            ------- 
1           | | | | 
            ------- 
2           | | | | 
            ------- 
    ------- ------- ------- 
3   | | | | | | | | | | | | 
    ------- ------- ------- 
4   | | | | | | | | | | | | 
    ------- ------- ------- 
5   | | | | | | | | | | | | 
    ------- ------- ------- 
            ------- 
6           | | | | 
            ------- 
7           | | | | 
            ------- 
8           | | | | 
            ------- 
            ------- 
9           | | | | 
            ------- 
10          | | | | 
            ------- 
11          | | | | 
            ------- 

Default/Solved Positions of the cube
	Cubelet cubePosition[3][3][3] = { 
									{ { WOB,  WB, WRB }, //BACK
									  {  OB,   B,  RB },
									  { YOB,  YB, YRB }
									 }, {
									  {  WO,   W,  WR }, //MIDDLE
									  {   O,   N,   R },
									  {  YO,   Y,  YR }
									 }, {
									  { WOG,  WG, WRG }, //FRONT
									  {  OG,   G,  RG },
									  { YOG,  YG, YRG }
									} };
	Cubelet cubeFaces[12][9] = {	N, N, N, W, W, W, N, N, N,
									N, N, N, W, W, W, N, N, N,
									N, N, N, W, W, W, N, N, N,
									O, O, O, G, G, G, R, R, R,
									O, O, O, G, G, G, R, R, R,
									O, O, O, G, G, G, R, R, R,
									N, N, N, Y, Y, Y, N, N, N,
									N, N, N, Y, Y, Y, N, N, N,
									N, N, N, Y, Y, Y, N, N, N,
									N, N, N, B, B, B, N, N, N,
									N, N, N, B, B, B, N, N, N,
									N, N, N, B, B, B, N, N, N	};
/*
				{ WOB,  WB, WRB } //BACK
				{  OB,   B,  RB }
				{ YOB,  YB, YRB }

				{  WO,   W,  WR } //MIDDLE
				{   O,   N,   R }
				{  YO,   Y,  YR }
  
				{ WOG,  WG, WRG } //FRONT
				{  OG,   G,  RG }
				{ YOG,  YG, YRG }
*/



