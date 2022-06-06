#include <curses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libconfig.h>
#include "config_editor_dynamic.h"
#include "config_defaults.h"
//Code was changed

static bool isNumber(char *str){
	int length = strlen(str);
	
	for(int i = 0; i < length; i++){
		if(!isdigit(str[i])){
			return false;
		}
	}	
	return true;	
}

/** 
	Trims whitespace from a character string.
	Used for the strings in a field buffer, as extra whitespace
	is applied to them. This should only be used for character strings,
	as some may have internal whitespace, that the user may intend to
	keep (so this only gets rid of leading and trailing whitespace).
	@param str
	@return newString
*/

static char* trim_whitespaces(char *str){
	
	while(*str && isspace(*str)){
		str++;
	}
	
	if(strlen(str) == 0){
		return NULL;
	}
	
	char *end = str + strlen(str) - 1;
	
	while(end >= str && isspace(*end)){
		end--;
	}
	
	*(end + 1) = '\0';
	
	return str;
	
}

/** Trims all whitepace on a char*. This is used for trimming
	values that will be converted to ints, as ints cannot have any
	whitepace (leading, internal, trailing.)
	@param str
	@return newString*/

static char* trim_all_whitespaces(char *str)
{
	//Takes in the length of the string.
	int length = strlen(str);
	
	//Creates a new string without whitespace.
	char * newString = calloc(length, sizeof(char*));
	
	//Represents the index of the new string currently.
	int index = 0;
	
	//For loop that goes through the string, adding all of the
	//characters from the original string to the new one (that aren't whitespace).
	for(int i = 0; i < length; i++){
		if(!isspace(str[i])){
			newString[index++] = str[i];
		}
	}
	
	//If the index is zero
	//release the newString (as it is of no use).
	//Then return NULL
	if(index == 0){
		free(newString);
		return NULL;
	}
	
	//Add null character at the end of the string for good measure.
	newString[index] = '\0';

	//Return the new string.
	return newString;
}


/** 
	This function makes the form operateable. It allows the user
	to fill out the fields of the form, then submit their answers.
	
	However, the user must always fill out all of the fields before proceeding
	(due to an issue with the libconfig write function, which causes all fields that 
	aren't filled out to disappear entirely from the config file if not written to).
	
	If at least one field is not written to, the user will be warned about it, and
	the file will not be written to. 
	
	Although tedious, users who want to maintain the
	data they have, must rewrite it into the form and re-submit it in order for the configurations
	to come out exactly how they'd like.
*/

static void form_Func(WINDOW *win){
	
	//This value represents the next 
	//character written in by the user.
	int nextChar;	
	//Represents the configuration file being used.
	config_t cfg;	
	
	//While loop that keeps reading in the user's input,
	//until they press the '=' character.
	while ((nextChar = getch()) != '='){
		
	switch (nextChar) {
		
		/** If the '-' character is typed, the fields in the form will be read.
			If all of the strings aren't NULL (empty), then the input will be
			put in the config file and displayed on the screen. 
			If at least one is null, the values will not be inputted into the config
			file, and the user will be faced with a warning.*/
		
		case '-':
			//Moves the cursor to the next field.
			form_driver(form, REQ_NEXT_FIELD);
			
			//Moves the cursor to the previous field.
			form_driver(form, REQ_PREV_FIELD);
			
			//All the values in the fields are read in, then checked if they are null.
			char *str = trim_all_whitespaces(field_buffer(fields[1], 0));
			
			char *str2 = trim_whitespaces(field_buffer(fields[3], 0));
			
			char *str3 = trim_whitespaces(field_buffer(fields[5], 0));
			
			char *str4 = trim_all_whitespaces(field_buffer(fields[7], 0));
			
			char *str5 = trim_whitespaces(field_buffer(fields[9], 0));
			
			char *str6 = trim_whitespaces(field_buffer(fields[11], 0));			
			
			char *str7 = trim_whitespaces(field_buffer(fields[13], 0));

			char *str8 = trim_all_whitespaces(field_buffer(fields[15], 0));
			
						//If all strings aren't null, they are processed and written to the config file.
			if((str != NULL) 
			&& (str2 != NULL)
			&& (str3 != NULL)
			&& (str4 != NULL)
			&& (str5 != NULL)
			&& (str6 != NULL)
			&& (str7 !=NULL)
			&& (str8 !=NULL)){
				
				if(isNumber(str) && isNumber(str4) && isNumber(str8)){
					
						int converted = atoi(str);
				
						int converted2 = atoi(str4);
						
						int converted3 = atoi(str8);
					
						if(converted3 == 1 || converted3 == 2){

						mvwprintw(win, 32, 57, "                                    ");
							mvwprintw(win, 32, 2, "                                   ");
							mvwprintw(win, 33, 2, "                                            ");
							mvwprintw(win, 34, 2, "                                            ");
							mvwprintw(win, 35, 4, "                                        ");
						
						config_init(&cfg);
						config_setting_t *items, *root;
						root = config_root_setting(&cfg);
						
						items = config_setting_add(root, "port", CONFIG_TYPE_INT);
						config_setting_set_int(items, converted);	
						
						items = config_setting_add(root, "error_404", CONFIG_TYPE_STRING);
						config_setting_set_string(items, str2);

						items = config_setting_add(root, "server_root", CONFIG_TYPE_STRING);
						config_setting_set_string(items, str3);
						
						items = config_setting_add(root, "max_connections", CONFIG_TYPE_INT);
						config_setting_set_int(items, converted2);
						
						items = config_setting_add(root, "logfile_location", CONFIG_TYPE_STRING);
						config_setting_set_string(items, str5);
						
						items = config_setting_add(root, "server_name", CONFIG_TYPE_STRING);
						config_setting_set_string(items, str6);
						
						items = config_setting_add(root, "default_document", CONFIG_TYPE_STRING);
						config_setting_set_string(items, str7);
						
						items = config_setting_add(root, "server_mode", CONFIG_TYPE_INT);
						config_setting_set_int(items, converted3);
						
						/** Checks if the file write was successful, if not
						an error is thrown.*/
				
						 if(! config_write_file(&cfg, "config.cfg"))
						  {
							fprintf(stderr, "Error while writing file.\n");
							config_destroy(&cfg);
						 } 
						 
						 /** For deleting what text currently resides where the new data will be
						 displayed.*/
					
						mvwprintw(win, 23, 24, "                                                        ");
						
						mvwprintw(win, 24, 21, "                                                  ");
						
						mvwprintw(win, 25, 31, "                                              ");
						
						mvwprintw(win, 26, 31, "                                                 ");
						
						mvwprintw(win, 27, 21, "                                                  ");
						
						mvwprintw(win, 28, 30, "                                            ");
						
						mvwprintw(win, 29, 29, "                                           ");
						
						mvwprintw(win, 30, 24, "                                                        ");
						
						/** The data is displayed, and the strings are freed.*/
					
						mvwprintw(win, 23, 24, str);	
						free(str);
						
						mvwprintw(win, 24, 21, str2);	
						
						mvwprintw(win, 25, 31, str3);	

						mvwprintw(win, 26, 31, str4);	
						free(str4);	

						mvwprintw(win, 27, 21, str5);	

						mvwprintw(win, 28, 24, str6);	
						
						mvwprintw(win, 29, 29, str7);
						
						mvwprintw(win, 30, 24, str8);	
						free(str8);
						
						/** Screen refreshes. */
						
						refresh();
						
						/** Moves cursor back to the location it was before the submission. */
						
						pos_form_cursor(form);
						
						/** Destroys the configuration file ref. */
						
						config_destroy(&cfg);
							
						} else {
							/** Warning that appears at the bottom right of the window,
							if the input is invalid. */
							mvwprintw(win, 32, 57, "INVALID: Mode must be 1 or 2        ");
						}
				} else {
					/** Warning that appears at the bottom right of the window,
					if they fields are filled in.*/
					mvwprintw(win, 32, 57, "INVALID: STRING IS IN INT PARAMETER!");
				}
			} else {
				/** Warning that appears at the bottom right of the window,
				if not all fields are filled in.*/
				mvwprintw(win, 32, 57, "YOU MUST FILL ALL FIELDS!           ");
			}
			
			break;
		/** If the down key is pressed, the cursor moves to the
			next field below it.*/	
		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_END_LINE);
			break;
		/** If the up key is pressed, the cursor moves to the
			next field above it.*/	
		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			form_driver(form, REQ_END_LINE);
			break;
		/** If the left key is pressed, the cursor moves to the
			character to the left of it.*/	
		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;
		/** If the right key is pressed, the cursor moves to the
			character to the right of it.*/	
		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;

		/** When the backspace key is pressed, one of two things
		can occur: 
		1) If the user presses it in a field that has no characters in it,
		the cursor will go to the previous field.
		
		2) If the user presses it in a field that has a character behind
		the cursor, it will delete the character before it.
		*/
		
		case KEY_BACKSPACE:
		case 127:
			form_driver(form, REQ_DEL_PREV);
			break;
			
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;
		/** Any other key that gets pressed will simply become input
		for the field that the user's cursor is in.*/
		default:
			form_driver(form, nextChar);
			break;
	}

	wrefresh(win);
		
	}
	
	/** 
	Unposts the form, then frees all its' fields.
	*/
	
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	free_field(fields[4]);
	free_field(fields[5]);
	free_field(fields[6]);
	free_field(fields[7]);
	free_field(fields[8]);
	free_field(fields[9]);
	free_field(fields[10]);
	free_field(fields[11]);	
	free_field(fields[12]);	
	free_field(fields[13]);
	free_field(fields[14]);	
	free_field(fields[15]);

	/** Deletes the window. */
	
	delwin(win);
	
}

/** This function takes in a window, clears it, then
generates the form screen onto the window.
The form's driver is then activated, allowing the user
to perform certain actions on it. Once the driver is done,
the screen is cleared, and the user can return to the main screen.
	@param win
	*/

static void form_fill(WINDOW *win){
	clear_screen(win);
	win = create_Form_Win();
	wrefresh(win);
	form_Func(win);
	clear_screen(win);
	main_screen(win);
}

/** Takes in a window as a parameter, then clears it.
	@param win
	*/

static void clear_screen (WINDOW *win){
	wclear(win);
}

/** Takes in a window as a parameter, then 
	creates the main window on it.
	@param win*/

static void main_screen(WINDOW *win){
	win = create_Main_Win();
	wrefresh(win);
}

/** Creates the form window, 
	@return mainwin
	*/

static WINDOW *create_Form_Win(){
	
	//Configuration file reference.
	config_t cfg;
	
	//Initializes the configuration reference.
	config_init(&cfg);
	
	
	//Confirms if the file read is successful.
	//If not, configuration is destroyed.
	if(!config_read_file(&cfg, "config.cfg")){
		config_destroy(&cfg);
		
					/** 
		Strings are assigned the values of the
		fields in the configuration file.
	*/
	int portnum = DEFAULT_PORT;
	int maxnum = DEFAULT_MAX_CONNECTIONS;
	int procnum = DEFAULT_SERVER_MODE;
	char* str2 = calloc(strlen(DEFAULT_404), sizeof(char*));
	strcpy(str2, DEFAULT_404);
	char* str3 = calloc(strlen(DEFAULT_SERVER_ROOT), sizeof(char*));;
	strcpy(str3, DEFAULT_SERVER_ROOT);
	char* str5 = calloc(strlen(DEFAULT_LOG_FILE), sizeof(char*));;
	strcpy(str5, DEFAULT_LOG_FILE);
	char* str6 = calloc(strlen(DEFAULT_SERVER_NAME), sizeof(char*));;
	strcpy(str6, DEFAULT_SERVER_NAME);
	char* str7 = calloc(strlen(DEFAULT_DOCUMENT), sizeof(char*));;
	strcpy(str7, DEFAULT_DOCUMENT);
	
	/** 
		The main window, and the child window of the
		screen are made.
	*/
	
	WINDOW *mainwin, *childwin;
	
	mainwin = newwin(39, 96, 0, 0);
	
	childwin = derwin(mainwin, 28, 86, 4, 1);
	box(childwin, 0, 0);
	
	//Title of window.
	mvwprintw(mainwin, 1, 30, "Configuration Options");
	
	mvwprintw(mainwin, 32, 2, "WARNING: 'config.cfg' was not found");
	mvwprintw(mainwin, 33, 2, "-Default values are being displayed instead.");
	mvwprintw(mainwin, 34, 2, "-Submitting this current form will result in");
	mvwprintw(mainwin, 35, 4, "the creation of a new 'config.cfg' file ");
	
	/** Fields are made.*/
	
	fields[0] = new_field(1, 30, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 25, 0, 0);
	fields[2] = new_field(1, 30, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 25, 0, 0);
	fields[4] = new_field(1, 30, 4, 0, 0, 0);
	fields[5] = new_field(1, 30, 4, 25, 0, 0);
	fields[6] = new_field(1, 30, 6, 0, 0, 0);
	fields[7] = new_field(1, 30, 6, 25, 0, 0);
	fields[8] = new_field(1, 30, 8, 0, 0, 0);
	fields[9] = new_field(1, 30, 8, 25, 0, 0);
	fields[10] = new_field(1, 30, 10, 0, 0, 0);
	fields[11] = new_field(1, 30, 10, 25, 0, 0);	
	fields[12] = new_field(1, 30, 12, 0, 0, 0);
	fields[13] = new_field(1, 30, 12, 25, 0, 0);
	fields[14] = new_field(1, 30, 14, 0, 0, 0);
	fields[15] = new_field(1, 30, 14, 25, 0, 0);
	fields[16] = NULL;
	
	
	/** Buffer is set for each field. */
	
	set_field_buffer(fields[0], 0, "Port Number: ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "404 File: ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "Server Root Folder: ");
	set_field_buffer(fields[5], 0, "");
	set_field_buffer(fields[6], 0, "Maximum Connection: ");
	set_field_buffer(fields[7], 0, "");
	set_field_buffer(fields[8], 0, "Log File: ");
	set_field_buffer(fields[9], 0, "");
	set_field_buffer(fields[10], 0, "Server Name: ");
	set_field_buffer(fields[11], 0, "");	
	set_field_buffer(fields[12], 0, "Default Document: ");
	set_field_buffer(fields[13], 0, "");
	set_field_buffer(fields[14], 0, "Threads(1)/Processes(2): ");
	set_field_buffer(fields[15], 0, "");

	/** Field options are set, some are treated as labels, while 
	some are treated as input fields.*/
	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);	
	set_field_opts(fields[6], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[7], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[8], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[9], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);	
	set_field_opts(fields[10], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[11], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[12], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[13], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[14], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[15], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	/** 
		All the current values for the fields are displayed at the bottom of
		the window.
	*/
	
	char buf[30];
	sprintf(buf, "%d", portnum);
	char buf2[30];
	sprintf(buf2, "%d", maxnum);
	char buf3[30];
	sprintf(buf3, "%d", procnum);
	
	mvwprintw(mainwin, 3, 16, "(Press '=' to go back, and '-' to accept changes.)");
	mvwprintw(mainwin, 23, 2, "Current Port Number = ");
	mvwprintw(mainwin, 23, 24, buf);
	mvwprintw(mainwin, 24, 2, "Current 404 File = ");
	mvwprintw(mainwin, 24, 21, str2);
	mvwprintw(mainwin, 25, 2, "Current Server Root Folder = ");
	mvwprintw(mainwin, 25, 31, str3);
	mvwprintw(mainwin, 26, 2, "Current Maximum Connection = ");
	mvwprintw(mainwin, 26, 31, buf2);	
	mvwprintw(mainwin, 27, 2, "Current Log File = ");
	mvwprintw(mainwin, 27, 21, str5);
	mvwprintw(mainwin, 28, 2, "Current Server Name = ");
	mvwprintw(mainwin, 28, 24, str6);
	mvwprintw(mainwin, 29, 2, "Current Default Document = ");
    mvwprintw(mainwin, 29, 29, str7);
	mvwprintw(mainwin, 30, 2, "Current Server Mode = ");
    mvwprintw(mainwin, 30, 24, buf3);
	
	/** Input fields are underlined. */
	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);
	set_field_back(fields[5], A_UNDERLINE);
	set_field_back(fields[7], A_UNDERLINE);
	set_field_back(fields[9], A_UNDERLINE);
	set_field_back(fields[11], A_UNDERLINE);
	set_field_back(fields[13], A_UNDERLINE);
	set_field_back(fields[15], A_UNDERLINE);
	
	free(str2);
	free(str3);
	free(str5);
	free(str6);
	free(str7);
	
	/** The form is created with all of its' fields.*/
	form = new_form(fields);
	
	/** The form is placed in the child window.*/
	set_form_win(form, childwin);
	set_form_sub(form, derwin(childwin, 16, 76, 1, 1));
	/** Form is posted. */
	post_form(form);
	
	/** Window is refreshed. */
	refresh();
	
	/** Child window is not used anymore,
	so it can be deleted (saves memory).*/
	delwin(childwin);
	
	/** main window is returned. */
	return mainwin;
	
	} else {
			/** 
		Strings are assigned the values of the
		fields in the configuration file.
	*/
	int portnum;
	int maxnum;
	config_lookup_int(&cfg, "port", &portnum);
	const char* str2;
	config_lookup_string(&cfg, "error_404", &str2);
	const char* str3;
	config_lookup_string(&cfg, "server_root", &str3);	
	config_lookup_int(&cfg, "max_connections", &maxnum);
	const char* str5;
	config_lookup_string(&cfg, "logfile_location", &str5);
	const char* str6;
	config_lookup_string(&cfg, "server_name", &str6);
	const char* str7;
	config_lookup_string(&cfg, "default_document", &str7);
	int procnum;
	config_lookup_int(&cfg, "server_mode", &procnum);
	
	/** 
		The main window, and the child window of the
		screen are made.
	*/
	
	WINDOW *mainwin, *childwin;
	
	mainwin = newwin(39, 96, 0, 0);
	
	childwin = derwin(mainwin, 28, 86, 4, 1);
	box(childwin, 0, 0);
	
	//Title of window.
	mvwprintw(mainwin, 1, 30, "Configuration Options");
	
	/** Fields are made.*/
	
	fields[0] = new_field(1, 30, 0, 0, 0, 0);
	fields[1] = new_field(1, 30, 0, 25, 0, 0);
	fields[2] = new_field(1, 30, 2, 0, 0, 0);
	fields[3] = new_field(1, 30, 2, 25, 0, 0);
	fields[4] = new_field(1, 30, 4, 0, 0, 0);
	fields[5] = new_field(1, 30, 4, 25, 0, 0);
	fields[6] = new_field(1, 30, 6, 0, 0, 0);
	fields[7] = new_field(1, 30, 6, 25, 0, 0);
	fields[8] = new_field(1, 30, 8, 0, 0, 0);
	fields[9] = new_field(1, 30, 8, 25, 0, 0);
	fields[10] = new_field(1, 30, 10, 0, 0, 0);
	fields[11] = new_field(1, 30, 10, 25, 0, 0);	
	fields[12] = new_field(1, 30, 12, 0, 0, 0);
	fields[13] = new_field(1, 30, 12, 25, 0, 0);
	fields[14] = new_field(1, 30, 14, 0, 0, 0);
	fields[15] = new_field(1, 30, 14, 25, 0, 0);
	fields[16] = NULL;
	
	
	/** Buffer is set for each field. */
	
	set_field_buffer(fields[0], 0, "Port Number: ");
	set_field_buffer(fields[1], 0, "");
	set_field_buffer(fields[2], 0, "404 File: ");
	set_field_buffer(fields[3], 0, "");
	set_field_buffer(fields[4], 0, "Server Root Folder: ");
	set_field_buffer(fields[5], 0, "");
	set_field_buffer(fields[6], 0, "Maximum Connection: ");
	set_field_buffer(fields[7], 0, "");
	set_field_buffer(fields[8], 0, "Log File: ");
	set_field_buffer(fields[9], 0, "");
	set_field_buffer(fields[10], 0, "Server Name: ");
	set_field_buffer(fields[11], 0, "");	
	set_field_buffer(fields[12], 0, "Default Document: ");
	set_field_buffer(fields[13], 0, "");
	set_field_buffer(fields[14], 0, "Threads(1)/Processes(2): ");
	set_field_buffer(fields[15], 0, "");

	/** Field options are set, some are treated as labels, while 
	some are treated as input fields.*/
	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);	
	set_field_opts(fields[6], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[7], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[8], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[9], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);	
	set_field_opts(fields[10], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[11], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[12], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[13], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[14], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[15], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	/** 
		All the current values for the fields are displayed at the bottom of
		the window.
	*/
	
	char buf[30];
	sprintf(buf, "%d", portnum);
	char buf2[30];
	sprintf(buf2, "%d", maxnum);
	char buf3[30];
	sprintf(buf3, "%d", procnum);
	
	mvwprintw(mainwin, 3, 16, "(Press '=' to go back, and '-' to accept changes.)");
	mvwprintw(mainwin, 23, 2, "Current Port Number = ");
	mvwprintw(mainwin, 23, 24, buf);
	mvwprintw(mainwin, 24, 2, "Current 404 File = ");
	mvwprintw(mainwin, 24, 21, str2);
	mvwprintw(mainwin, 25, 2, "Current Server Root Folder = ");
	mvwprintw(mainwin, 25, 31, str3);
	mvwprintw(mainwin, 26, 2, "Current Maximum Connection = ");
	mvwprintw(mainwin, 26, 31, buf2);	
	mvwprintw(mainwin, 27, 2, "Current Log File = ");
	mvwprintw(mainwin, 27, 21, str5);
	mvwprintw(mainwin, 28, 2, "Current Server Name = ");
	mvwprintw(mainwin, 28, 24, str6);
	mvwprintw(mainwin, 29, 2, "Current Default Document = ");
    mvwprintw(mainwin, 29, 29, str7);
	mvwprintw(mainwin, 30, 2, "Current Server Mode = ");
    mvwprintw(mainwin, 30, 24, buf3);
	
	/** Input fields are underlined. */
	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);
	set_field_back(fields[5], A_UNDERLINE);
	set_field_back(fields[7], A_UNDERLINE);
	set_field_back(fields[9], A_UNDERLINE);
	set_field_back(fields[11], A_UNDERLINE);
	set_field_back(fields[13], A_UNDERLINE);
	set_field_back(fields[15], A_UNDERLINE);
	
	/** The form is created with all of its' fields.*/
	form = new_form(fields);
	
	/** The form is placed in the child window.*/
	set_form_win(form, childwin);
	set_form_sub(form, derwin(childwin, 16, 76, 1, 1));
	/** Form is posted. */
	post_form(form);
	
	/** Window is refreshed. */
	refresh();
	
	/** Child window is not used anymore,
	so it can be deleted (saves memory).*/
	delwin(childwin);
		
	/** Config reference is destroyed.*/	
	config_destroy(&cfg);
	
	/** main window is returned. */
	return mainwin;
	}
}

/** 
	Creates the main window for the program.
	Creates a single box with the options written in.
	@return mainwin.
*/

static WINDOW *create_Main_Win(){
	
	/** Creates the main and child windows. */
	
    WINDOW * mainwin, * childwin;
	 
    /** 
	These are the dimensions and rows/columns
	for the child window.
	*/
 
    int      width = 30, height = 7;
    int      rows  = 25, cols   = 80;
    int      x = (cols - width)  / 2;
    int      y = (rows - height) / 2;
 
    /**  Ncurses is initialized. If the function returns NULL
		Then an error is printed on the screen.*/
 
    if ( (mainwin = initscr()) == NULL ) {
	fprintf(stderr, "Error initialising ncurses.\n");
	exit(EXIT_FAILURE);
    }
 
    noecho();
    keypad(mainwin, TRUE);
 
    /**  The child window is initialized,
		then a box is placed around it.
		Text is printed inside the child window.
		*/
 
    childwin = subwin(mainwin, height, width, y, x);
    box(childwin, 0, 0);
    mvwaddstr(childwin, 1, 8, "Select  a Mode:");
    mvwaddstr(childwin, 3, 7, "Edit - Press '1'");
    mvwaddstr(childwin, 4, 7, "Quit - Press '2'");
 
	/** Refreshes the window. */
    refresh();
	
	/** Deletes the child window, which frees up space.*/
	delwin(childwin);
	
	/** Returns the main window.*/
	return mainwin;
	}


int main(void){
	
	/** The main window is initialized to the main window by
	default.*/
	WINDOW *mainwin = create_Main_Win();
	
	/** Represents the character typed by the user.*/
	int ch;
 
	/** While statement that keeps going until 'q' is pressed.
	If 'e' is pressed, the user is sent to the form window.*/
	
   while ( (ch = getch()) != '2') {
 
	switch ( ch ) {
	case '1':
		form_fill(mainwin);
	    break;
		
	default:
		beep();
		refresh();
		break;
		}
    }
	
	/** The window is deleted once 'q' is pressed.
	The window is ended, and the screen is refreshed.*/
 
    delwin(mainwin);
    endwin();
    refresh();
	
}
