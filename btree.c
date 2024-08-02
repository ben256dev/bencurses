#define _XOPEN_SOURCE 700
#include <curses.h>
#include <locale.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORIZONTAL_BIT                       1u << 7
#define BNODE_FRACTION_BITS      BNODE_IS_HORIZONTAL_BIT - 1u
#define BNODE_CENTER_BITS        BNODE_IS_HORIZONTAL_BIT >> 1

typedef uint8_t bwin_flags;
#define BNODE_IS_DIRTY_BIT 1u << 7
typedef struct bwin
{
	WINDOW* wptr;
	bwin_flags flags;
	uint8_t node_id;
} bwin;

int main(void)
{
	initscr();
	setlocale(LC_ALL, "");
    	noecho();
    	keypad(stdscr, TRUE);
	raw();

    	int rows, cols;
    	getmaxyx(stdscr, rows, cols);

	int bwins_count = 0;
	bwin bwins[64];
	for (int i = 0; i < 64; i++)
		bwins[i].flags = BNODE_IS_DIRTY_BIT;
	bwins[0].wptr = newwin(rows, cols, 0, 0);
	bwins[0].flags = 0;
	bwins[0].node_id = 1;
	box(bwins[0].wptr, 0, 0);
	bwins_count++;

	bnode btree_allocation[255];
	memset(btree_allocation, 0, 255);
	bnode* btree = &btree_allocation[0] - 1;

	btree[1] = 1u;

	uint8_t cur = 1;

	int c;
	while ((c = wgetch(bwins[0].wptr)) != 'q')
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
					 *
					 * I was missing parenthesis so the whole
					 * while loop was bugged.
					 */
				}
				break;
			default:
				//waddwstr(bwins[0].wptr, L" 몬 ");
				waddch(bwins[0].wptr, c);
				break;
		}
	}

	endwin();
}
/*
 * Remap space to be ctrl outside of insert mode
 * mallocs need free!
 */
