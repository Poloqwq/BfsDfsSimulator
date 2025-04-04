#include <curses.h> 
#include <windows.h>
#include <locale.h>
#include <string>
#include <vector>
#include <queue>
#include <stack>


using namespace std;

// g++ BfsDfsSimulator.cpp -o BfsDfsSimulator libpdcurses.a -I.

#define COLOR_GRAY 8
#define COLOR_DARKGREEN 9
#define STARTING_COLOR COLOR_PAIR(2)
#define OBS_COLOR COLOR_PAIR(3)
#define GOAL_COLOR COLOR_PAIR(4)

#define y first
#define x second

int ymax, xmax;
vector<vector<int>> obs, mapping;
pair<int, int> starting_point, goal_point;//(y, x)


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

void DrawSquare(WINDOW* w, int i, int j){
    mvwaddch(w, i, j, ACS_ULCORNER);
    mvwaddch(w, i, j+1, ACS_HLINE);
    mvwaddch(w, i, j+2, ACS_URCORNER);
    mvwaddch(w, i+1, j, ACS_LLCORNER);
    mvwaddch(w, i+1, j+1, ACS_HLINE);
    mvwaddch(w, i+1, j+2, ACS_LRCORNER);
}

void DrawXs(WINDOW* w, int i, int j){
    mvwaddch(w, i, j, 'X');
    mvwaddch(w, i, j+1, 'X');
    mvwaddch(w, i, j+2, 'X');
    mvwaddch(w, i+1, j, 'X');
    mvwaddch(w, i+1, j+1, 'X');
    mvwaddch(w, i+1, j+2, 'X');
}

void DrawGs(WINDOW* w, int i, int j){
    mvwaddch(w, i, j, 'G');
    mvwaddch(w, i, j+1, 'G');
    mvwaddch(w, i, j+2, 'G');
    mvwaddch(w, i+1, j, 'G');
    mvwaddch(w, i+1, j+1, 'G');
    mvwaddch(w, i+1, j+2, 'G');
}

void reprint(WINDOW* w, int _y, int _x, string s, int clear_flag){
    if(clear_flag) wclear(w);
    mvwprintw(w, _y, _x, s.c_str());
    box(w, 0, 0);
    wrefresh(w);
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
    init_pair(2, COLOR_DARKGREEN, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_YELLOW);

    getmaxyx(stdscr, ymax, xmax);
    resize_term(ymax, xmax);
}


void GetRsl(int &h, int &w){
    h = (ymax/5)*4;
    w = (xmax/5)*4;

    if((h-2)%2!=0)  h+=1;
    if((w-2)%3!=0)  w = 3*((w-2)/3)+2;
}

bool bfs(WINDOW* win, int iny, int inx, int ty, int tx);
bool dfs(WINDOW* win, int iny, int inx, int ty, int tx);

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

    WINDOW* swin = newwin(Mh, Mw+1, ymax/2-Mh/2, xmax/2-Mw/2);
    WINDOW* win = newwin(Mh, Mw, ymax/2-Mh/2, xmax/2-Mw/2);
    wbkgd(swin, COLOR_PAIR(1));
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);
    wrefresh(swin);
    wrefresh(win);
    
    WINDOW* sinfo = newwin(Mh/8, Mw+1, 1, xmax/2-Mw/2);
    WINDOW* info = newwin(Mh/8, Mw, 1, xmax/2-Mw/2);
    wbkgd(sinfo, COLOR_PAIR(1));
    wbkgd(info, COLOR_PAIR(1));
    box(info, 0, 0);
    wrefresh(sinfo);
    wrefresh(info);


    keypad(win, true);
    
    int ty, tx; // 36, 146
    getmaxyx(win, ty, tx);
    // mvprintw(1, 1, "%d, %d, %d, %d", ty, tx, ymax, xmax);
    // mvprintw(2, 1, "%d, %d", win->_begy, win->_begx);
    refresh();
    
    int iny, inx;
    iny = (ty-2)/2;
    inx = (tx-3+1)/3;

    vector<int> tmp;
    tmp.assign(inx+1, 0);
    obs.assign(iny+1, tmp);
    mapping.assign(iny+1, tmp);

    string s = "Please choose your starting coordinate.";
    //y=(info->_maxy-1)/2, x=info->_maxx/2
    reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2-s.size()/2, s, true);
    s = "Press space bar to select!";
    reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);
    
    int op;
    pair<int, int> slc = {1, 1};//(y, x)
    while(1) //select starting point
    {
        
        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
                DrawSquare(win, i, j);
                wattroff(win, A_REVERSE);
            }
        }
        op = wgetch(win);
    
        switch(op){
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
    
        if(op == ' ') break;
    }

    mapping[slc.y][slc.x] = 2;
    starting_point = {slc.y, slc.x};


    s = "Now choose which are the obstacles.";
    reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
    s = "press space bar to selcet / press enter when all done / press r to reset / press e to erase";
    reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);

    op = 0;
    int flag=0;
    while(1) //select ostacles
    {
        if(flag==1)
        {
            s = "THAT is a starting point! Choose another one!";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
        }
        else if(flag==2)
        {
            s = "You've already chosen this one! Choose another one!";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
        }
        else 
        {
            s = "Now choose which are the obstacles.";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
            s = "press space bar to selcet / press enter when all done / press r to reset / press e to erase";
            reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);
        }


        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if(obs[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, OBS_COLOR);
                    if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
                    DrawXs(win, i, j);
                }
                else
                {
                    if(mapping[(i+1)/2][(j+2)/3] == 2) wattron(win, STARTING_COLOR);
                    if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
                    DrawSquare(win, i, j);
                }
                
                wattroff(win, A_REVERSE);
                wattroff(win, STARTING_COLOR);
                wattroff(win, OBS_COLOR);
            }
        }

        flag = 0;
        op = wgetch(win);
        if(op == 10) break;

        switch(op){
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
            case ' ':
                if(obs[slc.y][slc.x]==1) flag=2;
                if(starting_point != pair<int,int>{slc.y, slc.x}) obs[slc.y][slc.x] = 1;
                else flag=1;
                break;
            case 'R':
            case 'r':
                for(auto &row : obs)
                    row.assign(row.size(), 0);
                break;
            case 'E':
            case 'e':
                obs[slc.y][slc.x] = 0;
                break;
            default:
                break;
        }  
    }
    
    s = "Now choose which is the goal point!";
    reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
    s = "Press space bar to select!";
    reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);
    
    op=0;
    flag=0;
    while(1) //select goal point
    {
        if(!flag)
        {
            s = "Now choose which is the goal point!";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
            s = "Press space bar to select!";
            reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);
        }
        else if(flag==1)
        {
            s = "THAT is an obstacle! Choose another one!";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
        }
        else if(flag==2)
        {
            s = "THAT is your starting point! Choose another one!";
            reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
        }
        
        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if(obs[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, OBS_COLOR);
                    if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
                    DrawXs(win, i, j);
                }
                else if(goal_point == pair<int, int>{(i+1)/2, (j+2)/3})
                {
                    wattron(win, GOAL_COLOR);
                    DrawGs(win, i, j);
                    wattroff(win, GOAL_COLOR);
                }
                else
                {
                    if(mapping[(i+1)/2][(j+2)/3] == 2) wattron(win, STARTING_COLOR);
                    if((i+1)/2==slc.y && (j+2)/3==slc.x) wattron(win, A_REVERSE);
                    DrawSquare(win, i, j);
                }
                
                wattroff(win, A_REVERSE);
                wattroff(win, STARTING_COLOR);
                wattroff(win, OBS_COLOR);
                wattroff(win, GOAL_COLOR);
            }
        }
        if(op == ' ') break;
        op = wgetch(win);
        switch(op){
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
            case ' ':
                if(!obs[slc.y][slc.x] && !mapping[slc.y][slc.x]) goal_point = {slc.y, slc.x};
                else
                {
                    op=0;
                    if(obs[slc.y][slc.x]) flag=1;
                    else if(mapping[slc.y][slc.x]) flag = 2;
                }
                break;
            default:
                break;
        }
    }
    wrefresh(win);

    s = "Now select your searching method!";
    reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
    s = "Press space bar to select!";
    reprint(info, (info->_maxy-1)/2+1, (info->_maxx-1)/2 - s.size()/2, s, false);

    WINDOW* swbfs = newwin(Mh/8, Mw/3+1, ((win->_begy+win->_maxy-1) + ymax)/2 - Mh/16, ((win->_begx + xmax/2)/2 - Mw/6));
    WINDOW* wbfs = newwin(Mh/8, Mw/3, ((win->_begy+win->_maxy-1) + ymax)/2 - Mh/16, ((win->_begx + xmax/2)/2 - Mw/6));
    wbkgd(swbfs, COLOR_PAIR(1));
    wbkgd(wbfs, COLOR_PAIR(1));
    wrefresh(swbfs);
    
    WINDOW* swdfs = newwin(Mh/8, Mw/3+1, ((win->_begy+win->_maxy-1) + ymax)/2 - Mh/16, ((win->_begx+win->_maxx-1)/2 + xmax/4) - Mw/6);
    WINDOW* wdfs = newwin(Mh/8, Mw/3, ((win->_begy+win->_maxy-1) + ymax)/2 - Mh/16, ((win->_begx+win->_maxx-1)/2 + xmax/4) - Mw/6);
    wbkgd(swdfs, COLOR_PAIR(1));
    wbkgd(wdfs, COLOR_PAIR(1));
    wrefresh(swdfs);

    keypad(wbfs, true);
    
    // mvprintw(43, 1, "%d, %d, %d, %d, %d", Mh/8, Mw/3, ((win->_begy+win->_maxy-1) + ymax)/2 - Mh/16, ((win->_begx+win->_maxx-1) + xmax)/4 - Mw/6, ((win->_begx+win->_maxx-1)/2 + xmax/4) - Mw/6);
    // getch();
    int highlight = 1;
    op = 0;
    while(1)
    {
        if(highlight == 1) wbkgd(wbfs, COLOR_PAIR(1) | A_REVERSE);
        else
        {
            wbkgd(wbfs, A_NORMAL);
            wbkgd(wbfs, COLOR_PAIR(1));
        }
        s = "BFS (Breadth First Search)";
        reprint(wbfs, (wbfs->_maxy-1)/2, (wbfs->_maxx-1)/2 - s.size()/2, s, true);
        
        if(highlight == 2) wbkgd(wdfs, COLOR_PAIR(1) | A_REVERSE);
        else
        {
            wbkgd(wdfs, A_NORMAL);
            wbkgd(wdfs, COLOR_PAIR(1));
        } 
        s = "DFS (Depth First Search)";
        reprint(wdfs, (wdfs->_maxy-1)/2, (wdfs->_maxx-1)/2 - s.size()/2, s, true);
        
        wrefresh(wbfs);
        wrefresh(wdfs);
        
        op = wgetch(wbfs);
        switch(op){
            case KEY_LEFT:
            highlight -= 1;
            if(highlight < 1) highlight = 1;
            break;
            case KEY_RIGHT:
            highlight += 1;
            if(highlight > 2) highlight = 2;
            break;
            default:
            break; 
        }
        
        if(op==' ') break;
    }
    
    
    
    bool check;
    if(highlight==1) check = bfs(win, iny, inx, ty, tx);
    else check = dfs(win, iny, inx, ty, tx);
    if(check)
    {
        s = "We have reached the goal point! WEEEEHOOOOOO!";
        reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
    }
    else
    {
        s = "We can't reach the goal point! OHH NYOOO! :(((";
        reprint(info, (info->_maxy-1)/2, (info->_maxx-1)/2 - s.size()/2, s, true);
    }

    
    // mvprintw(44, 1, "You've chosen {%d, %d}", starting_point.y, starting_point.x);
    attroff(COLOR_PAIR(1));
    getch();
    endwin();
}

bool bfs(WINDOW* win, int iny, int inx, int ty, int tx){
    queue<pair<int, int>> record;
    record.push(starting_point);
    

    int flag = 0;    
    while(!record.empty()){
        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if(obs[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, OBS_COLOR);
                    DrawXs(win, i, j);
                }
                else if(goal_point == pair<int, int>{(i+1)/2, (j+2)/3})
                {
                    wattron(win, GOAL_COLOR);
                    DrawGs(win, i, j);
                    wattroff(win, GOAL_COLOR);
                }
                else if(mapping[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, A_REVERSE);
                    DrawSquare(win, i, j);
                }
                else
                {
                    if(mapping[(i+1)/2][(j+2)/3] == 2) wattron(win, STARTING_COLOR);
                    DrawSquare(win, i, j);
                }
                
                wattroff(win, A_REVERSE);
                wattroff(win, STARTING_COLOR);
                wattroff(win, OBS_COLOR);
                wattroff(win, GOAL_COLOR);
            }
        }

        Sleep(20);
        wrefresh(win);
        
        // if(mapping[goal_point.y][goal_point.x]) break;

        pair<int, int> tmp = record.front();
        record.pop();
        if(!flag)
        {
            if(tmp.x+1<=inx) // 防條件式overflow
            {
                if(!obs[tmp.y][tmp.x+1] && !mapping[tmp.y][tmp.x+1])
                {
                    record.push({tmp.y, tmp.x+1});
                    mapping[tmp.y][tmp.x+1] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y, tmp.x+1}) flag = 1;
            } 
            if(tmp.y+1<=iny) // (用聯立會overflow)
            {
                if(!obs[tmp.y+1][tmp.x] && !mapping[tmp.y+1][tmp.x])
                {
                    record.push({tmp.y+1, tmp.x});
                    mapping[tmp.y+1][tmp.x] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y+1, tmp.x}) flag = 1;
            } 
            if(tmp.x-1>=1) 
            {
                if(!obs[tmp.y][tmp.x-1] && !mapping[tmp.y][tmp.x-1])
                {
                    record.push({tmp.y, tmp.x-1});
                    mapping[tmp.y][tmp.x-1] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y, tmp.x-1}) flag = 1;
            }
            if(tmp.y-1>=1) 
            {
                if(!obs[tmp.y-1][tmp.x] && !mapping[tmp.y-1][tmp.x])
                {
                    record.push({tmp.y-1, tmp.x});
                    mapping[tmp.y-1][tmp.x] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y-1, tmp.x}) flag = 1;
            }
        }
    }
    return flag;

}

bool dfs(WINDOW* win, int iny, int inx, int ty, int tx){
    stack<pair<int, int>> record;
    vector<vector<int>> rem = mapping;
    record.push(starting_point);

    int flag = 0;    
    while(!record.empty()){
        for(int i=1; i < ty-2; i+=2) // 1, 3, 5, 7, ..., 31, 33 (34) n=17 n=(an+1)/2
        {
            for(int j=1; j < tx-3; j+=3) // 1, 4, 7, ..., 139, 142 (143) n=48 n=(an+2)/3
            {
                if(obs[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, OBS_COLOR);
                    DrawXs(win, i, j);
                }
                else if(goal_point == pair<int, int>{(i+1)/2, (j+2)/3})
                {
                    wattron(win, GOAL_COLOR);
                    DrawGs(win, i, j);
                    wattroff(win, GOAL_COLOR);
                }
                else if(mapping[(i+1)/2][(j+2)/3] == 1)
                {
                    wattron(win, A_REVERSE);
                    DrawSquare(win, i, j);
                }
                else
                {
                    if(mapping[(i+1)/2][(j+2)/3] == 2) wattron(win, STARTING_COLOR);
                    DrawSquare(win, i, j);
                }
                
                wattroff(win, A_REVERSE);
                wattroff(win, STARTING_COLOR);
                wattroff(win, OBS_COLOR);
                wattroff(win, GOAL_COLOR);
            }
        }

        Sleep(20);
        wrefresh(win);
        
        if(rem[goal_point.y][goal_point.x]) break;

        pair<int, int> tmp = record.top();
        record.pop();
        if(!flag) //
        {
            if(tmp.y-1>=1) 
            {
                if(!obs[tmp.y-1][tmp.x] && !rem[tmp.y-1][tmp.x])
                {
                    record.push({tmp.y-1, tmp.x});
                    rem[tmp.y-1][tmp.x] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y-1, tmp.x}) flag = 1;
            }
            if(tmp.x-1>=1) 
            {
                if(!obs[tmp.y][tmp.x-1] && !rem[tmp.y][tmp.x-1])
                {
                    record.push({tmp.y, tmp.x-1});
                    rem[tmp.y][tmp.x-1] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y, tmp.x-1}) flag = 1;
            }
            if(tmp.y+1<=iny)
            {
                if(!obs[tmp.y+1][tmp.x] && !rem[tmp.y+1][tmp.x])
                {
                    record.push({tmp.y+1, tmp.x});
                    rem[tmp.y+1][tmp.x] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y+1, tmp.x}) flag = 1;
            } 
            if(tmp.x+1<=inx)
            {
                if(!obs[tmp.y][tmp.x+1] && !rem[tmp.y][tmp.x+1])
                {
                    record.push({tmp.y, tmp.x+1});
                    rem[tmp.y][tmp.x+1] = 1;
                }
                if(goal_point == pair<int, int>{tmp.y, tmp.x+1}) flag = 1;
            } 
            mapping[tmp.y][tmp.x] = 1;
        }
    }
    return flag;
}