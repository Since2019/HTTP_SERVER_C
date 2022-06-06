#ifndef CONFIG_EDITOR_DYNAMIC_H
#define CONFIG_EDITOR_DYNAMIC_H

//This code (mostly the form code) was based on one I found on github, linked below:
//https://gist.github.com/alan-mushi/c8a6f34d1df18574f643
//Note that many other resources were used and cited on sources.txt, which contains
//their URLs.
//Code was changed


//Represents the form object for the editor.
static FORM *form;
//Represents the fields in the form.
static FIELD *fields[17];

//Performs the functions to make a form.
static void form_fill(WINDOW *win);
//Turns the window to the main screen.
static void main_screen(WINDOW *win);
//Creates the main window.
static WINDOW *create_Main_Win();
//Creates the form window.
static WINDOW *create_Form_Win();
//Clears the screen so that a new screen will
//take its' place.
static void clear_screen (WINDOW *win);
//Trims whitespace from a character string.
static char* trim_whitespaces(char *str);
//Trims whitespace from a numerical string.
static char* trim_all_whitespaces(char *str);
//Allows form to perform its' regular functionality.
static void form_Func(WINDOW *win);
//Tells if a string is a number or not.
static bool isNumber(char *str);

#endif