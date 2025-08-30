#include <ncurses.h>
#include <time.h>

static void enable_any_event_mouse(void) {
    // any-event(1003) + SGR(1006) mouse
    printf("\033[?1003h\033[?1006h");
    fflush(stdout);
}
static void disable_any_event_mouse(void) {
    printf("\033[?1003l\033[?1006l");
    fflush(stdout);
}




int main(void){
    initscr(); //初期化
    noecho(); //キーが入力されても表示しない
    curs_set(0); //カーソルを非表示(0)
    keypad(stdscr, TRUE); //xtermでマウスイベントの取得に必要　カーソルキーをコード内容ではなくする
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL); //マウスイベントを取得
    MEVENT e;
    int px = 2;
    
    // getch の タイムアウトを最低値にする
    // 0 にもできるが、リフレッシュでちらついてしまう
    timeout(1);


    // 端末によっては ncurses の自動設定だけだと 1002 止まりになることがあるため、
    // 念のため 1003/1006 を手動で有効化
    enable_any_event_mouse();



    clock_t start_time = clock();
    clock_t end_time;

    int bx = 0, by = 0; // アスタリスク位置
    int dbx = 1, dby = 1;//アスタリスク進行方向確認

    typedef struct blockStruc {
        int x;
        int y;
        bool live;
    }block;


    int k = 0;
    block bl[10];
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            bl[k].x = j + 5;
            bl[k].y = i + 5;
            bl[k].live = TRUE;
            k++;
        }
    }


    while (true) {
        int ch = getch();
        if(ch == 'q') break;
        if (ch == KEY_MOUSE){
            if(getmouse(&e) == OK){
                //getch()の返り値がKEY_MOUSEだった時、getmouse関数を使ってマウスイベントを取る
                clear(); //全画面クリア
                px = e.x;
                if(px < 2) px = 2;
                if(px > 77) px = 77;
            }
        }

        // アニメーション用に経過時間を計測する
        end_time = clock();
        double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

                
        // 経過時間が 0.004秒以上ならばテストアニメーションを更新する
        // なお、Timeout 時だけ処理す場合は
        // if( ch == ERR ){ などとする
        if (elapsed_time >= 0.006) {
            bx += dbx;
            by += dby;
            if( bx > 78 ) {
                dbx = -1;
            }
            if( bx < 0 ){
                dbx = 1;
            }

            if(by == 23){
                if(px - 2 == bx || px - 1 == bx || px == bx || px + 1 == bx){
                    dby = -1;
                }
            }
            if(by < 0){
                dby = 1;
            }
            if(by > 30){
                break;
            }

            for (int i = 0; i < 10; i++){
                if(bl[i].x == bx && bl[i].y == by){
                    bl[i].live = FALSE;
                    if(dby == 1){
                        dby = -1;
                    }else{
                        dby = 1;
                    }
                }
            }


            start_time = clock();
        }

        clear();
        mvprintw(by, bx, "*");
        mvprintw(23, px - 2, "-----");
        for (int i = 0; i < 10; i++){
            if(bl[i].live == TRUE){
                mvprintw(bl[i].y, bl[i].x, "O");
            }
        }

        // かかった時間の出力を行うデバッグコード
        // mvprintw(15, 10, "%f \n", elapsed_time);


        refresh(); //画面を更新

    }

    disable_any_event_mouse();
    endwin();
}