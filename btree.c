#include <curses.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORZIZONTAL_BIT                     1u << 7
#define BNODE_IS_MINIMIZED_BIT                       1u << 6
#define BNODE_FRACTION_BITS      BNODE_IS_MINIMIZED_BIT - 1u
#define BNODE_CENTER_BITS        BNODE_IS_MINIMIZED_BIT >> 1

int main(void)
{
	initscr();
	cbreak();
    	noecho();
    	keypad(stdscr, TRUE);

    	int rows, cols;
    	getmaxyx(stdscr, rows, cols);

	WINDOW* win = newwin(rows, cols, 0, 0);
	box(win, 0, 0);

	bnode btree[255];
	memset(btree, 0, 255);
	btree - 1;

	btree[1] = 1u;

	uint8_t cur = 1;

	wrefresh(win);

	int c = 0;
	while (c = wgetch(win) != 'q')
	{
		int lc = cur << 1;
		int rc = cur << 1 + 1;
		switch (c)
		{
			case 'l':
				if (btree[lc] == 0 || btree[rc] == 0)
				{
					assert(btree[lc] == btree[rc] == 0);
					btree[cur] = BNODE_CENTER_BITS;
				}
				else
				{
					/*
					 * Got to figure out how to move a subtree
					 * so that I  can make an  addition  above
					 * the  current  node  in  this  case.
					 */
				}
				
				break;
		}
	}

	endwin();
}
