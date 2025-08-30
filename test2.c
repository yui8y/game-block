#include <ncurses.h>
#include <stdlib.h>

static void enable_any_event_mouse(void) {
    // any-event(1003) + SGR(1006) mouse
    printf("\033[?1003h\033[?1006h");
    fflush(stdout);
}
static void disable_any_event_mouse(void) {
    printf("\033[?1003l\033[?1006l");
    fflush(stdout);
}

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mouseinterval(0);

    mmask_t old;
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &old);

    // 端末によっては ncurses の自動設定だけだと 1002 止まりになることがあるため、
    // 念のため 1003/1006 を手動で有効化
    enable_any_event_mouse();

    nodelay(stdscr, FALSE);
    curs_set(0);

    mvprintw(0,0,"Move mouse (no button needed). Clicks also OK. Press 'q' to quit.");
    refresh();

    int ch;
    MEVENT me;
    while ((ch = getch()) != 'q') {
        if (ch == KEY_MOUSE && getmouse(&me) == OK) {
            // REPORT_MOUSE_POSITION の有無で移動イベントを判定
            bool is_move = (me.bstate & REPORT_MOUSE_POSITION) != 0;
            clear();
            mvprintw(0,0,"x=%d y=%d bstate=0x%lx  %s",
                     me.x, me.y, (unsigned long)me.bstate,
                     is_move ? "[MOVE]" : "[CLICK/OTHER]");
            mvprintw(2,0,"Press 'q' to quit.");
            refresh();
        }
    }

    disable_any_event_mouse();
    endwin();
    return 0;
}
