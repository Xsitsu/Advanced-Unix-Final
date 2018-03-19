#include <ncurses.h>

#include "packet.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

#define WIDTH 60
#define HEIGHT 20

#define CKEY_ENTER 10
#define CKEY_PLUS 43
#define CKEY_MINUS 45

int startx;
int starty;

int mid_x;
int mid_y;

int num_x;
int num_y;

void draw_calc(WINDOW *win);
int send_math();

int main()
{
	WINDOW *win;
	int key;

	initscr();
	clear();
	noecho();
	cbreak();

	startx = (SCREEN_WIDTH - WIDTH) / 2;
	starty = (SCREEN_HEIGHT - HEIGHT) / 2;

	mid_x = SCREEN_WIDTH / 2;
	mid_y = SCREEN_HEIGHT / 2;


	win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(win, TRUE);
	curs_set(0);

	int x = 0;
	int y = 0;
	int op = 0;

	bool playing = true;
	while (playing)
	{
		draw_calc(win);
		wrefresh(win);
		key = wgetch(win);
		if (key >= 48 && key < 57)
		{
			
		}
		else if (key == CKEY_ENTER)
		{
			printf("KEY ENTER\n");
		}
		else if (key == CKEY_PLUS)
		{
			printf("KEY PLUS\n");
		}
		else if (key == CKEY_MINUS)
		{
			printf("KEY MINUS\n");
		}
	}

	return 0;
}

void draw_calc(WINDOW *win)
{
	int x = mid_x;
	int y = mid_y - 6;

	wmove(win, y, x - (37 / 2));
	wprintw(win, "Use the numpad to use the calculator!\n");
	wmove(win, y + 1, x - (28 / 2));
	wprintw(win, "Maximum 4 digits per number.\n");
}

int send_math()
{

}
