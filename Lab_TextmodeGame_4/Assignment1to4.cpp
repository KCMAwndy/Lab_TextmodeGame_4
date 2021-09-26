#include<stdio.h>
#include <windows.h>
#include <time.h>
#define scount 80
#define screen_x 80
#define screen_y 25

HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
COORD star[scount];
//COORD ship;
COORD characterPos = { 0,0 };
COORD bufferSize = { screen_x,screen_y };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
CHAR_INFO consoleBuffer[screen_x * screen_y];

void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}
char cursor(int x, int y) {
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	char buf[2]; COORD c = { x,y }; DWORD num_read;
	if (!ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))
		return '\0';
	else
		return buf[0];
}
int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}
void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 10;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos, &windowSize);
}
/*
void fill_data_to_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 7;
		}
	}
}
*/
void init_star()
{
	for (int i = 0; i < scount; ++i) {
		star[i].X = rand() % screen_x+1;
		star[i].Y = rand() % screen_y+1;
	}
}
void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { (short)(rand() % screen_x),1 };
		}
		else {
			star[i] = { star[i].X,(short)(star[i].Y + 1) };
		}
	}
}
void fill_star_to_buffer()
{
	for (int i = 0; i < scount; ++i) {
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}
}
void earse_star_to_buffer(int i)
{
	consoleBuffer[star[i].X + screen_x * star[i].Y+1].Char.AsciiChar = ' ';
	consoleBuffer[star[i].X + screen_x * star[i].Y+1].Attributes = 7;
}
//void ship_position(short x, short y) {
//	ship = { x,y };
//}
void fill_ship_to_buffer(char ship[5],int x,int y,int color)
{
	for (int i = 0; i < 5; i++) {
		consoleBuffer[(x+i) + screen_x * y].Char.AsciiChar = ship[i];
		consoleBuffer[(x+i) + screen_x * y].Attributes = color;
	}
}

bool checkCollision(int *count,int x,int y) {
	for (int i = 0; i < scount; i++) {
		if (star[i].X >= x && star[i].X <= x+4 && star[i].Y == y-1) {
			*count=*count+1;
			earse_star_to_buffer(i);
			star[i].X = rand() % screen_x + 1; // correct
			//star[i].X = 10;
			star[i].Y = 1;
			//printf("\n%d", count);
			if (*count == 10) {
				return true;
			}
		}
	}
	return false;
}
int main()
{
	int i = 0, x = screen_x/2, y = screen_y, color = 7,count=0;
	char ship[5] = { '<','-','0','-','>'};
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	//setcursor(0);
	srand(time(NULL));
	setConsole(screen_x, screen_y);
	setMode();
	init_star();
	while (play)
	{
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {

				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					else if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						color = rand() % 256;
					}
				}

				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						x = posx;
						y = posy;
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						color = rand() % 256;
					}
				}
			}
			delete[] eventBuffer;
		}
		star_fall();
		clear_buffer();
		fill_star_to_buffer();
		fill_ship_to_buffer(ship, x, y, color);
		if (checkCollision(&count,x,y)) {
			play = false;
		}
		fill_buffer_to_console();
		Sleep(100);
	}
	return 0;
}
