#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

/*
          ╭───╮      
    Row   │ R │      🡨   🡫   🡩   🡪
          ╰┬──┴╮   ╭───┬───┬───┬───╮
   Free    │ F.│   │ H │.J │ K │ L │
         ╭─┴─┬─┴─╮ ╰───┴───┴───┴───╯
 Column  │ C │ V │  View
         ╰───┴───╯
┌────────────────── XW ┌─────┐
│ A moose crossed the ││     │
│ road and he was the NQ     │
│ biggest I ever saw. ││     │
└─────────────────────┘ MW ──┘
┌──┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬─────┐
│`~│!1 │@2 │#3 │$4 │%5 │ 6^│ 7&│ 8*│ 9(│ 0)│ -_│ +=│ Bksp│
├──┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬───┤
│Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ {[│ }]│ |\│
├────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴───┤
│Caps │ A │ S │ D │ F.│ G │ H │.J │ K │ L │ :;│ "'│ Enter│
├─────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴──────┤
│Shift  │ Z │ X │ C │ V │ B │ N │ M │ <,│ >.│ ?/│  Shift │
├────┬──┴┬──┴──┬┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴──┬─────┤
│Ctrl│Mod│ Alt │                       │ Alt │     │ Ctrl│
└────┴───┴─────┴───────────────────────┴─────┴─────┴─────┘
     │
     I
    ╱ ╲
   1   2
  ╱│   │╲
┌────────────────────────────────────────────────────────┐
│             │              │              │            │
│             │       f      │              │      k     │
│             │              │              │            │
│      a      │              │      i       │            │
│             │───────H──────│              │──────L─────│
│             │              │              │            │
│             │              │              │            │
│──────C──────│       g      │───────J──────│      l     │
│      │  c   B              A              I            │
│  b   │──F───│              │              │            │
│      E      │──────G───────│              │──────K─────│
│      │  d   │              │              │   m  │   o │
│──────D──────│              │      j       │      │     │
│             │      h       │              │──N───│──O──│
│      e      │              │              │   n  M   p │
│             │              │              │      │     │
└────────────────────────────────────────────────────────┘
           A
       ╱       ╲    
      B         I
   ╱    ╲     ╱   ╲      
  C      G   J     K
 ╱ ╲    ╱ ╲ ╱ ╲
a   E    H h i j L     M
   b F  f g     k l N   O
    c d            m n o p
    
  1   1   1   2   2   3   3   4   4   5   5   6   6   7   7   8   8   9   9   10  10  11  11  12  12  13  13  14  14

  1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29 
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ A │ B │ I │ C │ G │ J │ K │ a │ E │ H │ h │ i │ j │ L │ M │ b │ F │ f │ g │ k │ l │ N │ O │ c │ d │ m │ n │ o │ p │
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
  1   2   2   4   4   4   4   4   5   5   5   6   7   8   8   3   4   5   6   6   7   8   8   0   1   2   3   4   5  

  Theres a big problem with how I've approached this issue which is that a non-complete binary search tree doesn't
  have the same indexing characteristics as complete trees where getting parent's and stuff like that is simple.
*/

// @key typedef
typedef uint64_t ben_key;
#define NCURSES_KEY_ALT 27

typedef uint8_t ben_win_type;
#define BEN_WIN_TYPE_NULL  0
#define BEN_WIN_TYPE_LEAF  1
#define BEN_WIN_TYPE_LEFT  2
#define BEN_WIN_TYPE_RIGHT 3
#define BEN_WIN_TYPE_VERT  4
#define BEN_WIN_TYPE_HORZ  5

// @ben win
typedef struct ben_win
{
	WINDOW* window;
	int first;
	int second;
	ben_win_type type;
} ben_win;

// @ben win combo
int combo_start_chars_written = 0;
int combo_end_chars_written = 0;

// @ben win combo print end
void ben_win_print_combo_end(const char* end)
{
	const char* end_str;
	combo_end_chars_written = strlen(end);
}

// @ben win mvwaddwstr
wchar_t* ben_win_mvwaddwstr(WINDOW* win, int y, int x, const wchar_t* str)
{
	static wchar_t* old_str;

	if (str == NULL)
	{
		mvwaddwstr(win, y, x, old_str);
		return old_str;
	}
	if (win == NULL)
	{
		free(old_str);
		return NULL;
	}

	int old_str_len = wcslen(str);
	int old_str_width = wcswidth(str, old_str_len);

	old_str = realloc(old_str, old_str_width * sizeof(wchar_t));

	for (int i = 0; i < old_str_width; i++)
	{
		cchar_t cc;
		mvwin_wch(win, y, x + i, &cc);
		getcchar(&cc, &old_str[i], 0, 0, 0);
	}

	mvwaddwstr(win, y, x, str);

	return old_str;
}

// @ben win cleanup
void ben_win_cleanup()
{
	ben_win_mvwaddwstr(0, 0, 0, 0);
}

// @main
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    size_t node_count = 2;
    ben_win* windows = malloc(sizeof(ben_win) * node_count);

    windows[0].window = NULL;
    windows[0].first = 1;
    windows[0].second = BEN_WIN_TYPE_NULL;
    windows[0].type = BEN_WIN_TYPE_NULL;

    windows[1].window = newwin(rows, cols, 0, 0);
    windows[1].first = BEN_WIN_TYPE_NULL;
    windows[1].second = BEN_WIN_TYPE_NULL;
    windows[1].type = BEN_WIN_TYPE_NULL;

    WINDOW* win = windows[1].window;
    box(win, 0, 0);
    wrefresh(win);

    curs_set(0);

    int c;
    while ((c = mvwgetch(win, 0, 0)) != 'q')
    {
	    getmaxyx(stdscr, rows, cols);

	    switch (c)
	    {
		    // for all of the possible combo starts
		    #define CS_W 1
		    case 'W':
			    wattron(win, A_BLINK);
			    box(win, 0, 0);
			    wattroff(win, A_BLINK);

			    wattron(win, A_REVERSE);
			    ben_win_mvwaddwstr(win, rows - 1, 1, L" W ");
			    wattroff(win, A_REVERSE);
			    break;

		    // for all of the possible combo ends
		    case 'd':
			    break;
		    case 'l':
			    /*
			     * Next step is to make opening and closing window
			     * functionality.  Actually,  you  should exit the
			     * program  simply  by  closing all windows.  That
			     * makes  the  most  sense  to  me.
			     *
			     * I need  to be able  to print the  window binary
			     * tree with a single function. I shouldn't do any
			     * of this stuff  where I  draw  the  windows   in
			     * different  places.
			     */
			    node_count++;
			    ben_win* new_alloc = malloc(sizeof(ben_win) * node_count);
			    memcpy(new_alloc, windows, sizeof(ben_win) * (node_count - 1));
			    windows = new_alloc;

			    int par_win_i = 0;
			    int cur_win_i = 1;
			    int new_win_i = node_count - 1;
			    windows[par_win_i].type   = BEN_WIN_TYPE_VERT;
			    windows[par_win_i].second = new_win_i;

			    int p_rows, p_cols;
			    getmaxyx(win, p_rows, p_cols);
			    int p_cols_left = p_cols / 2;
			    int p_cols_right = p_cols - p_cols_left;
			    windows[new_win_i].window = newwin(p_rows, p_cols_right, 0, p_cols_left );
			    windows[new_win_i].type   = BEN_WIN_TYPE_LEAF;
			    windows[new_win_i].first  = BEN_WIN_TYPE_NULL;
			    windows[new_win_i].second = BEN_WIN_TYPE_NULL;

			    wresize(win, p_rows, p_cols_left);
			    box(windows[new_win_i].window, 0, 0);
		    case 'h':
		    case 'j':
		    case 'k':
		    case 'r':
		    default:
			    box(win, 0, 0);
	    }

	    // if (asking for verification) switch () case 'y': verify(); //

	    for (int i = 0; i < node_count; i++)
	    {
		    if (windows[i].window)
			    wrefresh(windows[i].window);
	    }
    }

    delwin(win);
    free(windows);
    endwin();

    return 0;
}
