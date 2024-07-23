#include <stdint.h>
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
#define BEN_KEY_ALT 1ULL << 32
#define NCURSES_KEY_ALT 27

// @ben win
#define BEN_WIN_FLAGS_DIRTY_BIT 1

typedef struct ben_win
{
	WINDOW* ncurses_win_ptr;
	struct ben_win* region_child_win_ptr;
	int flags;
} ben_win;

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

    int w = 0;
    ben_win* windows = malloc(sizeof(ben_win));

    windows[0].ncurses_win_ptr = newwin(rows, cols, 0, 0);
    windows[0].region_child_win_ptr = NULL;
    windows[0].flags = 0;

    box(windows[0].ncurses_win_ptr, 0, 0);
    wrefresh(windows[0].ncurses_win_ptr);

    ben_win* current_benwin = &windows[0];
    WINDOW* current_nwin = windows[0].ncurses_win_ptr;

    curs_set(0);

    chtype ch = mvwinch(current_nwin, 0, 0);
    ben_key bc = 0;
    int c = bc;
    while ((bc = (ben_key)mvwgetch(current_nwin, 0, 0)) != 'q')
    {
	    c = (int)bc;
	    mvwaddch(current_nwin, 0, 0, ch);

	    switch (c)
	    {
		    case NCURSES_KEY_ALT:
			    bc |= BEN_KEY_ALT;
			    break;
	    }

	    wrefresh(current_nwin);
    }

    /*
    srand(time(0));

    int vowel_index_selected = rand() % ALLOPHONE_COUNT;
    int c = 0;
    while (c != 'q')
    {
	    if (c == KEY_RESIZE)
	    {
                    getmaxyx(stdscr, rows, cols);
	            wresize(win_orig, rows, orig_x );
	            wresize(win_keyboard, keyboard_y, cols - orig_x );
	            wresize(win_input, rows - keyboard_y, cols - orig_x - menu_x);
	            wresize(win_menu, rows - keyboard_y, menu_x);
	            mvwin(win_menu, keyboard_y, cols - menu_x);

	            wclear(win_orig);
	            wclear(win_keyboard);
	            wclear(win_menu);
	            wclear(win_input);
	    }

            print_original(win_orig, quiz_flags);
	    print_keyboard(win_keyboard);
	    print_menu(win_menu, (quiz_flags & QUIZ_FLAGS_SINGLE_KEY_MODE_BIT) >> 2);
	    print_input(win_input, vowel_index_selected, quiz_flags);

	    c = process_input(win_input, &engine, &quiz_flags, vowel_index_selected, &vowel_index_selected);

	    refresh();
    }
    */

    delwin(windows[0].ncurses_win_ptr);
    free(windows);
    endwin();

    return 0;
}
