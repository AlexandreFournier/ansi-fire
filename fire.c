#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 4096

#define LINES 24
#define COLUMNS 80

#define ESC "\x1B"
#define BEL "\x07"
#define SS2 ESC "N"
#define OSC ESC "]"
#define CSI ESC "["

static int color(int value)
{
	int palette[] = { 16, 52, 88, 124, 160, 196, 202, 208, 214, 220, 15};
	//int palette[] = { 16, 22, 28,  34,  40,  46,  82, 118, 154, 190, 15};

	if (value >= 254)
		value = 254;

	value /= (255 / (sizeof(palette) / sizeof(int) - 1));
	return palette[value];
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	printf(OSC "0;FIRE" BEL, argc);
	printf(CSI "2J");
	printf(CSI "?25l");

	unsigned int fire[LINES][COLUMNS];
	memset(fire, 0, sizeof(fire));

	int i, j;
	int frame = 0;
	for (;;)
	{
		int lbeg = rand() % COLUMNS;
		int lend = lbeg + (rand() % (COLUMNS - lbeg));
		if (frame % (rand() % 50 + 1) == 0) {
			for (j = 0; j < LINES / 2; j++) {
				for (i = lbeg; i < lend; i++) {
					if (i < 4 || i > COLUMNS - 4)
						fire[j][i] = 0;
					else
						fire[j][i] = 0.25 * (rand() % 4) * 1024;
				}
			}
		}

		int oldcolor = 0;
		int newcolor;
		for (j = 1; j < LINES; j++) {
			printf(CSI "%d;0H", LINES - j);
			for (i = 0; i < COLUMNS; i++) {
				fire[j][i] += fire[j-1][i-1];
				fire[j][i] += fire[j-1][i];
				fire[j][i] += fire[j-1][i+1];
				fire[j][i] /= 4.2 + (rand() % 4) / (1.0 * j);

				newcolor = color(fire[j][i]);
				if (newcolor != oldcolor)
					printf(CSI "48;5;%dm ", newcolor);
				else
					printf(" ");
				oldcolor = newcolor;
			}
		}

		fflush(stdout);
		usleep(100000);
		frame++;
	}

	return 0;
}
