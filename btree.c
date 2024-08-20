#define _XOPEN_SOURCE 700
#include <curses.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

typedef uint8_t bnode;
#define BNODE_IS_HORIZONTAL_BIT 128
#define BNODE_VISITATION_BIT     64
#define BNODE_WINDOW              1
#define BNODE_FRACTION_BITS      63
#define BNODE_CENTER_BITS        31

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

typedef struct wtree
{
	bnode   node[255];
	bwin    flag[255];
	WINDOW* nwin[255];
} wtree;

typedef struct region
{
	int r;
	int c;
	int y;
	int x;
} region;

region get_child_region(const wtree* tree, region reg, uint8_t child_n)
{
	if (child_n < 2)
		return reg;

	int bin_stack_len = 7;
	for (;(child_n & 128) == 0; bin_stack_len--) 
		child_n <<= 1;
	child_n <<= 1;

	uint8_t n = 1;
	region prev_reg;
	for (int i = 0; i < bin_stack_len; i++)
	{
		if (tree->node[n] == BNODE_WINDOW)
			return prev_reg;

		prev_reg = reg;
		if (tree->node[n] & BNODE_IS_HORIZONTAL_BIT)
			reg.r = reg.r * (tree->node[n] & BNODE_FRACTION_BITS) / BNODE_FRACTION_BITS;
		else
			reg.c = reg.c * (tree->node[n] & BNODE_FRACTION_BITS) / BNODE_FRACTION_BITS;

		if (child_n & 128)
		{
			if (tree->node[n] & BNODE_IS_HORIZONTAL_BIT)
			{
				reg.y = reg.r;
				reg.r = prev_reg.r - reg.r;
			}
			else
			{
				reg.x = reg.c;
				reg.c = prev_reg.c - reg.c;
			}
			n <<= 1;
			n++;
		}
		else
			n <<= 1;
		child_n <<= 1;
	}

	return reg;
}

int main(void)
{
	initscr();
	cbreak();
	setlocale(LC_ALL, "");
    	noecho();
    	keypad(stdscr, TRUE);

    	getmaxyx(stdscr, rows, cols);
	region scr_reg = { rows, cols, 0, 0 };

	wtree tree;

	uint8_t cur;
	region child_region;
	tree.node[1] = BNODE_CENTER_BITS;

	tree.node[2] = BNODE_CENTER_BITS | BNODE_IS_HORIZONTAL_BIT;

	tree.node[3] = BNODE_CENTER_BITS | BNODE_IS_HORIZONTAL_BIT;

	tree.node[4] = BNODE_WINDOW;
	tree.flag[4] = BNODE_IS_DIRTY_BIT;

	tree.node[5] = BNODE_WINDOW;
	tree.flag[5] = BNODE_IS_DIRTY_BIT;

	tree.node[6] = BNODE_WINDOW;
	tree.flag[6] = BNODE_IS_DIRTY_BIT;

	tree.node[7] = BNODE_WINDOW;
	tree.flag[7] = BNODE_IS_DIRTY_BIT;

	child_region = get_child_region(&tree, scr_reg, 3);
	tree.nwin[3] = newwin(child_region.r, child_region.c, child_region.y, child_region.x);
	box(tree.nwin[3], 0, 0);

	child_region = get_child_region(&tree, scr_reg, 4);
	tree.nwin[4] = newwin(child_region.r, child_region.c, child_region.y, child_region.x);
	box(tree.nwin[4], 0, 0);

	child_region = get_child_region(&tree, scr_reg, 5);
	tree.nwin[5] = newwin(child_region.r, child_region.c, child_region.y, child_region.x);
	box(tree.nwin[5], 0, 0);

	child_region = get_child_region(&tree, scr_reg, 6);
	tree.nwin[6] = newwin(child_region.r, child_region.c, child_region.y, child_region.x);
	box(tree.nwin[6], 0, 0);

	child_region = get_child_region(&tree, scr_reg, 7);
	tree.nwin[7] = newwin(child_region.r, child_region.c, child_region.y, child_region.x);
	box(tree.nwin[7], 0, 0);

	int c;
	do
	{
		if (tree.node[1] == BNODE_WINDOW && tree.flag[1] | BNODE_IS_DIRTY_BIT)
		{
			wrefresh(tree.nwin[1]);
			goto FINISH_TREE;
		}

		uint8_t n = 2;
TRAVERSE_TREE:
		if (n == 1 && tree.node[n] & BNODE_VISITATION_BIT)
		{
			tree.node[n] ^= BNODE_VISITATION_BIT;
			goto FINISH_TREE;
		}
		
		if (tree.node[n] == BNODE_WINDOW)
		{
			if (tree.flag[n] | BNODE_IS_DIRTY_BIT)
			{
				wrefresh(tree.nwin[n]);
				tree.flag[n] ^ BNODE_IS_DIRTY_BIT;
			}

			tree.node[n] |= BNODE_VISITATION_BIT;
		}

		if (IS_SISTER(n))
		{
			if (tree.node[n] & BNODE_VISITATION_BIT && tree.node[n-1] & BNODE_VISITATION_BIT)
			{
				tree.node[n]         ^= BNODE_VISITATION_BIT;
				tree.node[n-1]       ^= BNODE_VISITATION_BIT;
				tree.node[PARENT(n)] |= BNODE_VISITATION_BIT;
				n = PARENT(n);
			}
			else
				n = CHILD(n);
		}
		else
		{
			if (tree.node[n] & BNODE_VISITATION_BIT)
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
	} while ( ( c = wgetch(tree.nwin[4]) ) != 'q');

	endwin();
	return 0;
}
