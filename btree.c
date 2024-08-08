#define _XOPEN_SOURCE 700
#include <curses.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORIZONTAL_BIT                        1 << 7
#define BNODE_WINDOW                                        1
#define BNODE_FRACTION_BITS      BNODE_IS_HORIZONTAL_BIT  - 1
#define BNODE_CENTER_BITS        BNODE_IS_HORIZONTAL_BIT >> 1

typedef uint8_t bwin;
#define BNODE_IS_DIRTY_BIT 1 << 7
#define BNODE_IS_MINIM_BIT 1 << 6

#define LC(x)         x << 1
#define RC(x)        (x << 1) + 1
#define IS_SISTER(x)  x & 1
#define SISTER(x)     x + 1
#define PARENT(x)     x >> 1
#define AUNT(x)      (x >> 1) + 1

int main(void)
{
	initscr();
	cbreak();
	setlocale(LC_ALL, "");
    	noecho();
    	keypad(stdscr, TRUE);

    	int rows, cols;
    	getmaxyx(stdscr, rows, cols);

	void* bencurses_alloc_ptr = calloc( 255, (sizeof(WINDOW*) + sizeof(bnode) + sizeof(bwin)) );
	bnode*   node_array = (bnode*)    bencurses_alloc_ptr;
	bwin*    flag_array = (bwin*)    (node_array + 255 * sizeof(bnode));
	WINDOW** nwin_array = (WINDOW**) (flag_array + 255 * sizeof(bwin));

	uint8_t cur = 1;
	node_array[cur] = BNODE_WINDOW;
	flag_array[cur] = BNODE_IS_DIRTY_BIT;
	nwin_array[cur] = newwin(rows, cols, 0, 0);

	int c;
	while ( ( c = wgetch(nwin_array[1]) ) != 'q')
	{
		uint8_t n = 1;
TRAVERSE_TREE:
		if (n | 128 || node_array[n] == BNODE_WINDOW)
		{
			if (IS_SISTER(n))
			{
				switch (n) 
				{
					case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
						goto FINISH_TREE;
						break;
					default:
						n = AUNT(n);
				}
			}
			n = SISTER(n);
			goto TRAVERSE_TREE;
		}
FINISH_TREE:
		switch (c)
		{
			case 'l':
		}
	}

	endwin();
	free(bencurses_alloc_ptr);
	return 0;
}
