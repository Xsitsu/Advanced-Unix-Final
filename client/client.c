#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <ncurses.h>

#include "packet.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

#define WIDTH 60
#define HEIGHT 20

#define CKEY_ENTER 10
#define CKEY_BACK 263
#define CKEY_PLUS 43
#define CKEY_MINUS 45
#define CKEY_UP 259
#define CKEY_DOWN 258

#define OPCHAR_PLUS "+"
#define OPCHAR_MINUS "-"

#define NETWORK_ERR -1333337

char serv_addr[] = "127.0.0.1";
int serv_port = 40078;

int startx;
int starty;

int mid_x;
int mid_y;

int num_x;
int num_y;
int op;

int offset_x;
int offset_y;

bool write_x;

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

	mid_x = (WIDTH / 2);
	mid_y = (HEIGHT / 2);


	win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(win, TRUE);
	curs_set(0);

	offset_x = 1;
	offset_y = 1;
	num_x = 0;
	num_y = 0;
	op = CKEY_PLUS;

	write_x = true;
	bool playing = true;
	while (playing)
	{
		draw_calc(win);
		wrefresh(win);
		key = wgetch(win);
		if (key >= 48 && key <= 57)
		{
			int* num;
			int* offset;
			if (write_x)
			{
				num = &num_x;
				offset = &offset_x;
			}
			else
			{
				num = &num_y;
				offset = &offset_y;
			}

			int n = *num;
			if (n == 0)
			{
				n = (key - 48);
				*num = n;
			}
			else if (n < 1000)
			{
				n = (n * 10) + (key - 48);
				*num = n;
				*offset = (*offset + 1);
			}
		}
		else if (key == CKEY_BACK)
		{
			int* num;
			int* offset;
			if (write_x)
			{
				num = &num_x;
				offset = &offset_x;
			}
			else
			{
				num = &num_y;
				offset = &offset_y;
			}

			int n = *num;
			if (n > 10)
			{
				n = (n / 10);
				*num = n;
				*offset = (*offset - 1);
			}
			else if (n > 0)
			{
				n = (n / 10);
				*num = n;
			}
		}
		else if (key == CKEY_ENTER)
		{
			int result = send_math();
			if (result != NETWORK_ERR)
			{
				wprintw(win, "Result: %d", result);
			}
			else
			{
				wprintw(win, "NETWORK ERROR");
			}
		}
		else if (key == CKEY_PLUS)
		{
			op = CKEY_PLUS;
		}
		else if (key == CKEY_MINUS)
		{
			op = CKEY_MINUS;
		}
		else if (key == CKEY_UP)
		{
			write_x = true;
		}
		else if (key == CKEY_DOWN)
		{
			write_x = false;
		}
		else
		{
			//printf("Key: %d\n", key);
		}
	}

	return 0;
}

void draw_calc(WINDOW *win)
{
	int x = mid_x;
	int y = mid_y - 6;

	wmove(win, y, x - (37 / 2));
	wprintw(win, "Use the numpad to use the calculator!");
	wmove(win, y + 1, x - (42 / 2));
	wprintw(win, "Arrow keys up down to select input number!");
	wmove(win, y + 2, x - (48 / 2));
	wprintw(win, "Enter to send results to server for computation!");
	wmove(win, y + 3, x - (28 / 2));
	wprintw(win, "Maximum 4 digits per number.");

	wmove(win, y + 5, x);
	for (int i = 4; i > offset_x; i--)
	{
		wprintw(win, " ");
	}
	wprintw(win, "%d", num_x);

	if (write_x)
	{
		wprintw(win, " <--");
	}
	else
	{
		wprintw(win, "    ");
	}

	wmove(win, y + 6, x - 2);
	if (op == CKEY_PLUS)
	{
		wprintw(win, OPCHAR_PLUS);
	}
	else if (op == CKEY_MINUS)
	{
		wprintw(win, OPCHAR_MINUS);
	}
	else
	{
		wprintw(win, "?");
	}

	wmove(win, y + 6, x);
	for (int i = 4; i > offset_y; i--)
	{
		wprintw(win, " ");
	}
	wprintw(win, "%d", num_y);

	if (!write_x)
	{
		wprintw(win, " <--");
	}
	else
	{
		wprintw(win, "    ");
	}
}

int send_math()
{
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serv_port);
	server_addr.sin_addr.s_addr = inet_addr(serv_addr);

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		return NETWORK_ERR;
	}

	request_t request;
	response_t response;

	request.x = htonl((uint32_t)num_x);
	request.y = htonl((uint32_t)num_y);
	if (op == CKEY_PLUS)
	{
		request.op = REQUEST_ADD;
	}
	else if (op == CKEY_MINUS)
	{
		request.op = REQUEST_SUB;
	}

	write(sock_fd, &request, sizeof(request_t));

	char buff[1024];
	memset(buff, 0, 1024);

	ssize_t read_size = recv(sock_fd, buff, 1024, 0);
	if (read_size == sizeof(response_t))
	{
		memcpy(&response, buff, sizeof(response_t));

		if (response.status == RESPONSE_VALID)
		{
			int result = ntohl((int)response.result);
			return result;
		}
		else
		{
			return NETWORK_ERR;
		}
	}
	else
	{
		return NETWORK_ERR;
	}
}
