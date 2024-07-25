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
*/

// @key typedef
typedef uint64_t ben_key;
#define NCURSES_KEY_ALT 27

typedef uint8_t ben_win_type;
#define BEN_WIN_TYPE_NULL  0
#define BEN_WIN_TYPE_LEAF  1
#define BEN_WIN_TYPE_LEFT  2
#define BEN_WIN_TYPE_RIGHT 3

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

    ben_win* windows = malloc(sizeof(ben_win));

    windows[0].window = NULL;
    windows[0].first = 1;
    windows[0].second = BEN_WIN_TYPE_NULL;
    windows[0].type = BEN_WIN_TYPE_NULL;

    windows[1].window = NULL;
    windows[1].first = BEN_WIN_TYPE_NULL;
    windows[1].second = BEN_WIN_TYPE_NULL;
    windows[1].type = BEN_WIN_TYPE_NULL;

    WINDOW* win = newwin(rows, cols, 0, 0);
    windows[1].window = win;

    box(win, 0, 0);
    wrefresh(win);

    curs_set(0);

    int cs = 0;
    int ce = 0;
    int c;
    while ((c = mvwgetch(win, 0, 0)) != 'q')
    {
	    getmaxyx(stdscr, rows, cols);

	    switch (c)
	    {
		    // for all of the possible combo starts
		    #define CS_W 1
		    case 'W':
			    wattron(win, A_REVERSE);
			    ben_win_mvwaddwstr(win, rows - 1, 1, L" W ");
			    wattroff(win, A_REVERSE);
			    break;

		    // for all of the possible combo ends
		    case 'h':
		    case 'j':
		    case 'k':
		    case 'l':
		    case 'r':
		    case 'd':
			    /*
			     * Next step is to make opening and closing window
			     * functionality.  Actually,  you  should exit the
			     * program  simply  by  closing all windows.  That
			     * makes  the  most  sense  to  me.
			     * 
			     * However,  the problem  right now  is   that the
                             * current method for getting what is  overwritten
                             * doesn't seem to  accurately retrieve the  width
			     * of   overwritten   characters.
			     * 
			     * I am really tired of this bullshit trying to
			     * draw over the border. I don't know why it is so
			     * difficult to simply get the contents ncurses
			     * overwrites and buff them into a buffer. I
			     * should just give up and redraw the border each
			     * time. It's probably not that slow.
			     */
			    box(win, 0, 0);
			    ce = c;
			    cs = 0;
			    break;
	    }

	    // if (asking for verification) switch () case 'y': verify(); //

	    wrefresh(win);
    }

    delwin(win);
    free(windows);
    endwin();

    return 0;
}
