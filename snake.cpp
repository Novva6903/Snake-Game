#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define W 60
#define H 20

// ============================= Setting & Hiding Cursor ===============================================
void setCursorPosition(int x, int y) {
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { x, y };
	SetConsoleCursorPosition(output, pos);
}

enum MenuSystem {
	SHOWMENU, MOVECURSOR, EXITPROGRAM
};

void hidecursor() {
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

// ============================= Initialize Map, Snake, etc =============================================

char map[H][W];
int score = 0;
int playerY;
int playerX;
int direction = -1; //1 = up, 2 = left, 3 = right, 4 = down
bool gameOver = false;
bool restart = false;

struct Snake {
	int Y;
	int X;
	char symbol;
	Snake *next, *prev;
}*head = NULL, *tail = NULL;

void initializeMap() {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (i == 0 && j == 0) map[i][j] = 218;
			else if (i == H-1 && j == 0) map[i][j] = 192;
			else if (i == 0 && j == W-1) map[i][j] = 191;
			else if (i == H-1 && j == W-1) map[i][j] = 217;
			else if (i == 0 || i == H-1) map[i][j] = 196;
			else if (j == 0 || j == W-1) map[i][j] = 179;
			else map[i][j] = ' ';
		}
	}
}

// ============================= Snake adding & removing function =======================================

Snake *newSnake(int Y, int X, char symbol) {
	Snake *curr = (Snake*)malloc(sizeof(Snake));

	curr->Y = Y;
	curr->X = X;
	curr->symbol = (char)219;
	curr->next = curr->prev = NULL;
	
	return curr;
}

void pushHead(int Y, int X, char symbol) {
	Snake *curr = newSnake(Y, X, symbol);
	if (!head) {
		head = tail = curr;
	}
	else {
		curr->next = head;
		head->prev = curr;
		head = curr;
	}

}

void pushTail(int Y, int X, char symbol) {
	Snake *curr = newSnake(Y, X, symbol);
	if (!head) head = tail = curr;
	else {
		tail->next = curr;
		curr->prev = tail;
		tail = curr;
	}
}

void popTail() {
	Snake *curr = tail;
	if (!head) return;
	if (head == tail) {
		setCursorPosition(head->X+30, head->Y+5);
		printf(" ");
		head = tail = NULL;
	}
	else {
		setCursorPosition(curr->X+30, curr->Y+5);
		printf(" ");
		tail = tail->prev;
		free(curr);
		curr = NULL;
		tail->next = NULL;
	}
}

// ============================= Snake & Map print Function =======================================

void printMap() {
	for (int spaceY = 0; spaceY < 5; spaceY++) {
		printf("\n");
	}
	for (int i = 0; i < H; i++) {
		for (int spaceX = 0; spaceX < 30; spaceX++) {
			printf(" ");
		}
		for (int j = 0; j < W; j++) {
			printf("%c", map[i][j]);
		}
		if (i == 3) printf("   Score: %d", score);
		else if (i == 4) printf("   Press R to restart");
		else if (i == 5) printf("   Press E to Exit");
		printf("\n");
	}
}

void printSnake() {
	if (!head) return;
	Snake *curr = head;
	while (curr) {
		setCursorPosition(curr->X+30, curr->Y+5);
		printf("%c", curr->symbol);
		if (playerX == curr->X && playerY == curr->Y && head != curr) {
			gameOver = true;
			break;
		}
		curr = curr->next;
	}
}

// ============================= Initialize Game =======================================

void initializeGame() {
	gameOver = false;
	score = 0;
	playerY = H / 2;
	playerX = W / 2;
	pushHead(playerY, playerX, 219);
	initializeMap();
	printMap();
	printSnake();
	setCursorPosition(45, 3);
	printf("Press (wasd) to start the game!");
	setCursorPosition(53, 26);
	printf("Created by @Novva");
}

// ============================= Moving Snake function =======================================

void moveSnake() {
	char move;
	if (kbhit()) {
		move = getch();
		switch (move) {
			case 'w':
				direction = 1;
				break;
			case 'a':
				direction = 2;
				break;
			case 'd':
				direction = 3;
				break;
			case 's':
				direction = 4;
				break;
			case 'r':
				restart = true;
				break;
			case 'e':
				exit(0);
				break;
		}
	}
}

// ============================= Generate Apple function =======================================

int appleX, appleY;
void generateApple() {
	bool valid = false;
	while (!valid) {
		appleX = (rand() % (W-1));
		appleY = (rand() % (H-1));
		
		if (appleX == 0) appleX++;
		if (appleY == 0) appleY++;
		
		Snake *curr = head;
		while (curr) {
			if (appleX == curr->X && appleY == curr->Y) break;
			
			if (curr->next == NULL) valid = true;
			curr = curr->next;
		}
	}
	
	setCursorPosition(appleX+30, appleY+5);
	printf("%c", (char)254);
}

// ============================= Main Game function =======================================

void game() {
	generateApple();
	while (!gameOver) {
		moveSnake();
		if (restart) return;
		popTail();
		switch (direction) {
			case 1:
				if (playerY != 1) playerY--;
				else playerY = H-2;
				break;
			case 2:
				if (playerX != 1) playerX--;
				else playerX = W-2;
				break;
			case 3:
				if (playerX != W-2) playerX++;
				else playerX = 1;
				break;
			case 4:
				if (playerY != H-2) playerY++;
				else playerY = 1;
				break;
		}
		if (appleX == playerX && appleY == playerY) {
			score++;
			setCursorPosition(W+40, 8);
			printf("%d", score);
			pushHead(playerY, playerX, 219);
			switch (direction) {
				case 1:
					if (playerY != 1) playerY--;
					else playerY = H-2;
					break;
				case 2:
					if (playerX != 1) playerX--;
					else playerX = W-2;
					break;
				case 3:
					if (playerX != W-2) playerX++;
					else playerX = 1;
					break;
				case 4:
					if (playerY != H-2) playerY++;
					else playerY = 1;
					break;
			}
			generateApple();
		}
		pushHead(playerY, playerX, 219);
		printSnake();
		if (direction == 2 || direction == 3) Sleep(60);
		else if (direction == 1 || direction == 4) Sleep(95);
	}
}

// ============================= Game Over function =======================================

void menuGameOver() {
	setCursorPosition(56, 3);
	printf("Game Over!");
	getchar();
	while (head) popTail();
	setCursorPosition(appleX+30, appleY+5);
	printf(" ");
}

// ============================= Main =======================================

int main() {
	srand(time(0));
	char startMove;
	hidecursor();
	
	while (true) {
		system("cls");
		startMove = ' ';
		initializeGame();
		while (startMove != 'w' && startMove != 'a' && startMove != 's' && startMove != 'd') {
			startMove = getch();
			switch (startMove) {
				case 'w':
					direction = 1;
					break;
				case 'a':
					direction = 2;
					break;
				case 'd':
					direction = 3;
					break;
				case 's':
					direction = 4;
					break;
				case 'e':
					exit(0);
					break;
			}
		}
		
		// Removing Texts
		setCursorPosition(45, 3);
		printf("                               ");
		setCursorPosition(53, 26);
		printf("                 ");
		
		game();
		
		if (restart) {
			restart = false;
			while (head) popTail();
			setCursorPosition(appleX+30, appleY+5);
			printf(" ");
			continue;
		}
		else menuGameOver();
	}
	
	return 0;
}
