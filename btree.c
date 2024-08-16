#define _XOPEN_SOURCE 700
#include <curses.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORIZONTAL_BIT                   128
#define BNODE_VISITATION_BIT                      64
#define BNODE_WINDOW                                   1
#define BNODE_FRACTION_BITS  63
#define BNODE_CENTER_BITS    31

typedef uint8_t bwin;
#define BNODE_IS_DIRTY_BIT 1 << 7
#define BNODE_IS_MINIM_BIT 1 << 6

#define LC(x)         x << 1
#define RC(x)        (x << 1) + 1
#define IS_SISTER(x)  x &  1
#define SIBLING(x)    x ^  1
#define PARENT(x)     x >> 1
#define CHILD(x)      x << 1

int rows, cols;

int main(void)
{
	initscr();
	cbreak();
	setlocale(LC_ALL, "");
    	noecho();
    	keypad(stdscr, TRUE);

    	getmaxyx(stdscr, rows, cols);

	void* bencurses_alloc_ptr = calloc( 255, (sizeof(WINDOW*) + sizeof(bnode) + sizeof(bwin)) );
	bnode*   node_array = (bnode*)    bencurses_alloc_ptr;
	bwin*    flag_array = (bwin*)    (node_array + 255 * sizeof(bnode));
	WINDOW** nwin_array = (WINDOW**) (flag_array + 255 * sizeof(bwin));

	uint8_t cur;
	int cur_pos_y = 0;
	int cur_pos_x = 0;
	int cur_rows = rows;
	int cur_cols = cols;
	int old_cols = cur_cols;
	int old_rows = cur_rows;
	node_array[1] = BNODE_CENTER_BITS;
	cur_cols = cur_cols * BNODE_CENTER_BITS / BNODE_FRACTION_BITS;
	node_array[2] = BNODE_WINDOW;
	flag_array[2] = BNODE_IS_DIRTY_BIT;
	nwin_array[2] = newwin(cur_rows, cur_cols, cur_pos_y, cur_pos_x);
	box(nwin_array[2], 0, 0);
	cur_pos_x = cur_cols;
	cur_cols = old_cols - cur_pos_x;
	node_array[3] = BNODE_WINDOW;
	flag_array[3] = BNODE_IS_DIRTY_BIT;
	nwin_array[3] = newwin(cur_rows, cur_cols, cur_pos_y, cur_pos_x);
	box(nwin_array[3], 0, 0);

	int c;
	do
	{
		if (node_array[1] == BNODE_WINDOW && flag_array[1] | BNODE_IS_DIRTY_BIT)
		{
			wrefresh(nwin_array[1]);
			goto FINISH_TREE;
		}

		uint8_t n = 2;
TRAVERSE_TREE:
		if (n == 1 && node_array[n] & BNODE_VISITATION_BIT)
		{
			node_array[n] ^= BNODE_VISITATION_BIT;
			goto FINISH_TREE;
		}
		
		if (node_array[n] == BNODE_WINDOW)
		{
			if (flag_array[n] | BNODE_IS_DIRTY_BIT)
			{
				wrefresh(nwin_array[n]);
				flag_array[n] ^ BNODE_IS_DIRTY_BIT;
			}

			node_array[n] |= BNODE_VISITATION_BIT;
		}

		if (IS_SISTER(n))
		{
			if (node_array[n] & BNODE_VISITATION_BIT && node_array[n-1] & BNODE_VISITATION_BIT)
			{
				node_array[n]         ^= BNODE_VISITATION_BIT;
				node_array[n-1]       ^= BNODE_VISITATION_BIT;
				node_array[PARENT(n)] |= BNODE_VISITATION_BIT;
				n = PARENT(n);
			}
		}
		else
		{
			if (node_array[n] & BNODE_VISITATION_BIT)
				n = SIBLING(n);
			else
				n = CHILD(n);

		}
		goto TRAVERSE_TREE;
FINISH_TREE:
		switch (c)
		{
			case 'l':
		}
	} while ( ( c = wgetch(nwin_array[2]) ) != 'q');

	endwin();
	free(bencurses_alloc_ptr);
	return 0;
}
