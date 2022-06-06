#include <curses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libconfig.h>

static void reader(WINDOW *win);
static void clear_screen (WINDOW *win);
static void main_screen(WINDOW *win);
static WINDOW *create_Main_Win();
static WINDOW *create_Form_Win();
static void form_Func(WINDOW *win);
static char* trim_whitespaces(char *str);
//static FORM *create_Form();

static FORM *form;
static FIELD *fields[7];

static char* trim_whitespaces(char *str)
{
	int length = strlen(str);
	char * newString = calloc(length, sizeof(char*));
	int index = 0;
	
	for(int i = 0; i < length; i++){
		if(!isspace(str[i])){
			newString[index++] = str[i];
		}
	}
	
	if(index == 0){
		free(newString);
		return NULL;
	}
	
	newString[index] = '\0';

	return newString;
}

static void form_Func(WINDOW *win){
	
	int nextChar;	
	config_t cfg;	
	
	while ((nextChar = getch()) != '='){
		
	switch (nextChar) {
		
		case '-':
			config_init(&cfg);
			config_setting_t *items, *root;
			root = config_root_setting(&cfg);
			
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_PREV_FIELD);
			
			char *str = trim_whitespaces(field_buffer(fields[1], 0));
			
			char *str2 = trim_whitespaces(field_buffer(fields[3], 0));
			
			char *str3 = trim_whitespaces(field_buffer(fields[5], 0));
			
				
	items = config_setting_add(root, "fourfile", CONFIG_TYPE_STRING);
	config_setting_set_string(items, str);	
	
	items = config_setting_add(root, "portNumber", CONFIG_TYPE_STRING);
	config_setting_set_string(items, str3);

	items = config_setting_add(root, "directory", CONFIG_TYPE_STRING);
	config_setting_set_string(items, str2);
	
	 if(! config_write_file(&cfg, "config.cfg"))
	  {
		fprintf(stderr, "Error while writing file.\n");
		config_destroy(&cfg);
	  } 
	
		if(!config_read_file(&cfg, "config.cfg")){
			config_destroy(&cfg);
		}
		
		/** 
		
		fourfile = "404.html";
portNumber = "1882";
directory = "bin/etc";
		*/
		
			mvwprintw(win, 15, 21, "                                                         ");
			
			mvwprintw(win, 16, 27, "                                                   ");
			
			mvwprintw(win, 17, 24, "                                                      ");
		
			if(str != NULL){
			mvwprintw(win, 15, 21, str);	
			free(str);
			}
			if(str2 != NULL){
			mvwprintw(win, 16, 27, str2);	
			free(str2);
			}
			if(str3 != NULL){
			mvwprintw(win, 17, 24, str3);	
			free(str3);
			}
			
			refresh();
			pos_form_cursor(form);
			config_destroy(&cfg);
			break;
			
		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;

		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;

		
		case KEY_BACKSPACE:
		case 127:
			form_driver(form, REQ_DEL_PREV);
			break;
			
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;

		default:
			form_driver(form, nextChar);
			break;
	}

	wrefresh(win);
		
	}
	
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	free_field(fields[4]);
	free_field(fields[5]);

	
	delwin(win);
	
}

static void reader(WINDOW *win){
	clear_screen(win);
	win = create_Form_Win();
	wrefresh(win);
	form_Func(win);
	clear_screen(win);
	main_screen(win);
}

static WINDOW *create_Form_Win(){
	
	config_t cfg;
	
	config_init(&cfg);
	
	if(!config_read_file(&cfg, "config.cfg")){
		config_destroy(&cfg);
	}
	
	const char* str;
	config_lookup_string(&cfg, "fourfile", &str);
	const char* str2;
	config_lookup_string(&cfg, "portNumber", &str2);
	const char* str3;
	config_lookup_string(&cfg, "directory", &str3);
	
	//char *str4 = strcat("404 File: ", str);
	
	WINDOW *mainwin, *childwin;
	
	mainwin = newwin(25, 80, 0, 0);
	
	childwin = derwin(mainwin, 15, 78, 4, 1);
	box(childwin, 0, 0);
	
	mvwprintw(mainwin, 1, 30, "Configuration Options");
	
	fields[0] = new_field(1, 30, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 25, 0, 0);
	fields[2] = new_field(1, 30, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 25, 0, 0);
	fields[4] = new_field(1, 30, 4, 0, 0, 0);
	fields[5] = new_field(1, 30, 4, 25, 0, 0);
	fields[6] = NULL;
	
	set_field_buffer(fields[0], 0, "404 File: ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "HTTP Root Directory: ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "Port Number: ");
	set_field_buffer(fields[5], 0, "");

	
	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);	

	
	mvwprintw(mainwin, 3, 16, "(Press '=' to go back, and '-' to accept changes.)");
	mvwprintw(mainwin, 15, 2, "Current 404 File = ");
	mvwprintw(mainwin, 15, 21, str);
	mvwprintw(mainwin, 16, 2, "Current Root Directory = ");
	mvwprintw(mainwin, 16, 27, str3);
	mvwprintw(mainwin, 17, 2, "Current Port Number = ");
	mvwprintw(mainwin, 17, 24, str2);
	

	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);
	set_field_back(fields[5], A_UNDERLINE);
	
	form = new_form(fields);
	
	set_form_win(form, childwin);
	set_form_sub(form, derwin(childwin, 10, 76, 1, 1));
	post_form(form);
	
	refresh();
	
	delwin(childwin);
		
	config_destroy(&cfg);
	//free(str4);
	
	return mainwin;
}

static void clear_screen (WINDOW *win){
	wclear(win);
}

static void main_screen(WINDOW *win){
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
    mvwaddstr(childwin, 3, 3, "Edit - Press 'e'");
    mvwaddstr(childwin, 4, 3, "Quit - Press 'q'");
 
    refresh();
	
	delwin(childwin);
	
	return mainwin;
}

int main(){
	
	WINDOW * mainwin = create_Main_Win();
    int ch;

    /*  Loop until user hits 'q' to quit  */
 
   while ( (ch = getch()) != 'q' ) {
 
	switch ( ch ) {
	case 'e':
		reader(mainwin);
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