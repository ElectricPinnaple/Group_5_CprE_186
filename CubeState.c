#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define DEBUG 0
#define PRINT_MODE 0
typedef enum Cubelet Cubelet;
enum Cubelet {  WRB, WRG, WOG, WOB, WR, WO, WB, WG, //TODO does this compile to like - enum enum Cubelet { ... ???
				YRB, YRG, YOG, YOB, YR, YO, YB, YG,
				 RB,  RG,  OG,  OB,
				 W, Y, O, R, G, B, N };

char solution[350] = {0}; //The solution string (Uses Standard Nomenclature for Cubes - Top == White && Front == Green)
int solutionLen = 0; //Length of the solution string
int useUp = 0; //TODO: documentarian

Cubelet cP[3][3][3] = { //Positions of the cubes faces
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
Cubelet cF[12][9] = { //Positions of the cube faces
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

Cubelet solvedFaces[12][9] = { //A "Solved" Cube in order to check for a failed solve
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

void permutateLastLayer();
void orientLastLayer();
void doF2LSolution(Cubelet face, Cubelet corner, Cubelet edge);
void setUpForF2L(Cubelet corner, Cubelet edge);
void firstTwoLayers();

int topBottomEdgeCheck(int i, int j, int k);

void whiteCross();
void findCube(Cubelet c, int* i, int* j, int* k);
void moveEdgeToBottom(int i, int j, int k);
void moveEdgeToColumn(Cubelet column, int i, int j, int k);
void putEdgeInPosition(int i, int j, int k);
int whiteCrossOrientationCheck(int i, int j, int k);

void printCube(int which);
void shortenSolution();
void cubeInput(char* input);

void front(Cubelet f);
void back(Cubelet f);
void down();
void up();
void right(Cubelet f);
void left(Cubelet f);
void frontInverse(Cubelet f);
void backInverse(Cubelet f);
void downInverse();
void upInverse();
void rightInverse(Cubelet f);
void leftInverse(Cubelet f);
int checkSolved(); //TODO: move main to the end? or maybe leave this and give brief overview of use? ... idk, something like that
void rotateLastLayer();

int setSerialPort(int fd, int speed) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);

	if (tcgetattr (fd, &tty) != 0) {
		printf("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	tty.c_cc[VMIN] = 1; //OR ZERO - not sure which
	tty.c_cc[VTIME] = 5;

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		printf("error %d from tcsetattr", errno);
		return -1;
	}

	return 0;
}

void randomizeCube() {
	int i;
		srand(time(NULL));
		for (i=0;i<20;i++) {
			int r = rand() % 12;
 			switch (r) {
				case  0:
					right(G);
					break;
				case  1:
					left(G);
					break;
				case  2:
					up(G);
					break;
				case  3:
					down(G);
					break;
				case  4:
					front(G);
					break;
				case  5:
					back(G);
					break;
				case  6:
					rightInverse(G);
					break;
				case  7:
					leftInverse(G);
					break;
				case  8:
					upInverse(G);
					break;
				case  9:
					downInverse(G);
					break;
				case 10:
					frontInverse(G);
					break;
				case 11:
					backInverse(G);
					break;
			}
		} //Randomize Cube State
		//strcat(solution, " | ");
		//solutionLen+=3;
		strcpy(solution, "");
		solutionLen = 0;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Input Format: [Arduino Device Name (Or '0' for none)] [Cube State (ex. RROBWRG...GB) (Or '0' for randomize)]\n");
		return 1;
	}

	if (1==1) {
		do {
			if (strcmp(argv[2], "0")) cubeInput(argv[2]);
			else { randomizeCube(); }

			whiteCross();
			if (DEBUG) printCube(PRINT_MODE);
			firstTwoLayers();
			if (DEBUG) printCube(PRINT_MODE);
			orientLastLayer();
			if (DEBUG) printCube(PRINT_MODE);
			permutateLastLayer();
			if (DEBUG) printCube(PRINT_MODE);
			rotateLastLayer();
			shortenSolution();
			if (DEBUG) printCube(PRINT_MODE);
		
			if (checkSolved()) {
				printf("SOLVED - SOLVED - SOLVED\n");
			}
			else {
				printf("UNSOLVED - UNSOLVED - UNSOLVED\n");
				return 1;
			}
		
			printf("%s\n", solution);
			//printf("%d\n", solutionLen);
			if (DEBUG) printf("\n-----------------------------------------------------\n\n");
			if (DEBUG) {
				unsigned int retTime = time(0) + 1;
    			while (time(0) < retTime);
    		}
		} while (DEBUG);
	}

	//Do Serial Port Stuff
	if (strcmp(argv[1], "0")) {
		//char* devicePort = "/dev/cu.usbmodem14301";
		int fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
		if (fd < 0) {
			printf("\nerror %d opening %s: %s\n", errno, argv[1], strerror(errno));
			return 1;
		}
		setSerialPort(fd, B9600);

		if (useUp) { printf("PUT THE TOP ON\n"); } //45 seconds to put the top on if needed
		unsigned int retTime = time(0) + 3 + (useUp * 45);
    	while (time(0) < retTime);


		//Limit the write to 64 bytes each time because the arduino's serial buffer is that size
		//The following assumes that a solution does not exceed 126 moves, that should be a safe assumption
		//TODO: add code so that it waits for a done signal before sending the second half(and starting [done above])
    	char send[128];

    	strncpy(send, solution, 63);
    	printf("%s\n", send);
    	write(fd, send, 63);
    	write(fd, "Z", 1);

    	char terminateString[16] = {0};
    	while (read(fd, terminateString, 16) == 0) {}
    	read(fd, terminateString, 16);

    	strcpy(send, &solution[63]);
		printf("%s\n", send);
    	write(fd, send, strlen(send));
    	write(fd, "Z", 1);
    	while (read(fd, terminateString, 16) == 0) {}

    	//printf("END PROGRAM\n");
	}
	return 0;
}

int checkSolved() {
	int i,j;
	for (i=0; i<12; i++) {
		for (j=0; j<9; j++) {
			if (cF[i][j] != solvedFaces[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

void rotateLastLayer() {
	int test = 0;
	while (cF[5][4] != G && test < 5) { //TODO: this is to prevent an infinite loop - could take it out if i want, or not... idk
		down();
		test++;
	}
}

void permutateLastLayer() { //TODO: this whole thing can drop checking the center cubelet, but I mean, it does not have to
	Cubelet locations[4][3][3] = { //Correct Positions for the Cubes on each rotation
			{ { YRB,  YB, YOB }, //ONE
			  {  YR,   Y,  YO },
			  { YRG,  YG, YOG }
			}, {
			  { YRG,  YR, YRB }, //TWO
			  {  YG,   Y,  YB },
			  { YOG,  YO, YOB }
			}, {
			  { YOG,  YG, YRG }, //THREE
			  {  YO,   Y,  YR },
			  { YOB,  YB, YRB }
			}, {
			  { YOB,  YO, YOG }, //FOUR
			  {  YB,   Y,  YG },
			  { YRB,  YR, YRG }
			} }; //TODO: remove the trailing down's because they are not helpful at all
	if (DEBUG) printf("PLL - ");
	int i,j;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][2] //1
			 && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
			 && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("XX\n");	
				//Last layer is already solved
				return;
			}	
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][2][2] //1
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("AA\n");	
				leftInverse(G); front(G); leftInverse(G); back(G); back(G); left(G); frontInverse(G); leftInverse(G); back(G); back(G); left(G); left(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][2][0] //2
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][2] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][2]) {
				if (DEBUG) printf("BB\n");	
				left(G); backInverse(G); left(G); front(G); front(G); leftInverse(G); back(G); left(G); front(G); front(G); left(G); left(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][2] //3
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][2][1]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][0] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("CC\n");	
				left(G); left(G); down(G); left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); downInverse(G); leftInverse(G); down(G); leftInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][2] //4
				  && cP[1][2][2] == locations[i][2][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][0]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][2] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("DD\n");	
				left(G); downInverse(G); left(G); down(G); left(G); down(G); left(G); downInverse(G); leftInverse(G); downInverse(G); left(G); left(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][2][1] && cP[0][2][0] == locations[i][0][2] //5
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][0]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][0][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("EE\n");	
				left(G); left(G); rightInverse(G); rightInverse(G); up(G); left(G); left(G); rightInverse(G); rightInverse(G); down(G); down(G); left(G); left(G); rightInverse(G); rightInverse(G); up(G); left(G); left(G); rightInverse(G); rightInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][2][2] //6
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][0]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][2]) {
				if (DEBUG) printf("FF\n");	
				left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); front(G); left(G); left(G); downInverse(G); leftInverse(G); downInverse(G); left(G); down(G); leftInverse(G); frontInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][1][0] && cP[0][2][0] == locations[i][0][0] //7
				  && cP[1][2][2] == locations[i][0][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("GG\n");
				leftInverse(G); down(G); rightInverse(G); down(G); down(G); left(G); downInverse(G); leftInverse(G); down(G); down(G); left(G); right(G); downInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][2][2] //8
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][2][1]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][2] && cP[2][2][0] == locations[i][0][2]) {
				if (DEBUG) printf("HH\n");	
				left(G); down(G); leftInverse(G); frontInverse(G); left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); front(G); left(G); left(G); downInverse(G); leftInverse(G); downInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][0] //9
				  && cP[1][2][2] == locations[i][2][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][0] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("II\n");	
				right(G); down(G); down(G); rightInverse(G); down(G); down(G); right(G); frontInverse(G); rightInverse(G); downInverse(G); right(G); down(G); right(G); front(G); right(G); right(G); down(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][0] //10
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][2][1]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][2] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("JJ\n");	
				leftInverse(G); down(G); down(G); left(G); down(G); down(G); leftInverse(G); front(G); left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); frontInverse(G); left(G); left(G); downInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][2][2] && cP[0][2][1] == locations[i][1][2] && cP[0][2][0] == locations[i][0][2] //11
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][0][1]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("KK\n");	
				leftInverse(G); down(G); leftInverse(G); downInverse(G); backInverse(G); leftInverse(G); back(G); back(G); downInverse(G); backInverse(G); down(G); backInverse(G); left(G); back(G); left(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][1][0] && cP[0][2][0] == locations[i][2][0] //12
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][0][1]
				  && cP[2][2][2] == locations[i][0][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("LL\n");	
				left(G); left(G); up(G); backInverse(G); down(G); backInverse(G); downInverse(G); back(G); upInverse(G); left(G); left(G); frontInverse(G); down(G); front(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][2][0] && cP[0][2][1] == locations[i][2][1] && cP[0][2][0] == locations[i][0][2] //13
				  && cP[1][2][2] == locations[i][0][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][2] && cP[2][2][1] == locations[i][1][0] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("MM\n");	
				leftInverse(G); downInverse(G); left(G); back(G); back(G); up(G); rightInverse(G); down(G); right(G); downInverse(G); right(G); upInverse(G); back(G); back(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][2][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][2] //14
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][2][1]
				  && cP[2][2][2] == locations[i][2][2] && cP[2][2][1] == locations[i][1][0] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("NN\n");	
				left(G); left(G); upInverse(G); front(G); downInverse(G); front(G); down(G); frontInverse(G); up(G); left(G); left(G); back(G); downInverse(G); backInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][2] && cP[0][2][1] == locations[i][1][0] && cP[0][2][0] == locations[i][2][0] //15
				  && cP[1][2][2] == locations[i][2][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][0][0] && cP[2][2][1] == locations[i][0][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("OO\n");	
				left(G); down(G); leftInverse(G); front(G); front(G); upInverse(G); right(G); downInverse(G); rightInverse(G); down(G); rightInverse(G); up(G); front(G); front(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][0][2] //16
				  && cP[1][2][2] == locations[i][1][2] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][0]
				  && cP[2][2][2] == locations[i][2][2] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][2][0]) {
				if (DEBUG) printf("PP\n");	
				leftInverse(G); down(G); down(G); leftInverse(G); downInverse(G); backInverse(G); leftInverse(G); back(G); back(G); downInverse(G); backInverse(G); down(G); backInverse(G); left(G); back(G); downInverse(G); left(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][1][0] && cP[0][2][0] == locations[i][0][2] //17
				  && cP[1][2][2] == locations[i][0][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][2][1]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][1][2] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("QQ\n");	
				left(G); left(G); rightInverse(G); rightInverse(G); up(G); left(G); left(G); rightInverse(G); rightInverse(G); down(G); leftInverse(G); right(G); front(G); front(G); left(G); left(G); rightInverse(G); rightInverse(G); back(G); back(G); leftInverse(G); right(G); down(G); down(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][2][2] && cP[0][2][1] == locations[i][1][0] && cP[0][2][0] == locations[i][0][2] //18
				  && cP[1][2][2] == locations[i][0][1] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("RR\n");
				front(G); left(G); downInverse(G); leftInverse(G); downInverse(G); left(G); down(G); leftInverse(G); frontInverse(G); left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); front(G); left(G); frontInverse(G);	
				return;
			}
			else if (cP[0][2][2] == locations[i][2][2] && cP[0][2][1] == locations[i][2][1] && cP[0][2][0] == locations[i][0][2] //19
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][2][0] && cP[2][2][1] == locations[i][0][1] && cP[2][2][0] == locations[i][0][0]) {
				if (DEBUG) printf("SS\n");	
				right(G); downInverse(G); left(G); down(G); down(G); rightInverse(G); down(G); leftInverse(G); right(G); downInverse(G); left(G); down(G); down(G); rightInverse(G); down(G); leftInverse(G); down(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][0][0] && cP[0][2][1] == locations[i][2][1] && cP[0][2][0] == locations[i][2][0] //20
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][0][2] && cP[2][2][1] == locations[i][0][1] && cP[2][2][0] == locations[i][2][2]) {
				if (DEBUG) printf("TT\n");	
				leftInverse(G); down(G); rightInverse(G); down(G); down(G); left(G); downInverse(G); right(G); leftInverse(G); down(G); rightInverse(G); down(G); down(G); left(G); downInverse(G); right(G); downInverse(G);
				return;
			}
			else if (cP[0][2][2] == locations[i][2][0] && cP[0][2][1] == locations[i][0][1] && cP[0][2][0] == locations[i][2][2] //21 //This can be changed to just an else
				  && cP[1][2][2] == locations[i][1][0] && cP[1][2][1] == locations[i][1][1] && cP[1][2][0] == locations[i][1][2]
				  && cP[2][2][2] == locations[i][0][0] && cP[2][2][1] == locations[i][2][1] && cP[2][2][0] == locations[i][0][2]) {
				if (DEBUG) printf("UU\n");	
				left(G); backInverse(G); leftInverse(G); front(G); left(G); back(G); leftInverse(G); front(G); front(G); rightInverse(G); back(G); right(G); front(G); rightInverse(G); backInverse(G); right(G);
				return;
			}

			down(G); //Rotate the last layer to look for a matching algorithm again.
		}
	}
	if (DEBUG) printf("FAIL\n");
}

void orientLastLayer() {
	if (DEBUG) printf("OLL - ");
	int i;
	for (i=0; i<4; i++) {
		if    (cF[7][4] == Y && cF[9][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			&& cF[5][2] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("AA\n");
			left(G); down(G); backInverse(G); left(G); back(G); leftInverse(G); leftInverse(G); downInverse(G); leftInverse(G); front(G); left(G); frontInverse(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[9][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("BB\n");
			leftInverse(G); front(G); left(G); frontInverse(G); down(G); down(G); leftInverse(G); front(G); left(G); front(G); front(G); down(G); down(G); front(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[6][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("CC\n");
			frontInverse(G); back(G); back(G); right(G); backInverse(G); right(G); front(G); down(G); down(G); frontInverse(G); right(G); front(G); backInverse(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][4] == Y && cF[9][4] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("DD\n");
			leftInverse(G); down(G); down(G); leftInverse(G); front(G); left(G); frontInverse(G); downInverse(G); frontInverse(G); downInverse(G); front(G); downInverse(G); left(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[7][4] == Y && cF[6][3] == Y && cF[9][4] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("EE\n");
			left(G); down(G); leftInverse(G); down(G); leftInverse(G); front(G); left(G); frontInverse(G); down(G); down(G); leftInverse(G); front(G); left(G); frontInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[6][5] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[5][1] == Y
			  && cF[5][4] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("FF\n");
			right(G); leftInverse(G); front(G); front(G); rightInverse(G); left(G); down(G); down(G); right(G); leftInverse(G); front(G); rightInverse(G); left(G); down(G); down(G); right(G); leftInverse(G); front(G); front(G); rightInverse(G); left(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[9][4] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("GG\n");
			leftInverse(G); down(G); down(G); front(G); left(G); down(G); leftInverse(G); downInverse(G); front(G); front(G); down(G); down(G); front(G); left(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[6][5] == Y
			  && cF[6][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][4] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("HH\n");
			front(G); left(G); down(G); leftInverse(G); down(G); frontInverse(G); down(G); down(G); frontInverse(G); right(G); front(G); rightInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[6][4] == Y && cF[9][5] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][5] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("II\n");
			leftInverse(G); downInverse(G); frontInverse(G); down(G); frontInverse(G); right(G); front(G); rightInverse(G); front(G); left(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[6][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][6] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("JJ\n");
			left(G); downInverse(G); back(G); back(G); up(G); backInverse(G); down(G); down(G); back(G); upInverse(G); back(G); back(G); down(G); leftInverse(G);
			return;	
		} 
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("KK\n");
			front(G); down(G); left(G); downInverse(G); leftInverse(G); down(G); left(G); downInverse(G); leftInverse(G); frontInverse(G);
			return;	//11
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("LL\n");
			rightInverse(G); backInverse(G); right(G); downInverse(G); leftInverse(G); down(G); left(G); downInverse(G); leftInverse(G); down(G); left(G); rightInverse(G); back(G); right(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[9][5] == Y && cF[5][0] == Y
			  && cF[5][2] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("MM\n");
			right(G); downInverse(G); leftInverse(G); down(G); rightInverse(G); down(G); left(G); down(G); leftInverse(G); down(G); left(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[5][0] == Y
			  && cF[5][2] == Y && cF[5][6] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("NN\n");
			left(G); down(G); leftInverse(G); down(G); left(G); downInverse(G); leftInverse(G); down(G); left(G); down(G); down(G); leftInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][3] == Y && cF[5][5] == Y  && cF[5][8] == Y) {
			if (DEBUG) printf("OO\n");
			rightInverse(G); down(G); left(G); downInverse(G); right(G); down(G); leftInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][5] == Y && cF[5][0] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("PP\n");
			leftInverse(G); down(G); down(G); left(G); down(G); leftInverse(G); down(G); left(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y //17
			  && cF[6][4] == Y && cF[9][3] == Y && cF[5][3] == Y) {
			if (DEBUG) printf("QQ\n");
			leftInverse(G); frontInverse(G); right(G); front(G); left(G); frontInverse(G); rightInverse(G); front(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[5][3] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("RR\n");
			left(G); left(G); up(G); leftInverse(G); down(G); down(G); left(G); upInverse(G); leftInverse(G); down(G); down(G); leftInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][3] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("SS\n");
			leftInverse(G); frontInverse(G); rightInverse(G); front(G); left(G); frontInverse(G); right(G); front(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[6][5] == Y //20
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][4] == Y && cF[5][1] == Y) {
			if (DEBUG) printf("TT\n");
			leftInverse(G); right(G); frontInverse(G); left(G); rightInverse(G); down(G); down(G); leftInverse(G); right(G); frontInverse(G); left(G); rightInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("UU\n");
			rightInverse(G); left(G); down(G); leftInverse(G); downInverse(G); right(G); leftInverse(G); front(G); left(G); frontInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[6][5] == Y
			  && cF[9][5] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("VV\n");
			right(G); front(G); leftInverse(G); front(G); left(G); front(G); front(G); rightInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[6][3] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][4] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("WW\n");
			front(G); leftInverse(G); frontInverse(G); left(G); down(G); left(G); downInverse(G); leftInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][4] == Y && cF[5][2] == Y && cF[5][5] == Y
			  && cF[5][7] == Y && cF[9][3] == Y && cF[9][4] == Y) {
			if (DEBUG) printf("XX\n"); //TODO: FIX
			leftInverse(G); downInverse(G); left(G); front(G); leftInverse(G); frontInverse(G); down(G); front(G); left(G); frontInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[9][4] == Y && cF[5][5] == Y && cF[5][3] == Y
			  && cF[5][7] == Y) {
			if (DEBUG) printf("YY\n"); //TODO: FIX
			downInverse(G); left(G); down(G); down(G); leftInverse(G); downInverse(G); left(G); downInverse(G); left(G); left(G); frontInverse(G); downInverse(G); front(G); down(G); left(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("ZZ\n");
			front(G); left(G); down(G); leftInverse(G); downInverse(G); left(G); down(G); leftInverse(G); downInverse(G); frontInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[9][5] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][4] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("aa\n");
			right(G); frontInverse(G); rightInverse(G); front(G); down(G); down(G); right(G); right(G); back(G); right(G); backInverse(G); right(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[6][3] == Y && cF[9][5] == Y && cF[9][3] == Y && cF[5][4] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("bb\n");
			downInverse(G); leftInverse(G); down(G); down(G); left(G); down(G); leftInverse(G); down(G); left(G); left(G); back(G); down(G); backInverse(G); downInverse(G); leftInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[5][2] == Y 
			  && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("cc\n");
			right(G); front(G); front(G); leftInverse(G); frontInverse(G); left(G); frontInverse(G); rightInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y //30
			  && cF[9][5] == Y && cF[5][2] == Y && cF[5][4] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("dd\n");
			leftInverse(G); down(G); down(G); left(G); left(G); backInverse(G); leftInverse(G); back(G); leftInverse(G); down(G); down(G); left(G);
			//frontInverse(G); rightInverse(G); downInverse(G); right(G); down(G); rightInverse(G); downInverse(G); right(G); down(G); front(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[9][5] == Y && cF[5][0] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("ee\n");
			frontInverse(G); rightInverse(G); downInverse(G); right(G); down(G); rightInverse(G); downInverse(G); right(G); down(G); front(G);
			//leftInverse(G); down(G); down(G); left(G); left(G); backInverse(G); leftInverse(G); back(G); leftInverse(G); down(G); down(G); left(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[9][3] == Y
			  && cF[5][3] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("ff\n");
			leftInverse(G); front(G); leftInverse(G); frontInverse(G); left(G); left(G); down(G); down(G); backInverse(G); left(G); back(G); leftInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[9][5] == Y && cF[9][3] == Y //Possible misordering or things... just watch it
			  && cF[5][3] == Y && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("gg\n");
			leftInverse(G); front(G); left(G); frontInverse(G); down(G); down(G); left(G); left(G); backInverse(G); leftInverse(G); back(G); leftInverse(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y
			  && cF[6][4] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("hh\n");
			left(G); down(G); leftInverse(G); backInverse(G); left(G); back(G); downInverse(G); backInverse(G); leftInverse(G); back(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[7][5] == Y && cF[7][4] == Y
			  && cF[6][4] == Y && cF[9][4] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][5] == Y) {
			printf("ii\n");
			rightInverse(G); backInverse(G); right(G); downInverse(G); leftInverse(G); down(G); left(G); rightInverse(G); back(G); right(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[6][4] == Y && cF[6][3] == Y && cF[9][4] == Y && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][5] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("jj\n");
			down(G); down(G); right(G); leftInverse(G); leftInverse(G); frontInverse(G); left(G); frontInverse(G); leftInverse(G); front(G); front(G); left(G); frontInverse(G); rightInverse(G); left(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y
			  && cF[6][4] == Y && cF[9][4] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("kk\n");
			backInverse(G); left(G); backInverse(G); left(G); left(G); down(G); left(G); down(G); leftInverse(G); downInverse(G); left(G); back(G); back(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[7][3] == Y && cF[6][4] == Y && cF[9][4] == Y && cF[9][3] == Y
			  && cF[5][3] == Y && cF[5][6] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("ll\n");
			right(G); downInverse(G); frontInverse(G); down(G); down(G); frontInverse(G); down(G); front(G); downInverse(G); front(G); down(G); down(G); front(G); downInverse(G); rightInverse(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[6][4] == Y && cF[9][5] == Y && cF[9][4] == Y
			  && cF[5][0] == Y && cF[5][3] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("mm\n");
			down(G); down(G); leftInverse(G); right(G); right(G); front(G); rightInverse(G); front(G); right(G); front(G); front(G); rightInverse(G); front(G); left(G); rightInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y //40
			  && cF[6][4] == Y && cF[9][4] == Y
			  && cF[5][2] == Y && cF[5][6] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("nn\n");
			leftInverse(G); leftInverse(G); down(G); leftInverse(G); backInverse(G); left(G); downInverse(G); leftInverse(G); leftInverse(G); down(G); left(G); back(G); leftInverse(G);
			//rightInverse(G); back(G); back(G); left(G); back(G); leftInverse(G); back(G); right(G);
			return;	
		}
		else if (cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][6] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("oo\n");
			rightInverse(G); back(G); back(G); left(G); back(G); leftInverse(G); back(G); right(G);
			//left(G); left(G); down(G); leftInverse(G); backInverse(G); left(G); downInverse(G); left(G); left(G); down(G); left(G); front(G); leftInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][4] == Y && cF[6][5] == Y
			  && cF[6][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("pp\n");
			left(G); down(G); left(G); backInverse(G); leftInverse(G); back(G); downInverse(G); leftInverse(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][4] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("qq\n");
			left(G); down(G); leftInverse(G); downInverse(G); backInverse(G); leftInverse(G); front(G); left(G); frontInverse(G); back(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[9][3] == Y
			  && cF[5][4] == Y && cF[5][5] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("rr\n");
			leftInverse(G); front(G); left(G); down(G); leftInverse(G); frontInverse(G); left(G); front(G); downInverse(G); frontInverse(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[9][5] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][3] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("ss\n");
			right(G); frontInverse(G); rightInverse(G); downInverse(G); right(G); front(G); rightInverse(G); frontInverse(G); down(G); front(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][0] == Y
			  && cF[5][4] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("tt\n");
			rightInverse(G); backInverse(G); right(G); leftInverse(G); downInverse(G); left(G); down(G); rightInverse(G); back(G); right(G);
			return;	
		}
		else if (cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[8][3] == Y
			  && cF[9][4] == Y && cF[5][5] == Y
			  && cF[5][2] == Y && cF[5][4] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("uu\n"); //TODO: FIX
			left(G); back(G); leftInverse(G); right(G); down(G); rightInverse(G); downInverse(G); left(G); backInverse(G); leftInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[6][5] == Y
			  && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][2] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("vv\n");
			front(G); down(G); left(G); downInverse(G); leftInverse(G); frontInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][5] == Y && cF[5][4] == Y && cF[5][5] == Y && cF[5][7] == Y) {
			if (DEBUG) printf("ww\n");
			leftInverse(G); downInverse(G); front(G); down(G); left(G); downInverse(G); leftInverse(G); frontInverse(G); left(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[8][4] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[6][5] == Y
			  && cF[9][3] == Y && cF[5][1] == Y
			  && cF[5][3] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("xx\n");
			right(G); down(G); frontInverse(G); downInverse(G); rightInverse(G); down(G); right(G); front(G); rightInverse(G);
			return;	
		}
		else if (cF[8][4] == Y && cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("yy\n");
			frontInverse(G); downInverse(G); rightInverse(G); down(G); right(G); front(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[5][4] == Y && cF[5][6] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("zz\n");
			front(G); left(G); down(G); leftInverse(G); downInverse(G); frontInverse(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][5] == Y && cF[9][4] == Y && cF[5][4] == Y && cF[5][5] == Y) {
			if (DEBUG) printf("11\n");
			left(G); down(G); leftInverse(G); downInverse(G); leftInverse(G); front(G); left(G); frontInverse(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[6][4] == Y && cF[9][4] == Y && cF[5][3] == Y && cF[5][7] == Y && cF[5][8] == Y) {
			if (DEBUG) printf("22\n");
			right(G); down(G); rightInverse(G); down(G); right(G); downInverse(G); rightInverse(G); downInverse(G); rightInverse(G); back(G); right(G); backInverse(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[7][5] == Y && cF[7][4] == Y
			  && cF[6][4] == Y && cF[6][3] == Y && cF[9][4] == Y && cF[5][0] == Y && cF[5][1] == Y
			  && cF[5][5] == Y) {
			if (DEBUG) printf("33\n");
			leftInverse(G); downInverse(G); left(G); downInverse(G); leftInverse(G); down(G); left(G); down(G); left(G); backInverse(G); leftInverse(G); back(G);
			return;	
		}
		else if (cF[8][5] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y
			  && cF[6][3] == Y && cF[9][4] == Y && cF[9][3] == Y
			  && cF[5][4] == Y && cF[5][6] == Y) {
			if (DEBUG) printf("44\n");
			leftInverse(G); front(G); left(G); down(G); leftInverse(G); downInverse(G); frontInverse(G); down(G); left(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[7][5] == Y && cF[7][4] == Y && cF[7][3] == Y && cF[6][5] == Y
			  && cF[9][5] == Y && cF[9][4] == Y && cF[5][2] == Y && cF[5][4] == Y) {
			if (DEBUG) printf("55\n");
			right(G); frontInverse(G); rightInverse(G); downInverse(G); right(G); down(G); front(G); downInverse(G); rightInverse(G);
			return;	
		}
		else if (cF[8][3] == Y && cF[8][4] == Y && cF[8][5] == Y &&
				 cF[7][3] == Y && cF[7][4] == Y && cF[7][5] == Y &&
				 cF[6][3] == Y && cF[6][4] == Y && cF[6][5] == Y)	{
			//Last layer is already oriented.
			return;
		}
		down();
	}
	if (DEBUG) printf("FAIL\n");
}

int inputToFaces(char c) {
	switch (c) {
		case 'Y':
			return Y;
		case 'W':
			return W;
		case 'R':
			return R;
		case 'O':
			return O;
		case 'B':
			return B;
		case 'G':
			return G;
		default:
			return -999;
	}
}

int inputToCubes(char a, char b, char c) {
	switch (a+b+c) {
		case 235:
			return WRB;
		case 240:
			return WRG;
		case 232:
			return WOB;
		case 169:
			return WR;
		case 166:
			return WO;
		case 158:
			return WG;
		case 242:
			return YRG;
		case 239:
			return YOG;
		case 234:
			return YOB;
		case 171:
			return YR;
		case 168:
			return YO;
		case 155:
			return YB;
		case 160:
			return YG;
		case 148:
			return RB;
		case 150:
			return OG;
		case 145:
			return OB;
		case 153: //Deal with the cases where the sums clash
			if (a == 'W' || a == 'B') return WB;
			else return RG;
		case 237:
			if (a == 'W' || a == 'O' || a == 'G') return WOG;
			else return YRB;
		default:
			return -999;
	}
}

void cubeInput(char* input) {
	//if (length != 55) { //TODO: this may be the wrong length
		cF[0][3]  = inputToFaces(input[0]);
		cF[0][4]  = inputToFaces(input[1]);
		cF[0][5]  = inputToFaces(input[2]);
		cF[1][3]  = inputToFaces(input[3]);
		cF[1][4]  = inputToFaces(input[4]);
		cF[1][5]  = inputToFaces(input[5]);
		cF[2][3]  = inputToFaces(input[6]);
		cF[2][4]  = inputToFaces(input[7]);
		cF[2][5]  = inputToFaces(input[8]);
		cF[3][3]  = inputToFaces(input[9]);
		cF[3][4]  = inputToFaces(input[10]);
		cF[3][5]  = inputToFaces(input[11]);
		cF[4][3]  = inputToFaces(input[12]);
		cF[4][4]  = inputToFaces(input[13]);
		cF[4][5]  = inputToFaces(input[14]);
		cF[5][3]  = inputToFaces(input[15]);
		cF[5][4]  = inputToFaces(input[16]);
		cF[5][5]  = inputToFaces(input[17]);
		cF[6][3]  = inputToFaces(input[18]);
		cF[6][4]  = inputToFaces(input[19]);
		cF[6][5]  = inputToFaces(input[20]);
		cF[7][3]  = inputToFaces(input[21]);
		cF[7][4]  = inputToFaces(input[22]);
		cF[7][5]  = inputToFaces(input[23]);
		cF[8][3]  = inputToFaces(input[24]);
		cF[8][4]  = inputToFaces(input[25]);
		cF[8][5]  = inputToFaces(input[26]);
		cF[9][3]  = inputToFaces(input[27]);
		cF[9][4]  = inputToFaces(input[28]);
		cF[9][5]  = inputToFaces(input[29]);
		cF[10][3] = inputToFaces(input[30]);
		cF[10][4] = inputToFaces(input[31]);
		cF[10][5] = inputToFaces(input[32]);
		cF[11][3] = inputToFaces(input[33]);
		cF[11][4] = inputToFaces(input[34]);
		cF[11][5] = inputToFaces(input[35]);
		cF[3][0]  = inputToFaces(input[36]);
		cF[3][1]  = inputToFaces(input[37]);
		cF[3][2]  = inputToFaces(input[38]);
		cF[4][0]  = inputToFaces(input[39]);
		cF[4][1]  = inputToFaces(input[40]);
		cF[4][2]  = inputToFaces(input[41]);
		cF[5][0]  = inputToFaces(input[42]);
		cF[5][1]  = inputToFaces(input[43]);
		cF[5][2]  = inputToFaces(input[44]);
		cF[3][6]  = inputToFaces(input[45]);
		cF[3][7]  = inputToFaces(input[46]);
		cF[3][8]  = inputToFaces(input[47]);
		cF[4][6]  = inputToFaces(input[48]);
		cF[4][7]  = inputToFaces(input[49]);
		cF[4][8]  = inputToFaces(input[50]);
		cF[5][6]  = inputToFaces(input[51]);
		cF[5][7]  = inputToFaces(input[52]);
		cF[5][8]  = inputToFaces(input[53]);

		cP[0][0][0] = inputToCubes(input[0],  input[33], input[36]);
		cP[0][0][1] = inputToCubes(input[1],  input[34], 0);
		cP[0][0][2] = inputToCubes(input[2],  input[47], input[35]);
		cP[0][1][0] = inputToCubes(input[39], input[30], 0);
		cP[0][1][2] = inputToCubes(input[50], input[32], 0);
		cP[0][2][0] = inputToCubes(input[42], input[24], input[27]);
		cP[0][2][1] = inputToCubes(input[25], input[28], 0);
		cP[0][2][2] = inputToCubes(input[53], input[26], input[29]);
		cP[1][0][0] = inputToCubes(input[3],  input[37], 0);
		cP[1][0][2] = inputToCubes(input[5],  input[46], 0);
		cP[1][2][0] = inputToCubes(input[43], input[21], 0);
		cP[1][2][2] = inputToCubes(input[52], input[23], 0);
		cP[2][0][0] = inputToCubes(input[6],  input[9],  input[38]);
		cP[2][0][1] = inputToCubes(input[7],  input[10], 0);
		cP[2][0][2] = inputToCubes(input[8],  input[11], input[45]);
		cP[2][1][0] = inputToCubes(input[12], input[41], 0);
		cP[2][1][2] = inputToCubes(input[14], input[48], 0);
		cP[2][2][0] = inputToCubes(input[44], input[15], input[18]);
		cP[2][2][1] = inputToCubes(input[16], input[19], 0);
		cP[2][2][2] = inputToCubes(input[17], input[20], input[51]);
	//}
}

void shortenSolution() { //TODO: Deal with four of something in a row & inverse next to regular move
	int iter;
	for (iter=0; iter<2; iter++) { //TODO: Itterate over the solution x times, this solution is not perfect, just about optimizing to get best return on investments
		int i, j, cnt;
		char theC;
		for (i=0; i<solutionLen; i++) {
			theC = solution[i];
			cnt = 1;
			j = i+1;
			while (j < solutionLen && solution[j] == theC) {
				cnt++;
				j++;
			}
			while (cnt >= 4) {
				int k;
				for (k = i; k<solutionLen-4; k++) {
					solution[k] = solution[k+4];
				}	
				solution[k] = 0;
				cnt -= 4;
				solutionLen -= 4;
			}
			if (cnt == 3) {
				solution[i] = tolower(solution[i]);
				int k;
				for (k = i+1; k<solutionLen-2; k++) {
					solution[k] = solution[k+2];
				}	
				solution[k] = 0;
				solutionLen -= 2;
			}
		}
		/* //TODO: whatever this is
		for (i=0; i<solutionLen-1; i++) {
			if (((isupper(solution[i]) && islower(solution[i+1])) || (islower(solution[i]) && isupper(solution[i+1]))) && (toupper(solution[i]) == toupper(solution[i+1]))) {
			printf("\n\n\nNo idea if this works because i have never gotten it to trigger.\nMessage From - shortenSolution()\n\n\n");
				if (i == solutionLen-2) {
					solutionLen -= 2;
					solution[i]=0;
				}
				else {
					for (j=i; j<solutionLen-2; j++) {
						solution[j] = solution[j+2];
					}	
					solutionLen -= 2;
				}
			}
		}
		*/
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

void firstTwoLayers() {
 //I lost those things that i wrote... but i remember so... dope

	setUpForF2L(WOG, OG);
	if (DEBUG) printCube(1);
	if (DEBUG) printf("WOG - ");
	doF2LSolution(G, WOG, OG);

	setUpForF2L(WRB, RB); //TODO: I'm getting "Abort Trap: 6" for some reason (writing to memory I dont own???)... idk
	if (DEBUG) printCube(1);
	if (DEBUG) printf("WRB - ");
	doF2LSolution(B, WRB, RB);

	setUpForF2L(WOB, OB);
	if (DEBUG) printCube(1);
	if (DEBUG) printf("WOB - ");
	doF2LSolution(O, WOB, OB);

	setUpForF2L(WRG, RG); //TODO: also got "Abort Trap: 6" ... idk my man, deal with it later i guess
	if (DEBUG) printCube(1);
	if (DEBUG) printf("WRG - ");
	doF2LSolution(R, WRG, RG);

	//TODO
}

void doF2LSolution(Cubelet face, Cubelet corner, Cubelet edge) {
	int i, j, k;
	int corner_layer,
		corner_orientation, //0 == "W on TB" - 1 == "face on TB" - 2 == "other on TB" //TODO: make a pretty thing saying what all this
		edge_position, //4 == "in position" - 0-3 == "start above face and work around clockwise"
		edge_orientation; ////0 == "correct (correct means face == TB" - 1 == "incorrect"

	findCube(corner, &i, &j, &k);
	corner_layer = j;
	Cubelet cornerCheck;

	if(corner_layer == 0) {
		if (face == G) {
			cornerCheck = cF[2][3];
		}
		else if (face == R) {
			cornerCheck = cF[2][5];
		}
		else if (face == B) {
			cornerCheck = cF[0][5];
		}
		else {
			cornerCheck = cF[0][3];
		}
	}
	else {
		if (face == G) {
			cornerCheck = cF[6][3];
		}
		else if (face == R) {
			cornerCheck = cF[6][5];
		}
		else if (face == B) {
			cornerCheck = cF[8][5];
		}
		else {
			cornerCheck = cF[8][3];
		}
	}
	if (cornerCheck == W) {
		corner_orientation = 0;
	}
	else if (cornerCheck == face) {
		corner_orientation = 1;
	}
	else {
		corner_orientation = 2;
	}

	findCube(edge, &i, &j, &k);
	if (j == 1) {
		edge_position = 4;
		if (face == G) {
			if (cF[4][3] == G) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
		else if (face == R) {
			if (cF[4][6] == R) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}		}
		else if (face == B) {
			if (cF[10][5] == B) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
		else {
			if (cF[4][0] == O) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
	}
	else {
		if (i == 2) {
			edge_position = 0;
			if (cF[6][4] == face) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
		else if (k == 0) {
			edge_position = 1;
			if (cF[7][3] == face) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
		else if (i == 0) {
			edge_position = 2;
			if (cF[8][4] == face) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
		else {
			edge_position = 3;
			if (cF[7][5] == face) {
				edge_orientation = 0;
			}
			else {
				edge_orientation = 1;
			}
		}
	}
	if (edge_position != 4) {
		if (face == O) {
			edge_position = (edge_position+3)%4;
		}
		else if (face == B) {
			edge_position = (edge_position+2)%4;
		}
		else if (face == R) {
			edge_position = (edge_position+1)%4;
		}
	}

	/* from bottom
		corner position 2 == top
					  0 == bottom
		corner orientation == 0 == "W on TB" - 1 == "face on TB" - 2 == "other on TB" //TODO: make a pretty thing saying what all this
		edge position 4 == in-place
					  0 == face
					  1 == right
					  2 == back
					  3 == left
		edge orientation 0 == it is correct or face is on top
						 1 == it is incorrect or not-face is on top
	*/
	//TODO: remove - testing -- actually, maybe not... could be useful to keep around...
	//printf("Testing orientation getter-er\nCorner Position: %d\nCorner Orientation: %d\nEdge Position: %d\nEdge Orientation: %d\n", corner_layer, corner_orientation, edge_position, edge_orientation);	


	if (corner_layer == 2) {
		if (corner_orientation == 0) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); down(face); leftInverse(face);
					}
				else { //TODO: DO A SEARCH AND REPLEACE FOR () - (face)
					if (DEBUG) printf("B");
					left(face); downInverse(face); leftInverse(face); down(face);
					frontInverse(face); down(face); front(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					left(face); down(face); leftInverse(face); downInverse(face); 
					downInverse(face); left(face); down(face); leftInverse(face);
					downInverse(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("D");
					frontInverse(face); down(face); down(face); front(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					left(face); down(face); down(face); leftInverse(face);
					downInverse(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("F");
					frontInverse(face); downInverse(face); front(face); down(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					down(face); left(face); down(face); down(face);
					leftInverse(face); down(face); left(face);
					downInverse(face); leftInverse(face);	
					}
				else {
					if (DEBUG) printf("H");
					down(face); down(face); frontInverse(face); downInverse(face);
					front(face); downInverse(face); frontInverse(face); down(face);
					front(face);
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					down(face); down(face); left(face); down(face); leftInverse(face);
					down(face); left(face); downInverse(face); leftInverse(face);	
					}
				else {
					if (DEBUG) printf("J");
					downInverse(face); frontInverse(face); down(face); down(face);
					front(face); downInverse(face); frontInverse(face); down(face);
					front(face);
					}
				}
			if (DEBUG) printf("1\n");
			}
		else if (corner_orientation == 1) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					downInverse(face); left(face); downInverse(face); leftInverse(face);
					downInverse(face); left(face); down(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("B");
					downInverse(face); left(face); down(face); leftInverse(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					frontInverse(face); down(face); front(face); downInverse(face);
					downInverse(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("D");
					down(face); frontInverse(face); down(face); front(face);
					downInverse(face); frontInverse(face); downInverse(face);
					front(face);
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					down(face); left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("F");
					downInverse(face); left(face); down(face); down(face);
					leftInverse(face); down(face); frontInverse(face); downInverse(face);
					front(face);
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					downInverse(face); left(face); down(face); leftInverse(face);
					downInverse(face); left(face); down(face); down(face);
					leftInverse(face);
					}
				else {
					if (DEBUG) printf("H");
					down(face); frontInverse(face); downInverse(face); front(face);
					downInverse(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					downInverse(face); left(face); down(face); down(face);
					leftInverse(face); downInverse(face); left(face); down(face);
					down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("J");
					frontInverse(face); downInverse(face); front(face);
					}
				}
			if (DEBUG) printf("2\n");
			}
		else if (corner_orientation == 2) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					down(face); frontInverse(face); down(face); front(face);
					down(face); frontInverse(face); down(face); down(face);
					front(face);
					}
				else {
					if (DEBUG) printf("B");
					down(face); frontInverse(face); downInverse(face); front(face);
					downInverse(face);
					left(face); down(face); leftInverse(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					down(face); frontInverse(face); down(face); down(face);
					front(face); downInverse(face); left(face); down(face);
					leftInverse(face);
					}
				else {
					if (DEBUG) printf("D");
					downInverse(face); frontInverse(face); down(face); front(face);
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					downInverse(face); left(face); downInverse(face); leftInverse(face);
					down(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("F");
					left(face); downInverse(face); leftInverse(face); down(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("H");
					down(face); frontInverse(face); down(face); down(face);
					front(face); down(face); frontInverse(face); down(face);
					down(face); front(face);
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					downInverse(face); left(face); down(face); leftInverse(face);
					down(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("J");
					down(face); frontInverse(face); downInverse(face); front(face);
					down(face); frontInverse(face); down(face); down(face);
					front(face);
					}
				}
			if (DEBUG) printf("3\n");
			}
		}
	if (corner_layer == 0) {
		if (corner_orientation == 0) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					//MY WORK HERE IS DONE
					}
				else {
					if (DEBUG) printf("B");
					left(face); downInverse(face); leftInverse(face); down(face);
					frontInverse(face); down(face); down(face); front(face);
					down(face); frontInverse(face); down(face); down(face);
					front(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					downInverse(); downInverse(); frontInverse(face); down(face);
					front(face); down(face); left(face); downInverse(face);
					leftInverse(face);	
					}
				else {
					if (DEBUG) printf("D");
					down(face); left(face); downInverse(face); leftInverse(face);
					downInverse(face); frontInverse(face); down(face); front(face);	
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					downInverse(face); frontInverse(face); down(face); front(face);
					down(face); left(face); downInverse(face); leftInverse(face);	
					}
				else {
					if (DEBUG) printf("F");
					down(face); down(face); left(face); downInverse(face); leftInverse(face); downInverse(face); frontInverse(face); down(face); front(face);
					//left(face); downInverse(face); leftInverse(face); downInverse(face);
					//frontInverse(face); down(face); front(face);	
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					frontInverse(face); down(face); front(face); down(face);
					left(face); downInverse(face); leftInverse(face);	
					}
				else {
					if (DEBUG) printf("H");
					downInverse(face); left(face); downInverse(face); leftInverse(face);
					downInverse(face); frontInverse(face); down(face); front(face);	
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					down(face); frontInverse(face); down(face); front(face);
					down(face); left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("J");
					left(face); downInverse(face); leftInverse(face); downInverse(face);
					frontInverse(face); down(face); front(face);	
					}
				}
			if (DEBUG) printf("4\n");
			}
		else if (corner_orientation == 1) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					left(face); downInverse(face); leftInverse(face); down(face);
					left(face); down(face); down(face); leftInverse(face);
					down(face); left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("B");
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); downInverse(face); leftInverse(face); down(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					downInverse(face); left(face); down(face); leftInverse(face);
					downInverse(face); left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("D");
					frontInverse(face); down(face); front(face); downInverse(face);
					frontInverse(face); down(face); front(face);
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("F");
					down(face);
					frontInverse(face); down(face); front(face); downInverse(face);
					frontInverse(face); down(face); front(face);
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					down(face);
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("H");
					down(face); down(face);
					frontInverse(face); down(face); front(face); downInverse(face);
					frontInverse(face); down(face); front(face);
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					down(face); down(face); left(face); down(face);
					leftInverse(face); downInverse(face); left(face); down(face);
					leftInverse(face);
					}
				else {
					if (DEBUG) printf("J");
					downInverse(face); frontInverse(face); down(face); front(face);
					downInverse(face); frontInverse(face); down(face); front(face);
					}
				}
			if (DEBUG) printf("5\n");
			}
		else if (corner_orientation == 2) {
			if (edge_position == 4) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("A");
					left(face); downInverse(face); leftInverse(face); downInverse(face);
					left(face); down(face); leftInverse(face); downInverse(face);
					left(face); down(face); down(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("B");
					left(face); downInverse(face); leftInverse(face); down(face);
					frontInverse(face); downInverse(face); front(face); downInverse(face);
					frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 0) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("C");
					downInverse(face);
					left(face); downInverse(face); leftInverse(face); down(face);
					left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("D");
					frontInverse(face); downInverse(face); front(face); down(face);
					frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 1) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("E");
					left(face); downInverse(face); leftInverse(face); down(face);
					left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("F");
					down(face);
					frontInverse(face); downInverse(face); front(face); down(face); 
					frontInverse(face); downInverse(face); front(face);
					}
				}
			else if (edge_position == 2) {
				if (edge_orientation == 0) {
					if (DEBUG) printf("G");
					down(face);
					left(face); downInverse(face); leftInverse(face); down(face);
					left(face); downInverse(face); leftInverse(face);
					}
				else {
					if (DEBUG) printf("H");
					down(face); down(face); frontInverse(face); downInverse(face);
					front(face); down(face); frontInverse(face); downInverse(face);
					front(face);
					}
				}
			else if (edge_position == 3) { //TODO: this can technically just be an if --- would only make it harder to read
				if (edge_orientation == 0) {
					if (DEBUG) printf("I");
					down(face); down(face); left(face); downInverse(face);
					leftInverse(face); down(face); left(face); downInverse(face);
					leftInverse(face);
					}
				else {
					if (DEBUG) printf("J");
					downInverse(); frontInverse(face); downInverse(face); front(face);
					down(face); frontInverse(face); downInverse(face); front(face);
					}
				}
			if (DEBUG) printf("6\n");
			}
		}
}

void setUpForF2L(Cubelet corner, Cubelet edge) {
	if (cP[0][0][0] == corner && corner != WOB) { //Move a corner in one of the locations of the other corners to the bottom layer and avoid putting the edge into one of the three locations for the other edges at the same time
		if(cP[1][2][2] == edge) { //TODO: do this part like the second part so that it is more prettier and consistenter...
			leftInverse(G); downInverse(G); left(G);
		}
		else {
			back(G); down(G); backInverse(G);
		}
	}
	if (cP[0][0][2] == corner && corner != WRB) {
		if(cP[1][2][0] == edge) {
			right(G); downInverse(G); rightInverse(G);
		}
		else {
			backInverse(G); downInverse(G); back(G);
		}
	}
	if (cP[2][0][0] == corner && corner != WOG) {
		if(cP[1][2][0] == edge) {
			left(G); down(G); leftInverse(G);
		}
		else {
			front(G); down(G); frontInverse(G);
		}
	}
	if (cP[2][0][2] == corner && corner != WRG) {
		if(cP[1][2][2] == edge) {
			rightInverse(G); down(G); right(G);
		}
		else {
			front(G); downInverse(G); frontInverse(G);
		}
	}

	if (cP[0][1][0] == edge && edge != OB) { //Move an edge in the wrong place into the bottom layer without putting the desired corner into a wrong place
		back(G);
		if(cP[2][2][0] == corner) { down(G); }
		else { downInverse(G); }
		backInverse(G);
	}
	if (cP[0][1][2] == edge && edge != RB) {
		backInverse(G);
		if(cP[2][2][2] == corner) { downInverse(G); }
		else { down(G); }
		back(G);
	}
	if (cP[2][1][0] == edge && edge != OG) {
		frontInverse(G);
		if(cP[0][2][0] == corner) { downInverse(G); }
		else { down(G); }
		front(G);
	}
	if (cP[2][1][2] == edge && edge != RG) {
		front(G);
		if(cP[0][2][2] == corner) { down(G); }
		else { downInverse(G); }
		frontInverse(G);
	}

	if (corner == WOB && cP[0][0][0] != WOB) {
		while (cP[0][2][0] != WOB) {
			down(G);
		}
	}
	if (corner == WRB && cP[0][0][2] != WRB) {
		while (cP[0][2][2] != WRB) {
			down(G);
		}
	}	if (corner == WOG && cP[2][0][0] != WOG) {
		while (cP[2][2][0] != WOG) {
			down(G);
		}
	}	if (corner == WRG && cP[2][0][2] != WRG) {
		while (cP[2][2][2] != WRG) {
			down(G);
		}
	}
}

/*
 * Moves a cube into its solved position as part of the white cross step
 * Assumes that the cube at location (i, j, k) is a valid cube to solve for white cross
 * Used in white cross
 *
 * int i, j, k : Assumes a valid cube on the correct face to solve for white cross
 */
void putEdgeInPosition(int i, int j, int k) {
	if (whiteCrossOrientationCheck(i, j, k)) { //If the cube is already oriented correctly, simply rotates it into its solved position
		if (i == 0) { back(G); back(G); }
		else if (i == 2) { front(G); front(G); }
		else {
			if (k == 0) { left(G); left(G); }
			else { right(G); right(G); }
		}
	}
	else { //Cube is not oriented correctly, fixes its orientation and puts in into its solved position
		if (i == 0) { down(G); left(G); backInverse(G); leftInverse(G); }
		else if (i == 2) { down(G); right(G); frontInverse(G); rightInverse(G); }
		else {
			if (k == 0) { down(G); front(G); leftInverse(G); frontInverse(G); }
			else { down(G); back(G); rightInverse(G); backInverse(G); }
		}
	}
}

/*
 * Checks whether the cube at (i, j, k) is oriented correctly or not
 * int i, j, k : Assumes the input is a valid edge cube on the bottom (yellow) face
 *
 * Used in white cross [more specifically putEdgeInPosition()]
 */
int whiteCrossOrientationCheck(int i, int j, int k) {
	// Unused Error Trapping
	//if (j != 2) {
	//	return -1;
	//}

	if (i == 0) return cF[9][4] == cF[10][4];
	else if (i == 2) return cF[4][4] == cF[5][4];
	else {
		if (k == 0) return cF[4][1] == cF[5][1];
		else return cF[4][7] == cF[5][7];
	}
}

/*
 * Rotates the bottom layer until 'column' matches the cube at location i, j, k
 *
 * column : cubelet face to check the cube at (i, j, k) against [Assumes valid values for all inputs]
 */
void moveEdgeToColumn(Cubelet column, int i, int j, int k) {
	Cubelet search = cP[i][j][k];
	while (column != cP[i][j-1][k]) {
		down(G);
		findCube(search, &i, &j, &k); //TODO: Inefficient
	}
}

/*
 * Moves an edge piece to the bottom (yellow) layer if not already in the bottom layer
 * Used in solving the white cross
 *
 * int i, j, k : location of cube to be moved to the bottom layer [Assumes that the location given is a valid edge]
 */
void moveEdgeToBottom(int i, int j, int k) { //Moves the cube at index (i, j, k) to the yellow face - This method assumes it is recieving a valid edge position on the white face
	if (i == 0) {
		if (j == 0) {
			back(G);
			back(G);
		}
		else if (j == 1) { //There is a possibility that the cube on the top layer is already in place, so this makes sure to not move that cube
			if (k == 0) {
				back(G);
				down(G);
				backInverse(G);
			}
			else {
				backInverse(G);
				down(G);
				back(G);
			}
		}
	}
	else if (i == 1) { //The cube must be the one on the top layer, so it is safe to simply move it to the bottom layer
		if (j == 0) { //This check ensure that the cube is not already on the bottom layer
			if (k == 0) {
				left(G);
				left(G);
			}
			else {
				right(G);
				right(G);
			}
		}
	}
	else {
		if (j == 0) {
			front(G);
			front(G);
		}
		else if(j == 1) { //There is a possibility that the cube on the top layer is already in place, so this makes sure to not move that cube
			if (k == 0) {
				frontInverse(G);
				down(G);
				front(G);
			}
			else {
				front(G);
				down(G);
				frontInverse(G);
			}
		}
	}
}

/*
 * Finds the cubelet 'search' in the cube representation (cP) and sets i, j, k to the location
 */
void findCube(Cubelet search, int* i, int* j, int* k) {
	for(*i=0; *i<3; (*i)++) {
		for(*j=0; *j<3; (*j)++) {
			for(*k=0; *k<3; (*k)++) {
				if(cP[*i][*j][*k] == search) return;
			}
		}
	}
}

/*
 * Prints a 2D visual representation of the cube's state
 * mode == 0 - print faces : mode == 1 - print cubes : mode == 2 - print both
 */
void printCube(int mode) {
	int i, j, k;
	char* enumStrings[27] = { //Strings to print (this array is parrallel with the Cubelet enum)
		"WRB", "WRG", "WOG", "WOB", "WR", "WO", "WB", "WG",
		"YRB", "YRG", "YOG", "YOB", "YR", "YO", "YB", "WG",
		"RB", "RG", "OG", "OB", "W", "Y", "O", "R", "G", "B", " " };

	if (mode == 0 || mode == 2) {
		printf("\n        ---------\n"); //Print Faces
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
	}
	
	if (mode == 1 || mode == 2) {
		for (i=0; i<3; i++) { //Print Cubes
			for (j=0; j<3; j++) {
				for (k=0; k<3; k++) {
					printf("%3s ", enumStrings[ cP[i][j][k] ]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
}

/*
 * Rotates the green face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 * 
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void front(Cubelet f) {
	if (f == R) { right(G); return; }
	if (f == B) { back(G); return; }
	if (f == O) { left(G); return; }	

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

/*
 * Rotates the blue face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 * 
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void back(Cubelet f) {
	if (f == R) { left(G); return; }
	if (f == B) { front(G); return; }
	if (f == O) { right(G); return; }	

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

/*
 * Rotates the yellow face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 */
void down(){
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

/*
 * Rotates the white face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 */
void up() {	
	strcat(solution, "U");
	solutionLen++;

	useUp = 1; //TODO: this is to tell if the top needs to be placed on top for the solution

	Cubelet tmp;
	tmp = cP[0][0][0]; cP[0][0][0] = cP[2][0][0]; cP[2][0][0] = cP[2][0][2]; cP[2][0][2] = cP[0][0][2]; cP[0][0][2] = tmp; //Up Position Corner Moves
	tmp = cP[0][0][1]; cP[0][0][1] = cP[1][0][0]; cP[1][0][0] = cP[2][0][1]; cP[2][0][1] = cP[1][0][2]; cP[1][0][2] = tmp; //Up Position Edge Moves
	tmp = cF[0][3]; cF[0][3] = cF[2][3]; cF[2][3] = cF[2][5]; cF[2][5] = cF[0][5];  cF[0][5] = tmp; //Up Face Corners
	tmp = cF[0][4]; cF[0][4] = cF[1][3]; cF[1][3] = cF[2][4]; cF[2][4] = cF[1][5];  cF[1][5] = tmp; //Up Face Edges
	tmp = cF[3][0]; cF[3][0] = cF[3][3]; cF[3][3] = cF[3][6]; cF[3][6] = cF[11][5]; cF[11][5] = tmp; //Up Edge Corners
	tmp = cF[3][1]; cF[3][1] = cF[3][4]; cF[3][4] = cF[3][7]; cF[3][7] = cF[11][4]; cF[11][4] = tmp; //Up Edge Edges
	tmp = cF[3][2]; cF[3][2] = cF[3][5]; cF[3][5] = cF[3][8]; cF[3][8] = cF[11][3]; cF[11][3] = tmp; //Up Edge Corners Part 2 TODO: Name this better (and the simmilar ones also)
}

/*
 * Rotates the red face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 * 
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void right(Cubelet f) {
	if (f == R) { back(G); return; }
	if (f == B) { left(G); return; }
	if (f == O) { front(G); return; }

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

/*
 * Rotates the orange face of the cube one quarter rotation clockwise
 * Appends notation for a clockwise face turn onto the solution string and increments the length
 * 
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void left(Cubelet f) {
	if (f == R) { front(G); return; }
	if (f == B) { right(G); return; }
	if (f == O) { back(G); return; }		

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

/*
 * Simulates a quarter turn of the green face counter-clockwise by calling three clockwise turns of the green face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void frontInverse(Cubelet f) {
	front(f); front(f); front(f);
}

/*
 * Simulates a quarter turn of the blue face counter-clockwise by calling three clockwise turns of the blue face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void backInverse(Cubelet f) {
	back(f); back(f); back(f);
}

/*
 * Simulates a quarter turn of the yellow face counter-clockwise by calling three clockwise turns of the yellow face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void downInverse() {
	down(); down(); down(); 
}

/*
 * Simulates a quarter turn of the white face counter-clockwise by calling three clockwise turns of the white face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void upInverse() {
	up(); up(); up();	
}

/*
 * Simulates a quarter turn of the red face counter-clockwise by calling three clockwise turns of the red face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void rightInverse(Cubelet f) {
	right(f); right(f); right(f);
}

/*
 * Simulates a quarter turn of the orange face counter-clockwise by calling three clockwise turns of the orange face
 *
 * f : if the relative front of the cube is not green, will call the corrosponding cube face
 */
void leftInverse(Cubelet f) {
	left(f); left(f); left(f);
}