#include <curses.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORZIZONTAL_BIT                     1u << 7
#define BNODE_FRACTION_BITS      BNODE_IS_HORIZONTAL_BIT - 1u
#define BNODE_CENTER_BITS        BNODE_IS_HORIZONTAL_BIT >> 1

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
					//disallow adding to the final layer.

					btree[lc] = btree[cur];
					btree[cur] = BNODE_CENTER_BITS;
					btree[rc] = 1u;
					cur = lc;
				}
				else
				{
					/*
					 * Got to figure out how to move a subtree
					 * so that I  can make an  addition  above
					 * the  current  node  in  this  case.
					 *
					 * This requires taking the child tree of
					 * the node we  want to  insert above and
					 * moving every layer down one  layer. If
					 * the  child  tree  reaches  to the  8th
					 * layer   then   the  user   should   be
					 * prevented from  adding above the node.
					 *
					 * Until I write the code to traverse the
					 * child tree, I can' t really write this
					 * else  block.
					 *
					 * I don't think window properties should
					 * be stored in nodes so I will have to
					 * allocate separate data for the state
					 * of windows such as whether they are
					 * minimized or their contents. This is
					 * because nodes are not necessarily
					 * windows.
					 *
					 * There may be a possibility to optimize
					 * the memory usage in the future by
					 * storing only subtrees but for now lets
					 * make the leaf a fixed array of size 64
					 * and the node tree of size 255
					 */
				}
				
				break;
		}
	}

	endwin();
}
