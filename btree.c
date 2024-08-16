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

		n <<= 1;
		if (child_n & 128)
		{
			n++;

			if (tree->node[n] & BNODE_IS_HORIZONTAL_BIT)
				reg.r = prev_reg.r - reg.r;
			else
				reg.c = prev_reg.c - reg.c;
		}
		child_n <<= 1;
	}
}

int main(void)
{
	initscr();
	cbreak();
	setlocale(LC_ALL, "");
    	noecho();
    	keypad(stdscr, TRUE);

    	getmaxyx(stdscr, rows, cols);
	region scr_reg = { rows, cols };

	wtree tree;

	uint8_t cur;
	int cur_pos_y = 0;
	int cur_pos_x = 0;
	int cur_rows = rows;
	int cur_cols = cols;
	int old_cols = cur_cols;
	int old_rows = cur_rows;
	tree.node[1] = BNODE_CENTER_BITS/2;
	int gcr = get_child_region(&tree, scr_reg, 2).c;
	cur_cols = cur_cols * (tree.node[1] & BNODE_FRACTION_BITS) / BNODE_FRACTION_BITS;
	tree.node[2] = BNODE_WINDOW;
	tree.flag[2] = BNODE_IS_DIRTY_BIT;
	tree.nwin[2] = newwin(cur_rows, cur_cols, cur_pos_y, cur_pos_x);
	box(tree.nwin[2], 0, 0);
	cur_pos_x = cur_cols;
	cur_cols = old_cols - cur_pos_x;
	tree.node[3] = BNODE_WINDOW;
	tree.flag[3] = BNODE_IS_DIRTY_BIT;
	tree.nwin[3] = newwin(cur_rows, cur_cols, cur_pos_y, cur_pos_x);
	box(tree.nwin[3], 0, 0);

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
	} while ( ( c = wgetch(tree.nwin[2]) ) != 'q');

	endwin();
	return 0;
}
