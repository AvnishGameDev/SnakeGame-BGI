// Made by Avnish Kirnalli.
#include <graphics.h>
#include <stdlib.h>
#include <math.h>

#pragma region GlobalDefinations

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 13
#define ESC 27

// Target Frames Per Second of the Game (Targeting 4 FPS for simplicity)
#define FPS 4

// Map stepping size in pixels
#define MAP_STEP 32

// Map X limit
#define MAP_X 20

// Map Y limit
#define MAP_Y 15

// Convert map location to screen location
#define COORD(x) x * MAP_STEP

// Starting score
#define STARTING_SCORE 0
#pragma endregion

// Get random int in Range from 0 to max
int randomInRange(int max)
{
	return (rand() % max);
}

#pragma region Vec2
// Vector of two numbers
typedef struct Vec2
{
	int x, y;
} Vec2;

int isEqual(Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}
#pragma endregion

/** Draw rectangle on screen
 * @param location location of the rectangle on the map
 * @param border color of the border
 * @param fill fill color of the rectangle
 */
void DrawRectangle(Vec2 location, int border, int fill)
{
	setcolor(border);
	setfillstyle(SOLID_FILL, fill);
	rectangle(COORD(location.x), COORD(location.y), COORD(location.x) + MAP_STEP, COORD(location.y) + MAP_STEP);
	floodfill(COORD(location.x)+16, COORD(location.y)+16, border);
}

#pragma region SnakeBodyPart
// Linked list node
typedef struct SnakeBodyPart
{
	// Border color of the body part
	int border;
	// Fill color of the body part
	int fill;

	// Current Location of the body part in map space
	Vec2 location;

	// Next Location to move to (set by the previous node) in map space
	Vec2 nextLocation;

	// Next node
	struct SnakeBodyPart* nextPart;
} SnakeBodyPart;

void SnakeBodyPart_Update(SnakeBodyPart* p)
{
	if (p->nextPart)
	{
		p->nextPart->nextLocation = p->location;
		SnakeBodyPart_Update(p->nextPart);
	}
	p->location = p->nextLocation;
}

void SnakeBodyPart_Draw(SnakeBodyPart* p)
{
	DrawRectangle(p->location, p->border, p->fill);
	if (p->nextPart)
		SnakeBodyPart_Draw(p->nextPart);
}

void SnakeBodyPart_Destroy(SnakeBodyPart* p)
{
	if (p->nextPart)
	{
		SnakeBodyPart_Destroy(p->nextPart);
		free(p->nextPart);
	}
}

#pragma endregion

#pragma region Snake
// Snake object
typedef struct Snake
{
	// Velocity of the snake in map space
	Vec2 velocity;

	// Linked list for SnakeBodyParts
	SnakeBodyPart* head;
	SnakeBodyPart* tail;

	// No of body parts, also the score
	int noOfParts;

	// int counter for number of parts that are to be added in update()
	int partsToAdd;

	// Previous movement direction
	int prevDir;
} Snake;

/** Set the direction of snake movement
 * @param dir new direction to set to \n
 * 1 - Left \n
 * 2 - Down \n
 * 3 - Right \n
 * 4 - Up \n
 * Note: direction can't be set to opposite direction of current direction
 */
void Snake_SetDirection(Snake* s, int dir)
{
	// If new direction opposite to current direction, return.
	if (abs(s->prevDir - dir) == 2)
		return;
		
	switch (dir)
	{
	case 1: // Left
		s->velocity.x = -1;
		s->velocity.y = 0;
		break;
	case 2: // Down
		s->velocity.x = 0;
		s->velocity.y = 1;
		break;
	case 3: // Right
		s->velocity.x = 1;
		s->velocity.y = 0;
		break;
	case 4: // Up
		s->velocity.x = 0;
		s->velocity.y = -1;
		break;
	default: // ERROR
		printf("ERROR: setDirection called with dir %d", dir);
		break;
	}

	// Set Previous Direction to New Direction
	s->prevDir = dir;
}

void Snake_Initialize(Snake* s)
{
	s->noOfParts = 0;
	s->partsToAdd = 0;
	s->prevDir = -4;
	
	Snake_SetDirection(s, 3);
	
	s->head = (SnakeBodyPart*)calloc(1, sizeof(SnakeBodyPart));
	s->head->border = YELLOW;
	s->head->fill = GREEN;
	s->head->location.x = -1;
	s->head->location.y = -1;
	s->tail = s->head;
}

void Snake_Update(Snake* s)
{
	// Adding parts
	if (s->partsToAdd > 0)
	{
		SnakeBodyPart* part = (SnakeBodyPart*)calloc(1, sizeof(SnakeBodyPart));
		part->border = GREEN;
		part->fill = YELLOW;
		s->tail->nextPart = part;
		s->tail = part;
		s->noOfParts++;
		s->partsToAdd--;
	}

	// Update body parts starting with head
	SnakeBodyPart_Update(s->head);
		
	// Update head next location
	s->head->nextLocation.x += s->velocity.x;
	s->head->nextLocation.y += s->velocity.y;
}

// Add body part
void Snake_AddPart(Snake* s)
{
	s->partsToAdd++;
}

void Snake_Draw(Snake* s)
{
	SnakeBodyPart_Draw(s->head);
}

// Destroy all body parts
void Snake_Destroy(Snake* s)
{
	SnakeBodyPart_Destroy(s->head);
	free(s->head);
}
#pragma endregion

#pragma region Apple
// Apple object
typedef struct Apple
{
	// Color of the Apple
	int color;
	
	// Current location of the Apple in map space
	Vec2 location;
} Apple;

void Apple_Draw(Apple* a)
{
	setcolor(a->color);
	setfillstyle(SOLID_FILL, a->color);
	circle(COORD(a->location.x) + 16, COORD(a->location.y) + 16, 16);
	floodfill(COORD(a->location.x) + 16, COORD(a->location.y) + 16, a->color);
}

void Apple_RandomizeLocation(Apple* a)
{
	a->location.x = randomInRange(MAP_X);
	a->location.y = randomInRange(MAP_Y);
}

#pragma endregion

int MainMenu()
{
	int ch;
	
	while (kbhit()) 
		getch(); // Read and discard each character in the input buffer
	ch = 0;
	while (1)
	{
		cleardevice();
		if (kbhit())
		{
			int keyCode = getch();
			if (keyCode == ENTER)
				break;
			
			if (keyCode == DOWN)
				ch++;
			else if (keyCode == UP)
				ch--;

			if (ch > 2)
				ch = 0;
			else if(ch < 0)
				ch = 2;
		}

		settextjustify(LEFT_TEXT, TOP_TEXT);
		settextstyle(BOLD_FONT, 0, 6);
		setcolor(YELLOW);
		outtextxy(100, 100, "Start");
		setcolor(BLUE);
		outtextxy(100, 200, "About");
		setcolor(RED);
		outtextxy(100, 300, "Exit");

		setcolor(GREEN);
		switch (ch)
		{
		case 0:
			rectangle(100, 100, 260, 150);
			break;
		case 1:
			rectangle(100, 200, 250, 250);
			break;
		case 2:
			rectangle(100, 300, 220, 350);
			break;
		}
		setcolor(WHITE);
		delay(60);
	}

	return ch;
}

void AboutMenu()
{
	while (kbhit()) 
		getch(); // Read and discard each character in the input buffer

	settextjustify(CENTER_TEXT, CENTER_TEXT);
	settextstyle(BOLD_FONT, 0, 4);
	setcolor(YELLOW);
	outtextxy(320, 40, "Microproject SYCO");
	settextstyle(BOLD_FONT, 0, 2);
	settextjustify(LEFT_TEXT, CENTER_TEXT);
	setcolor(GREEN);
	outtextxy(10, 200, "Made by");
	outtextxy(460, 200, "RollNo");
	setcolor(LIGHTBLUE);
	outtextxy(10, 240, "1. Avnish Kirnalli");
	outtextxy(460, 240, "92");
	setcolor(GREEN);

	setcolor(WHITE);
	while (!kbhit());
}

int main()
{
	int MenuCh;
	Snake snake;
	Apple apple;

	// GameOver flag, 0 = running, 1 = gameover
	int gameover = 0;
	
	int i;
	char c;

	SnakeBodyPart* p;

	char pointsStr[32] = "Points: ";
	char gameOverTxt[] = "Game Over!";
	char scoreTxt[32] = "Your Final score: ";
	char exitTxt[] = "Press any key to return to Main Menu";
	char creditTxt[] = "Made by Avnish Kirnalli.";

	int score;
	
	// Create window
	initwindow(640, 480, "Snake Game");

	MainMenu:
	MenuCh = MainMenu();
	if (MenuCh == 1)
	{
		AboutMenu();
		goto MainMenu;
	}
	else if(MenuCh == 2)
		return 0;
	
	/** Drawing **/

	// Initialize Snake and Apple
	Snake_Initialize(&snake);
	apple.color = LIGHTRED;
	Apple_RandomizeLocation(&apple);

	// Add starting score (if any)
	for (i = 0; i < STARTING_SCORE; i ++)
		Snake_AddPart(&snake);

	// Main application loop
	while (!gameover)
	{
		// Clear window
		cleardevice();
		
		/** INPUT **/
		
		// Movement Input
		if (kbhit())
		{
			getch();
			c = getch();

			// Movement Input
			if (c == UP)
				Snake_SetDirection(&snake, 4);
			else if (c == DOWN)
				Snake_SetDirection(&snake, 2);
			else if (c == LEFT)
				Snake_SetDirection(&snake, 1);
			else if (c == RIGHT)
				Snake_SetDirection(&snake, 3);
      
			// Exit Input
			if (c == ESC /* Escape */)
				break;
		}

		/** UPDATE **/

		Snake_Update(&snake);

		// Check If Snake's colliding with Apple
		if (isEqual(snake.head->location, apple.location))
		{
			// On player score increase
			Snake_AddPart(&snake); // Add part
			Apple_RandomizeLocation(&apple); // Set Apple to other Random location
		}

		// Check if Snake out of Map bounds and bring back from opposite side
		if (snake.head->nextLocation.x > MAP_X)
			snake.head->nextLocation.x = 0;
		if (snake.head->nextLocation.y > MAP_Y)
			snake.head->nextLocation.y = 0;
		if (snake.head->nextLocation.x < 0)
			snake.head->nextLocation.x = MAP_X;
		if (snake.head->nextLocation.y < 0)
			snake.head->nextLocation.y = MAP_Y;

		// Check if Snake head colliding with any body part
		p = snake.head->nextPart;
		while (p)
		{
			if (isEqual(snake.head->location, p->location))
			{
				// Game Over
				gameover = 1;
				break;
			}
			p = p->nextPart;
		}

		/** DRAW **/
		Snake_Draw(&snake);
		Apple_Draw(&apple);

		// Points text
		setcolor(LIGHTBLUE);
		settextstyle(BOLD_FONT, 0, 12);
		itoa(snake.noOfParts, pointsStr+8, 10);
		//snprintf(pointsStr, 32, "Points: %d", snake.noOfParts);
		outtext(pointsStr);

		// Targeting 2 FPS for simplicity
		delay(1000/FPS);
	}

	// Store final score to display
	score = snake.noOfParts;

	// Destroy snake
	Snake_Destroy(&snake);

	// Clear window
	cleardevice();

	// If exited on demand, exit directly.
	if (!gameover)
		return 1;
		
	// Game Over text
	setcolor(YELLOW);
	settextstyle(BOLD_FONT, 0, 8);
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	outtextxy(MAP_STEP*MAP_X/2, MAP_STEP*MAP_Y/2, gameOverTxt);

	// Score text
	itoa(score, scoreTxt+18, 10);
	//snprintf(scoreTxt, 32, "Your Final score: %d", score);
	settextstyle(BOLD_FONT, 0, 2);
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	outtextxy(MAP_STEP*MAP_X/2, MAP_STEP*MAP_Y/2+MAP_STEP, scoreTxt);

	// Press any key to exit text
	setcolor(GREEN);
	settextstyle(BOLD_FONT, 0, 2);
	settextjustify(CENTER_TEXT, BOTTOM_TEXT);
	outtextxy(MAP_STEP*MAP_X/2, MAP_STEP*MAP_Y-MAP_STEP, exitTxt);

	// Credits text
	setcolor(LIGHTBLUE);
	settextstyle(BOLD_FONT, 0, 2);
	settextjustify(RIGHT_TEXT, BOTTOM_TEXT);
	outtextxy(MAP_STEP*MAP_X, MAP_STEP, creditTxt);

	while (kbhit()) 
	{
		getch(); // Read and discard each character in the input buffer
	}

	while (!kbhit()); // Wait till user presses a key

	goto MainMenu;
}