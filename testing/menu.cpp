#include <curses.h> 
#include <windows.h>
#include <locale.h>
#include <string>

using namespace std;

// g++ qwq.cpp -o qwq libpdcurses.a -I.

#define COLOR_GRAY 8

int ymax, xmax;


void ConVisual(){
    
    clear();

    for(int i=0; i<ymax; i++)
    {
        for(int j=0; j<xmax; j++)
            // if(j==0&&i==ymax-1) addch('E');
            // else if(j==0) addch('0'+i%10);
            // else if(j%9==0) addch('X');
            // else if(j==xmax-1) addch('E');
            // else addch('O');
            addch(' ');
    }    
    refresh();
}

void Mcon(){
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

void resbuf(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);          //取得控制Console輸出的"板手"
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    SMALL_RECT r;
    r.Left = r.Top = 0;
    r.Right = csbi.srWindow.Right;
    r.Bottom = csbi.srWindow.Bottom;                        //計算可視螢幕範圍
    SetConsoleWindowInfo(hOut, TRUE, &r);                   

    COORD size;                                             //宣告"長寬物件"
    size.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    SetConsoleScreenBufferSize(hOut, size);

}

void PDinit(){
    setlocale(LC_ALL, "C");
    Mcon();
    resbuf();
    SetConsoleOutputCP(437);
    SetConsoleCP(437);
    
    
    initscr();
    noecho();
    cbreak();
    start_color();
    
    init_color(COLOR_GRAY, 400, 400, 400);
    init_pair(1, COLOR_WHITE, COLOR_GRAY);

    getmaxyx(stdscr, ymax, xmax);
    resize_term(ymax, xmax);
}



int main(){

    PDinit();

    
    // attron(COLOR_PAIR(1));
    // printw("%d, %d", ymax, xmax);
    // getch();
    ConVisual();
    // getch();
    // clear();

    // printw("有更動!");
    WINDOW* swin = newwin((ymax/5)*4, (xmax/5)*4+1, ymax/2-(ymax/5)*4/2, xmax/2-(xmax/5)*4/2);
    WINDOW* win = newwin((ymax/5)*4, (xmax/5)*4, ymax/2-(ymax/5)*4/2, xmax/2-(xmax/5)*4/2);
    wbkgd(swin, COLOR_PAIR(1));
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);
    refresh();
    wrefresh(swin);
    wrefresh(win);

    keypad(win, true);
    string choices[3] = {"qwq", "owo", "uwu"};
    int choice;
    int highlight = 0;
    while(1)
    {
        for(int i=0; i<3; i++)
        {
            if(i==highlight) wattron(win, A_REVERSE);
            mvwprintw(win, i+1, 2, choices[i].c_str());
            wattroff(win, A_REVERSE);
        }
        
        choice = wgetch(win);
        switch(choice){
            case KEY_UP:
            highlight--;
            if(highlight<0) highlight=0;
            break;
            case KEY_DOWN:
            highlight++;
            if(highlight>2) highlight=2;
            break;
            default:
            break;
        }
        if(choice == 10) break;
    }
    
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "You've chosen %s!", choices[highlight].c_str());
    wrefresh(win);
    
    attroff(COLOR_PAIR(1));
    getch();
    endwin();
}