#include <curses.h> 
#include <windows.h>
#include <locale.h>
#include <string>
#include <vector>


using namespace std;

// g++ qwq.cpp -o qwq libpdcurses.a -I.

#define COLOR_GRAY 8
#define y first
#define x second

int ymax, xmax;
vector<vector<int>> obs, mapping;


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
    setlocale(LC_ALL, "en_US.UTF-8");
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


void GetRsl(int &h, int &w){
    h = (ymax/5)*4;
    w = (xmax/5)*4;

    if((h-2)%2!=0)  h+=1;
    if((w-2)%3!=0)  w = 3*((w-2)/3)+2;
}

int main(){

    PDinit();

    
    // attron(COLOR_PAIR(1));
    // printw("%d, %d", ymax, xmax);
    // getch();
    ConVisual();
    // getch();
    // clear();

    int Mh, Mw;
    GetRsl(Mh, Mw);

    // printw("有更動!");
    WINDOW* swin = newwin(Mh, Mw+1, ymax/2-Mh/2, xmax/2-Mw/2);
    WINDOW* win = newwin(Mh, Mw, ymax/2-Mh/2, xmax/2-Mw/2);
    wbkgd(swin, COLOR_PAIR(1));
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);

    keypad(win, true);

    int ty, tx; // 36, 146
    getmaxyx(win, ty, tx);
    mvprintw(1, 1, "%d, %d, %d, %d", ty, tx, Mh, Mw);
    refresh();
    
    int iny, inx;
    iny = (ty-2)/2;
    inx = (tx-3+1)/3;

    vector<int> tmp;
    tmp.assign(inx+1, 0);
    obs.assign(iny+1, tmp);
    mapping.assign(iny+1, tmp);

    int choice;
    pair<int, int> slc = {1, 1};//(y, x)
    while(1)
    {
        
        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
    
                mvwaddch(win, i, j, ACS_ULCORNER);
                mvwaddch(win, i, j+1, ACS_HLINE);
                mvwaddch(win, i, j+2, ACS_URCORNER);
                mvwaddch(win, i+1, j, ACS_LLCORNER);
                mvwaddch(win, i+1, j+1, ACS_HLINE);
                mvwaddch(win, i+1, j+2, ACS_LRCORNER);
                
                wattroff(win, A_REVERSE);
            }
        }
        choice = wgetch(win);
    
        switch(choice){
            case KEY_UP:
                slc.y-=1;
                if(slc.y < 1) slc.y = 1;
                break;
            case KEY_LEFT:
                slc.x-=1;
                if(slc.x < 1) slc.x = 1;
                break;
            case KEY_DOWN:
                slc.y+=1;
                if(slc.y > iny) slc.y = iny;
                break;
            case KEY_RIGHT:
                slc.x+=1;
                if(slc.x > inx) slc.x = inx;
                break;
            default:
                break;
        }
    
        if(choice == ' ') break;
    }

    wclear(win);
    mvwprintw(win, 1, 1, "You've chosen {y: %d, x: %d}", slc.y, slc.x);
    wrefresh(win);
    getch();
    wclear(win);
    box(win, 0, 0);

    wrefresh(win);


    refresh();

    
    
    attroff(COLOR_PAIR(1));
    getch();
    endwin();
}

