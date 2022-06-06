#include <curses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libconfig.h>

int main(void){
	
			config_t cfg;	
	
			config_init(&cfg);
			config_setting_t *items, *root;
			root = config_root_setting(&cfg);
			
				if(!config_read_file(&cfg, "config.cfg")){
					config_destroy(&cfg);
					}
			
				const char* str;
				config_lookup_string(&cfg, "port", str);
				const char* str2;
				config_lookup_string(&cfg, "error_404", str2);
				const char* str3;
				config_lookup_string(&cfg, "document_root", str3);	
				const char* str4;
				config_lookup_string(&cfg, "max_connection", str4);	
				const char* str5;
				config_lookup_string(&cfg, "log_path", str5);
			
			items = config_setting_add(root, "port", CONFIG_TYPE_STRING);
			config_setting_set_string(items, "hello");	
			
			items = config_setting_add(root, "error_404", CONFIG_TYPE_STRING);
			config_setting_set_string(items, str2);

			items = config_setting_add(root, "document_root", CONFIG_TYPE_STRING);
			config_setting_set_string(items, str3);
			
			items = config_setting_add(root, "max_connection", CONFIG_TYPE_STRING);
			config_setting_set_string(items, str4);
			
			items = config_setting_add(root, "log_path", CONFIG_TYPE_STRING);
			config_setting_set_string(items, str5);
			
			 if(! config_write_file(&cfg, "config2.cfg"))
			  {
				fprintf(stderr, "Error while writing file.\n");
				config_destroy(&cfg);
			 }
			
			config_destroy(&cfg);
	
}