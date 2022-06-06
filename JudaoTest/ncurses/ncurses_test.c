#include <ncurses.h>

int main(int argc,char **argv){
    
    initscr();
    refresh();
    getch();
    endwin();
    return 0; 
}