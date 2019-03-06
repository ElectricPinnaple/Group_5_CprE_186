#include <stdio.h>

#define CUBE cP, cF
typedef enum Cubelet Cubelet;

enum Cubelet {  WRB, WRG, WOG, WOB, WR, WO, WB, WG,
				YRB, YRG, YOG, YOB, YR, YO, YB, YG,
				 RB,  RG,  OG,  OB,
				 W, Y, O, R, G, B, N }; //RED X ORANGE --- WHITE X YELLOW --- BLUE X GREEN
//enum Facelet { W, O, R, B, G, Y };

void printCube(Cubelet p[3][3][3], Cubelet f[12][9], int which);
void front(Cubelet p[3][3][3], Cubelet f[12][9]);
void back(Cubelet p[3][3][3], Cubelet f[12][9]);
void down(Cubelet p[3][3][3], Cubelet f[12][9]);
void up(Cubelet p[3][3][3], Cubelet f[12][9]);
void right(Cubelet p[3][3][3], Cubelet f[12][9]);
void left(Cubelet p[3][3][3], Cubelet f[12][9]);
void frontInverse(Cubelet p[3][3][3], Cubelet f[12][9]);
void backInverse(Cubelet p[3][3][3], Cubelet f[12][9]);
void downInverse(Cubelet p[3][3][3], Cubelet f[12][9]);
void upInverse(Cubelet p[3][3][3], Cubelet f[12][9]);
void rightInverse(Cubelet p[3][3][3], Cubelet f[12][9]);
void leftInverse(Cubelet p[3][3][3], Cubelet f[12][9]);

int main(int argc, char* argv[]) {
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


	//Do Stuff
	if (false) { //Make A Cool Pattern
		right(CUBE);
		leftInverse(CUBE);
		front(CUBE);
		backInverse(CUBE);
		up(CUBE);
		downInverse(CUBE);
		right(CUBE);
		leftInverse(CUBE);
		printCube(CUBE, 1);
	}

	return 0;
}

// which = 0 for position, 1 for faces, 2 for both
void printCube(Cubelet p[3][3][3], Cubelet f[12][9], int which) { //TODO print positions also
	char* enumStrings[27] = {
		"WRB", "WRG", "WOG", "WOB", "WR", "WO", "WB", "WG",
		"YRB", "YRG", "YOG", "YOB", "YR", "YO", "YB", "WG",
		"RB", "RG", "OG", "OB", "W", "Y", "O", "R", "G", "B", " " };

	printf("\n        ---------\n"); //Print Faces
	int i, j;
	for (i=0; i<12; i++) {
		if (i>2 && i<6) printf("| ");
		else printf("  ");
		for (j=0; j<9; j++) {
			if (j==3 || j==6) printf("| ");
			printf("%s ", enumStrings[ f[i][j] ]);
		}
		if (i>2 && i<6) printf("|");
		if (i>1 && i<6) printf("\n-------------------------\n");
		else printf("\n        ---------\n");
	}
	printf("\n");

	//print positions
}

void front(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[2][0][0]; p[2][0][0] = p[2][2][0]; p[2][2][0] = p[2][2][2]; p[2][2][2] = p[2][0][2]; p[2][0][2] = tmp; //Front Position Corner Moves
	tmp = p[2][0][1]; p[2][0][1] = p[2][1][0]; p[2][1][0] = p[2][2][1]; p[2][2][1] = p[2][1][2]; p[2][1][2] = tmp; //Front Position Edge Moves
	tmp = f[3][3]; f[3][3] = f[5][3]; f[5][3] = f[5][5]; f[5][5] = f[3][5]; f[3][5] = tmp; //Front Face Corners
	tmp = f[4][3]; f[4][3] = f[5][4]; f[5][4] = f[4][5]; f[4][5] = f[3][4]; f[3][4] = tmp; //Front Face Edges
	tmp = f[2][3]; f[2][3] = f[5][2]; f[5][2] = f[6][5]; f[6][5] = f[3][6]; f[3][6] = tmp; //Front Edge Corners
	tmp = f[2][4]; f[2][4] = f[4][2]; f[4][2] = f[6][4]; f[6][4] = f[4][6]; f[4][6] = tmp; //Front Edge Edges
	tmp = f[2][5]; f[2][5] = f[3][2]; f[3][2] = f[6][3]; f[6][3] = f[5][6]; f[5][6] = tmp; //Front Edge Corners Part 2 TODO 
}
void back(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[0][0][0]; p[0][0][0] = p[0][0][2]; p[0][0][2] = p[0][2][2]; p[0][2][2] = p[0][2][0]; p[0][2][0] = tmp; //Back Position Corner Moves
	tmp = p[0][0][1]; p[0][0][1] = p[0][1][2]; p[0][1][2] = p[0][2][1]; p[0][2][1] = p[0][1][0]; p[0][1][0] = tmp; //Back Position Edge Moves
	tmp = f[9][3]; f[9][3] = f[11][3]; f[11][3] = f[11][5]; f[11][5] = f[9][5]; f[9][5] = tmp; //Back Face Corners
	tmp = f[9][4]; f[9][4] = f[10][3]; f[10][3] = f[11][4]; f[11][4] = f[10][5]; f[10][5] = tmp; //Back Face Edges
	tmp = f[8][5]; f[8][5] = f[5][0]; f[5][0] = f[0][3]; f[0][3] = f[3][8]; f[3][8] = tmp; //Back Edge Corners
	tmp = f[8][4]; f[8][4] = f[4][0]; f[4][0] = f[0][4]; f[0][4] = f[4][8]; f[4][8] = tmp; //Back Edge Edges
	tmp = f[8][3]; f[8][3] = f[3][0]; f[3][0] = f[0][5]; f[0][5] = f[5][8]; f[5][8] = tmp; //Back Edge Corners Part 2 TODO
}
void down(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[0][2][0]; p[0][2][0] = p[0][2][2]; p[0][2][2] = p[2][2][2]; p[2][2][2] = p[2][2][0]; p[2][2][0] = tmp; //Down Position Corner Moves
	tmp = p[0][2][1]; p[0][2][1] = p[1][2][2]; p[1][2][2] = p[2][2][1]; p[2][2][1] = p[1][2][0]; p[1][2][0] = tmp; //Down Positioin Edge Moves
	tmp = f[6][3]; f[6][3] = f[8][3]; f[8][3] = f[8][5]; f[8][5] = f[6][5]; f[6][5] = tmp; //Down Face Corners
	tmp = f[6][4]; f[6][4] = f[7][3]; f[7][3] = f[8][4]; f[8][4] = f[7][5]; f[7][5] = tmp; //Down Face Edges
	tmp = f[5][8]; f[5][8] = f[5][5]; f[5][5] = f[5][2]; f[5][2] = f[9][3]; f[9][3] = tmp; //Down Edge Corners
	tmp = f[5][7]; f[5][7] = f[5][4]; f[5][4] = f[5][1]; f[5][1] = f[9][4]; f[9][4] = tmp; //Down Edge Edges
	tmp = f[5][6]; f[5][6] = f[5][3]; f[5][3] = f[5][0]; f[5][0] = f[9][5]; f[9][5] = tmp; //Down Edge Corners Part 2 TODO
}
void up(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[0][0][0]; p[0][0][0] = p[2][0][0]; p[2][0][0] = p[2][0][2]; p[2][0][2] = p[0][0][2]; p[0][0][2] = tmp; //Up Position Corner Moves
	//Up Position Edge Moves
	tmp = f[0][3]; f[0][3] = f[2][3]; f[2][3] = f[2][5]; f[2][5] = f[0][5];  f[0][5] = tmp; //Up Face Corners
	tmp = f[0][4]; f[0][4] = f[1][3]; f[1][3] = f[2][4]; f[2][4] = f[1][5];  f[1][5] = tmp; //Up Face Edges
	tmp = f[3][0]; f[3][0] = f[3][3]; f[3][3] = f[3][6]; f[3][6] = f[11][5]; f[11][5] = tmp; //Up Edge Corners
	tmp = f[3][1]; f[3][1] = f[3][4]; f[3][4] = f[3][7]; f[3][7] = f[11][4]; f[11][4] = tmp; //Up Edge Edges
	tmp = f[3][2]; f[3][2] = f[3][5]; f[3][5] = f[3][8]; f[3][8] = f[11][3]; f[11][3] = tmp; //Up Edge Corners Part 2 TODO: Name this better (and the simmilar ones also)
}
void right(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[0][0][2]; p[0][0][2] = p[2][0][2]; p[2][0][2] = p[2][2][2]; p[2][2][2] = p[0][2][2]; p[0][2][2] = tmp; //Right Position Corner Moves
	//Right Position Edge Moves
	tmp = f[3][6]; f[3][6] = f[5][6]; f[5][6] = f[5][8]; f[5][8] = f[3][8];  f[3][8] = tmp; //Right Face Corners
	tmp = f[4][6]; f[4][6] = f[5][7]; f[5][7] = f[4][8]; f[4][8] = f[3][7];  f[3][7] = tmp; //Right Face Edges
	tmp = f[0][5]; f[0][5] = f[3][5]; f[3][5] = f[6][5]; f[6][5] = f[9][5];  f[9][5] = tmp; //Right Edge Corners
	tmp = f[1][5]; f[1][5] = f[4][5]; f[4][5] = f[7][5]; f[7][5] = f[10][5]; f[10][5] = tmp; //Right Edge Edges
	tmp = f[2][5]; f[2][5] = f[5][5]; f[5][5] = f[8][5]; f[8][5] = f[11][5]; f[11][5] = tmp; //Right Edge Corners Part 2 TODO
}
void left(Cubelet p[3][3][3], Cubelet f[12][9]) {
	Cubelet tmp;
	tmp = p[0][0][0]; p[0][0][0] = p[0][2][0]; p[0][2][0] = p[2][2][0]; p[2][2][0] = p[2][0][0]; p[2][0][0] = tmp; //Left Position Moves
	//Left Position Edge Moves
	tmp = f[3][2];  f[3][2] =  f[3][0]; f[3][0] = f[5][0]; f[5][0] = f[5][2];  f[5][2] = tmp; //Left Face Corners
	tmp = f[3][1];  f[3][1] =  f[4][0]; f[4][0] = f[5][1]; f[5][1] = f[4][2];  f[4][2] = tmp; //Left Face Edges
	tmp = f[9][3];  f[9][3] =  f[6][3]; f[6][3] = f[3][3]; f[3][3] = f[0][3];  f[0][3] = tmp; //Left Edge Corners
	tmp = f[10][3]; f[10][3] = f[7][3]; f[7][3] = f[4][3]; f[4][3] = f[1][3];  f[1][3] = tmp; //Left Edge Edges
	tmp = f[11][3]; f[11][3] = f[8][3]; f[8][3] = f[5][3]; f[5][3] = f[2][3];  f[2][3] = tmp; //Left Edge Corners Part 2 TODO
}
void frontInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	front(p,f); front(p,f); front(p,f);
}
void backInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	back(p,f); back(p,f); back(p,f);
}
void downInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	down(p,f); down(p,f); down(p,f); 
}
void upInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	up(p,f); up(p,f); up(p,f);	
}
void rightInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	right(p,f); right(p,f); right(p,f);
}
void leftInverse(Cubelet p[3][3][3], Cubelet f[12][9]) {
	left(p,f); left(p,f); left(p,f);
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
*/