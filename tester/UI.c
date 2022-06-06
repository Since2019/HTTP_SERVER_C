#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <form.h>

static void reader(WINDOW *win, FORM *form);
//static void writer (WINDOW *win);
static void clear_screen (WINDOW *win);
static void main_screen(WINDOW *win);
static WINDOW *create_Main_Win();
static FORM *create_Form();

static void clear_screen (WINDOW *win){
	wclear(win);
}

static void main_screen(WINDOW *win){
	win = create_Main_Win();
	wrefresh(win);
}
/**
static void writer (WINDOW *win){
	wclear(win);
	printw("Make your request:\n");
	char message[100];
	//message = getstr();
	
	//win = createMainWin();
}
*/

static void reader(WINDOW *win, FORM *form){
	
	clear_screen(win);
	
	endwin();
	
	form = create_Form();
	int ch;
	
	/* Loop through to get user requests */
	while((ch = getch()) != KEY_F(1))
	{	switch(ch)
		{	case KEY_DOWN:
				/* Go to next field */
				form_driver(form, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(form, REQ_END_LINE);
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(form, REQ_PREV_FIELD);
				form_driver(form, REQ_END_LINE);
				break;
			default:
				/* If this is a normal character, it gets */
				/* Printed				  */	
				form_driver(form, ch);
				break;
		}
	}
	
	unpost_form(form);
	free_form(form);

	endwin();
	//printw("Now Reading");
	win = create_Main_Win();
	wrefresh(win);
	
}

static WINDOW *create_Main_Win(){
	
	 WINDOW * mainwin, * childwin;
	 
    /*  Set the dimensions and initial
	position for our child window   */
 
    int      width = 23, height = 7;
    int      rows  = 25, cols   = 80;
    int      x = (cols - width)  / 2;
    int      y = (rows - height) / 2;
 
    /*  Initialize ncurses  */
 
    if ( (mainwin = initscr()) == NULL ) {
	fprintf(stderr, "Error initialising ncurses.\n");
	exit(EXIT_FAILURE);
    }
 
    /*  Switch of echoing and enable keypad (for arrow keys)  */
 
    noecho();
    keypad(mainwin, TRUE);
 
    /*  Make our child window, and add
	a border and some text to it.   */
 
    childwin = subwin(mainwin, height, width, y, x);
    box(childwin, 0, 0);
    mvwaddstr(childwin, 1, 4, "Select  a Mode:");
    mvwaddstr(childwin, 3, 3, "Read - Press 'r'");
    mvwaddstr(childwin, 4, 3, "Write - Press 'w'");
    mvwaddstr(childwin, 5, 3, "Quit - Press 'q'");
 
    refresh();
	
	delwin(childwin);
	
	return mainwin;
}

static FORM *create_Form(){
	
	FIELD *field[4];
	FORM  *my_form;
	
	/* Initialize curses */
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	/* Initialize the fields */
	field[0] = new_field(1, 20, 4, 18, 0, 0);
	field[1] = new_field(1, 20, 6, 18, 0, 0);
	field[2] = new_field(1, 20, 8, 18, 0, 0);
	field[3] = NULL;

	/* Set field options */
	set_field_back(field[0], A_UNDERLINE); 	/* Print a line for the option 	*/
	field_opts_off(field[0], O_AUTOSKIP);  	/* Don't go to next field when this */
						/* Field is filled up 		*/
	set_field_back(field[1], A_UNDERLINE); 
	field_opts_off(field[1], O_AUTOSKIP);
	
	set_field_back(field[2], A_UNDERLINE); 
	field_opts_off(field[2], O_AUTOSKIP);

	/* Create the form and post it */
	my_form = new_form(field);
	post_form(my_form);
	refresh();
	
	mvprintw(4, 10, "404 File: ");
	mvprintw(6, 10, "HTTP Root Directory: ");
	mvprintw(8, 10, "Port Number: ");
	refresh();
	
	free_field(field[0]);
	free_field(field[1]); 
	free_field(field[2]); 
	
	return my_form;
	
}

int main(void){
	
    WINDOW * mainwin = create_Main_Win();
	FORM *form;
    int ch;

    /*  Loop until user hits 'q' to quit  */
 
   while ( (ch = getch()) != 'q' ) {
 
	switch ( ch ) {
	case 'r':
		reader(mainwin, form);
	    break;
 
	case 'w':
		//writer(mainwin);
	    break;
		
	default:
		beep();
		refresh();
		break;
		}
    }
 
    /*  Clean up after ourselves  */
    delwin(mainwin);
    endwin();
    refresh();
}