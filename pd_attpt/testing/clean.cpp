#include <curses.h>
#include <windows.h>
#include <vector>

#define COLOR_DARKGREEN 8
#define y first
#define x second

using namespace std;
int ChWidth = 8;
int ChHeight = 16;

int ScWidth;
int ScHeight;

int ConCols;
int ConRows;

void resBuf(){
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hwnd = GetConsoleWindow();
    
    ScWidth = GetSystemMetrics(SM_CXSCREEN);
    ScHeight = GetSystemMetrics(SM_CYSCREEN);
    
    
    
}

int main(){
    
    // resBuf();
    
    
    initscr();
    start_color();

    WINDOW* win = newwin(5, 5, 1, 1);
    wbkgd(win, COLOR_PAIR(1));
    refresh();
    wrefresh(win);
    

    init_color(COLOR_DARKGREEN, 0, 1, 0);
    init_pair(1, COLOR_DARKGREEN, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    wattron(win, COLOR_PAIR(2));
    wprintw(win, "X\n");
    wattroff(win, COLOR_PAIR(2));
    wrefresh(win);
    mvwaddch(win, 4, 1, 'a');
    wgetch(win);

    // pair<int, int> starting_point = {1, 2}, slc = {1, 2};
    // if(starting_point == pair<int,int>{slc.y, slc.x}) printw("yes");
    

    getch();
    endwin();
}