
#include "gsc_utils.hpp"
#include "include.hpp"
#include <dirent.h> // dir stuff
#include <assert.h>
#include <stdlib.h>

#define MAX_LANGUAGES 16
#define MAX_LANGUAGE_ITEMS 1024

int languages_defined = 0;
int language_items_defined = 0;
char languages[MAX_LANGUAGES][3]; //add space for \0
char *language_items[MAX_LANGUAGE_ITEMS];
char *language_references[MAX_LANGUAGES][MAX_LANGUAGE_ITEMS];
bool language_reference_mallocd[MAX_LANGUAGES][MAX_LANGUAGE_ITEMS];

void gsc_misc_forcedlcvars()
{
	Plugin_Cvar_RegisterBool("cl_allowdownload", qtrue, 0x48, "");
	Plugin_Cvar_RegisterBool("cl_wwwdownload", qtrue, 0x48, "");
}

void gsc_utils_renice()
{
	int renicevalue = Plugin_Scr_GetInt(0);
	char renice[512];
	snprintf(renice, sizeof(renice), "sudo renice %d -p %d", renicevalue, getpid());
	setenv("LD_PRELOAD", "", 1);
	int nope = system(renice);
}

void Gsc_Utils_GetAscii() {
	char *str = Plugin_Scr_GetString(0);

	Plugin_Scr_AddInt(str[0]);
}

void gsc_utils_file_link() {
	char *source = Plugin_Scr_GetString(0);
	char *dest = Plugin_Scr_GetString(1);
	Plugin_Scr_AddInt( link(source, dest) ); // 0 == success
}

void gsc_utils_file_unlink() {
	char *file = Plugin_Scr_GetString(0);
	Plugin_Scr_AddInt( unlink(file) ); // 0 == success
}

void gsc_utils_stringToFloat() {
	char *str = Plugin_Scr_GetString(0);
	Plugin_Scr_AddFloat( atof(str) );
}

void Gsc_Utils_sprintf() {
	char 	result[MAX_STRING_CHARS];
	char 	*str = Plugin_Scr_GetString(0);
	int 	param = 1; // maps to first %
	int 	len = strlen(str);
	int 	num = 0;

	for(int i = 0; i < len; i++) {
		 if(str[i] == '%') {
			if(str[i + 1] == '%') {
				result[num++] = '%';
				i++;
			}
			else {
				if(param >= Plugin_Scr_GetNumParam())
					continue;
				switch(Plugin_Scr_GetType(param)) 
				{
					// 2 - string 5 - float 6 - int
					case 2:
					{
						char *tmp_str = Plugin_Scr_GetString(param);
						num += sprintf(&(result[num]), "%s", tmp_str);
						break;
					}
					case 4:
					{
						vec3_t vec;
						Plugin_Scr_GetVector(param, vec);
						num += sprintf(&(result[num]), "(%.2f, %.2f, %.2f", vec[0], vec[1], vec[2]);
						break;
					}
					case 5:
					{
						float tmp_float = Plugin_Scr_GetFloat(param);
						num += sprintf(&(result[num]), "%.3f", tmp_float); // need a way to define precision
						break;
					}
					case 6:
					{
						int tmp_int = Plugin_Scr_GetInt(param);
						num += sprintf(&(result[num]), "%d", tmp_int);
						break;
					}
				}
				param++;
			}
		}
		else
			result[num++] = str[i];
	}
	result[num] = '\0';
	Plugin_Scr_AddString(result);
}

void gsc_utils_gethex()
{
	char result[9];
	int in = Plugin_Scr_GetInt(0);
	snprintf(result, 9, "%08x", in);
	//Plugin_Printf("%s\n", result);
	
	Plugin_Scr_AddString(result);
}

void Gsc_Utils_FileLink() {
	char *source = Plugin_Scr_GetString(0); 
	char *dest = Plugin_Scr_GetString(1);
	if (Plugin_Scr_GetNumParam() != 2) {
		printf("scriptengine> ERROR: please specify source and dest to gsc_link_file()\n");
		Plugin_Scr_AddUndefined();
		return;
	}
	#ifdef __WIN32
		Plugin_Printf("source=%s dest=%s\n", source, dest);
		Plugin_Printf("result=%i\n", CreateHardLink(source, dest, NULL));
		Plugin_Scr_AddInt( CreateHardLink(source, dest, 0x0) );
	#else 
		Plugin_Scr_AddInt( link(source, dest) );
	#endif
}

void Gsc_Utils_FileUnlink() {
	char *file = Plugin_Scr_GetString(0);
	if (Plugin_Scr_GetNumParam() != 1) {
		printf("scriptengine> ERROR: please specify file to gsc_unlink_file()\n");
		Plugin_Scr_AddUndefined();
		return;
	}
	#ifdef __WIN32
		Plugin_Scr_AddInt( DeleteFile(file) );
	#else
		Plugin_Scr_AddInt( unlink(file) );
	#endif 
}

void Gsc_Utils_FileExists() {
	char *filename = Plugin_Scr_GetString(0);
	if (Plugin_Scr_GetNumParam() != 1) {
		Plugin_Scr_AddUndefined();
		return;
	}
	Plugin_Scr_AddInt( ! (access(filename, F_OK) == -1) );
}

void Gsc_Utils_FS_LoadDir() {
	char *path = Plugin_Scr_GetString(0), *dir = Plugin_Scr_GetString(1);
	if (Plugin_Scr_GetNumParam() != 2) {
		Plugin_Scr_AddUndefined();
		return;
	}
//	Plugin_Printf("path %s dir %s \n", path, dir);
	Plugin_Scr_AddInt( FS_LoadDir(path, dir) );
}

void Gsc_Utils_AddLanguage()
{
	char *str = Plugin_Scr_GetString(0);

	if(str[0] == '\0' || str[1] == '\0' || str[2] != '\0')
	{
		Plugin_Printf("Languages are defined by 2 characters\n");
		Plugin_Scr_AddUndefined();
		return;
	}
	for(int i = 0; i < languages_defined; i++)
	{
		if(!strcmp(languages[i], str))
		{
			Plugin_Printf("%s is already an added language\n", str);
			Plugin_Scr_AddUndefined();
			return;
		}
	}
	if(languages_defined == MAX_LANGUAGES)
	{
		Plugin_Printf("Cannot add another language. Already got %d languages\n", MAX_LANGUAGES);
		Plugin_Scr_AddUndefined();
		return;
	}
	strcpy(languages[languages_defined], str);
	languages_defined++;
	//Plugin_Printf("Added %s as language %d\n", str, languages_defined);
	Plugin_Scr_AddInt(0);
}

void add_lang_item(char* lang, char* item, char* txt)
{
	//Plugin_Printf("adding %s to %s, contents: %s\n", item, lang, txt);
	int language_number = -1;
	for(int i = 0; i < languages_defined; i++)
	{
		if(languages[i][0] == lang[0] && languages[i][1] == lang[1])
		{
			language_number = i;
			break;
		}
	}
	if(language_number == -1)
	{
		Plugin_Printf("Language (%s) not added\n", lang);
		return;
	}
	int language_item_number = language_items_defined;
	for(int i = 0; i < language_items_defined; i++)
	{
		if(!strcmp(language_items[i], item))
		{
			language_item_number = i;
			break;
		}
	}
	if(language_item_number == MAX_LANGUAGE_ITEMS)
	{
		Plugin_Printf("Maximum language items reached\n");
		return;
	}
	bool fill_other_langs = false;
	if(language_item_number == language_items_defined)
	{
//		Plugin_Printf("malloccing item\n");
		char *item_m = (char*)malloc(sizeof(char) * (MAX_STRING_CHARS + 1));
		if(item_m == NULL)
		{
			Plugin_Printf("Could not malloc\n");
			return;
		}
		fill_other_langs = true;
		strncpy(item_m, item, MAX_STRING_CHARS);
		language_items[language_item_number] = item_m;
		language_items_defined++;
		for(int i = 0; i < languages_defined; i++)
			language_reference_mallocd[i][language_item_number] = false;
	}
	char *txt_m;
	if(!language_reference_mallocd[language_number][language_item_number])
	{
//		Plugin_Printf("malloccing text\n");
		txt_m = (char*)malloc(sizeof(char) * (MAX_STRING_CHARS + 1));
		if(txt_m == NULL)
		{
			Plugin_Printf("Could not malloc\n");
			return;
		}
		language_reference_mallocd[language_number][language_item_number] = true;
		language_references[language_number][language_item_number] = txt_m;
	}
	else
	{
		//Plugin_Printf("reusing previous malloc\n");
		txt_m = language_references[language_number][language_item_number];
	}
	strncpy(txt_m, txt, MAX_STRING_CHARS);
	if(fill_other_langs)
	{
		//Plugin_Printf("filling other items\n");
		for(int i = 0; i < languages_defined; i++)
		{
			if(i == language_number)
				continue;
			language_references[i][language_item_number] = txt_m;
		}
	}
}

void Gsc_Utils_LoadLanguages()
{
	static bool loaded = false;
	char *str = Plugin_Scr_GetString(0);
	if(Plugin_Scr_GetType(0) != 2)
	{
		Plugin_Printf("Param 0 needs to be a string for load_languages\n");
		Plugin_Scr_AddUndefined();
		return;
	}

	int force_reload;
//	if(!stackGetParamInt(1, &force_reload))
	if(!Plugin_Scr_GetInt(1))
		force_reload = 0;
	if(!force_reload && loaded)
	{
		Plugin_Printf("Already loaded languages\n");
		//Plugin_Scr_AddUndefined();
		Plugin_Scr_AddUndefined();
		return;
	}
	char curitem[MAX_STRING_CHARS + 1] = "";
	char buffer[MAX_STRING_CHARS + 1];
	bool item_found = false;
	FILE * file;
	file = fopen(str, "r");
	int linenum = 0;
	if(file != NULL)
	{
		while(fgets(buffer, sizeof(buffer), file) != NULL)
		{
			linenum++;
			if(!strncmp(buffer, "REFERENCE", 9))
			{
				//read the rest of buffer, starting from the first non-space character
				int start = -1;
				int end = -1;
				for(int i = 9; i < MAX_STRING_CHARS; i++)
				{
					if(buffer[i] == '\0' || buffer[i] == '\r' || buffer[i] == '\n')
					{
						end = i;
						if(end - start > 0)
						{
							//string has a length
							//set it as curitem
							strncpy(curitem, &(buffer[start]), end - start);
							curitem[end - start] = '\0';
							//Plugin_Printf("Read item: %s", curitem);
							item_found = true;
						}
						break;
					}
					else if(start == -1 && buffer[i] != ' ' && buffer[i] != '\t')
						start = i;
					else if(start != -1 && (buffer[i] == ' ' || buffer[i] == '\t'))
					{
						//error, trailing whitespace
						//try to cut it off
						end = i;
						if(end - start > 0)
						{
							//string has a length
							//set it as curitem
							strncpy(curitem, &(buffer[start]), end - start);
							curitem[end - start] = '\0';
							//Plugin_Printf("Read item: %s", curitem);
							item_found = true;
						}
						break;
					}
				}
			}
			else if(!strncmp(buffer, "LANG_", 5))
			{
				//language is the [5] and [6]th element of this string
				//rest of string, starting at the first " is the string, ending at the last "
				bool lang_exist = false;
				for(int i = 0; i < languages_defined; i++)
				{
					if(languages[i][0] == buffer[5] && buffer[5] != '\0' && languages[i][1] == buffer[6] && buffer[6] != '\0')
					{
						lang_exist = true;
						break;
					}
				}
				if(!lang_exist)
				{
					if(buffer[5] != '\0' && buffer[6] != '\0')
						Plugin_Printf("Language not yet added for language: %c%c\n", buffer[5], buffer[6]);
					else
						Plugin_Printf("Line ended too soon on line %d\n", linenum);
				}
				else
				{
					//start scanning buffer, starting from 8
					char lang[2];
					lang[0] = buffer[5];
					lang[1] = buffer[6];
					int start = -1;
					int end = -1;
					bool ignore_next = false;
					for(int i = 8; i < MAX_STRING_CHARS; i++)
					{
						if(buffer[i] == '\\' && !ignore_next)
						{
							ignore_next = true;
							continue;
						}
						if(buffer[i] == '\0' || buffer[i] == '\r' || buffer[i] == '\n')
						{
							//string ended prematurely
							Plugin_Printf("Error in line %d\n", linenum);
							break;
						}
						if(buffer[i] == '\"' && !ignore_next)
						{
							if(start == -1)
							{
								if(buffer[i + 1] == '\0')
								{
									Plugin_Printf("Premature line end on line %d", linenum);
									break;
								}
								else
									start = i + 1;
							}
							else
							{
								end = i;
								//add buffer to languages stuff
								if(end - start > 0)
								{
									char curdesc[MAX_STRING_CHARS + 1];
									strncpy(curdesc, &(buffer[start]), end - start);
									curdesc[end - start] = '\0';
									add_lang_item(lang, curitem, curdesc);
									//Plugin_Printf("Adding %s as %s for language %c%c\n", curdesc, curitem, buffer[5], buffer[6]);
								}
								break;
							}
						}
						ignore_next = false;
					}
				}
			}
		}
		fclose(file);
	}
	else
	{
		Plugin_Printf("File %s does not exist\n", str);
		return;
	}
}

void Gsc_Utils_GetLanguageItem()
{
	char *str = Plugin_Scr_GetString(0);
	char *str2 = Plugin_Scr_GetString(1);

	//Plugin_Printf("str: %s, str2: %s\n", str, str2);
	if(str[0] == '\0' || str[1] == '\0')
	{
		Plugin_Printf("Invalid language item requested. Should be like EN\n");
		Plugin_Scr_AddUndefined();
		return;
	}
	int language_number = -1;
	for(int i = 0; i < languages_defined; i++)
	{
		if(str[0] == languages[i][0] && str[1] == languages[i][1])
		{
			//found a match
			language_number = i;
			break;
		}
	}
	if(language_number == -1)
	{
		Plugin_Printf("Invalid language selected. Load languages first\n");
		Plugin_Scr_AddUndefined();
		return;
	}
	int language_item_number = -1;
	for(int i = 0; i < language_items_defined; i++)
	{
		if(!strcmp(str2, language_items[i]))
		{
			//found match
			language_item_number = i;
			break;
		}
	}
	if(language_item_number == -1)
	{
		Plugin_Printf("Invalid language item selected. Load language items first\n");
		Plugin_Scr_AddString(str2);
		return;
	}
	//Plugin_Printf("found: %s\n", language_references[language_number][language_item_number]);
	Plugin_Scr_AddString(language_references[language_number][language_item_number]);
}

void  Gsc_Utils_CmdExecuteString() {
	char *str = Plugin_Scr_GetString(0);
    
    if(Plugin_Scr_GetNumParam() != 1)
        Plugin_Scr_Error("Usage: Cmd_ExecString(<command>)\n");

    Cmd_ExecuteString(0, 0, str);
    Plugin_Scr_AddInt(1);
}
/*
void Gsc_Utils_Printf() {
	char *str = Plugin_Scr_GetString(0);

	int param = 1; // maps to first %
	int len = strlen(str);

	for(int i=0; i<len; i++) {
		if(str[i] == '%')
		{
			if(str[i+1] == '%') {
				putchar('%');
				i++;
			} else
				Plugin_Printf("%s", param++);
		}
		else
			putchar(str[i]);
	}
	Plugin_Scr_AddInt(1);
}*/


void Gsc_Utils_Printf() {
	char *str = Plugin_Scr_GetString(0);

	Plugin_Printf("%s", str);
}

void Gsc_Utils_Fopen() {
	char *filename = Plugin_Scr_GetString(0); 
	char *mode = Plugin_Scr_GetString(1);

	FILE *file = fopen(filename, mode);
	Plugin_Scr_AddInt((int)file);
}

void Gsc_Utils_Fread() {
	FILE *file = (FILE*)(Plugin_Scr_GetInt(0));

	assert(file);
	char buffer[256];
	int ret = fread(buffer, 1, 255, file);
	if ( ! ret) {
		Plugin_Scr_AddUndefined();
		return;
	}
	buffer[ret] = '\0';
	Plugin_Scr_AddString(buffer);
}

void Gsc_Utils_Fwrite() {
	FILE *file = (FILE*)(Plugin_Scr_GetInt(0));
	char *buffer = Plugin_Scr_GetString(1);

	assert(file);
	int bytesWritten = fwrite(buffer, 1, strlen(buffer), file);
	Plugin_Scr_AddInt(bytesWritten);
}

void Gsc_Utils_Fclose() {
	FILE *file = (FILE*)(Plugin_Scr_GetInt(0));

	assert(file);
	Plugin_Scr_AddInt( fclose(file) );
}

void Gsc_G_FindConfigstringIndex() {
	char 	*name = Plugin_Scr_GetString(0);
	int 	count = Plugin_Scr_GetInt(1);
	char 	s[MAX_STRING_CHARS];

	for(int i=1; i<count; i++) {
		Plugin_SV_GetConfigstring(i, s, sizeof( s ));
		if(!strcasecmp(s, name)) {
			Plugin_Scr_AddInt(i);
			return;
		}
	}
	Plugin_Scr_AddInt(0);
	return;
}

void Gsc_Utils_SendGameServerCommand() {
	int clientNum = Plugin_Scr_GetInt(0);
	char *message = Plugin_Scr_GetString(1);
	int inputcount = Plugin_Scr_GetNumParam();
	int i = 0;
	while(message[i] != '\0')
	{
		if((unsigned char)message[i] < 0xa)
			message[i] = '?';
		i++;
	}
	if(inputcount == 3)
	{
		int reliable = Plugin_Scr_GetInt(2);
		SV_GameSendServerCommand(clientNum, reliable, message);
	}
	else
		SV_GameSendServerCommand(clientNum, 0, message);
	Plugin_Scr_AddInt(1);
}