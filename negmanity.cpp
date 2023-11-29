//C
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
//C++
#include <map>
#include <vector>
#include <string>

//			Structs
struct Archivo {
	unsigned short current_line = 0;
	unsigned short current_char = 0;
	unsigned short positiony = 0;
	unsigned short lines = 0;
	char name[200] = "New File";
	bool new_file;
	std::vector<std::string> text;
};

//			Variables
//		GUI
unsigned short x, y, key = 0, pressed = 0;
bool title_bar = true, line_numbers = true, quit = false, searching = false, showing_status = false, saving = false, savingas = false, bold[4] = {false, false, false, false};
unsigned short color1[4] = {55, 55, 55, 0}, color2[4] = {55, 55, 55, 0};

//		Teclas
//	Cursor
char up[20] = "KEY_UP", down[20] = "KEY_DOWN", left[20] = "KEY_LEFT", right[20] = "KEY_RIGHT", quit_var[20] = "^X";
unsigned short position = 0;
const char *enter = "^J", *borrar = "KEY_BACKSPACE";
//	Archivos
char open[20] = "^O", rightf[20] = "kRIT5", leftf[20] = "kLFT5", close[20] = "^Q", key_new[20] = "^N", save[20] = "^U", save_as[20] = "^A";
//	Edición
char copy[20] = "^C", paste[20] = "^V", undo[20] = "^Z", redo[20] = "^Y";
//	Ejecución
char build[30] = "^B", run[30] = "^R", terminal[100] = "x-terminal-emulator";
//		Archivos
unsigned short current_file = 0, total_files = 0;
char newfile[300], status[300], name_saving[300];
std::map <unsigned short, Archivo> files;

//			Funciones
void gui();
//		Colores
void create_color(unsigned short index, const char *token){
	bool second = false;
	bool iscolor = false;
	char read[30];
	size_t color_index = 0;
    
	for (size_t i = 0; i <= strlen(token); i++){
		if (token[i] != ',' && token[i] != '\0') {
			read[color_index++] = token[i];
		}
		else {
			read[color_index] = '\0';
			unsigned short c = 0;
			if (strcmp(read, "black") == 0) {c = 0; iscolor = true;}
			else if (strcmp(read, "red") == 0) {c = 1; iscolor = true;}
			else if (strcmp(read, "green") == 0) {c = 2; iscolor = true;}
			else if (strcmp(read, "yellow") == 0) {c = 3; iscolor = true;}
			else if (strcmp(read, "blue") == 0) {c = 4; iscolor = true;}
			else if (strcmp(read, "magenta") == 0) {c = 5; iscolor = true;}
			else if (strcmp(read, "cyan") == 0) {c = 6; iscolor = true;}
			else if (strcmp(read, "white") == 0) {c = 7; iscolor = true;}
			
			if (index != 4){
				if (iscolor){
					if (!second) {color1[index - 1] = c; second = true;}
					else {color2[index - 1] = c;}
					iscolor = false;
				}
				else if (strcmp(read, "bold") == 0) bold[index - 1] = true;
				memset(read, '\0', sizeof(read));
				color_index = 0;
			}
			else{
				{color1[3] = 0; color2[3] = c; break;} 
			}
		}
	}
	if (color2[index - 1] == 55) init_pair(index, color1[index - 1], color2[3]);
	else init_pair(index, color1[index - 1], color2[index - 1]);
}

void activar(const unsigned short color){
	if (bold[color - 1]) attron(A_BOLD);
	attron(COLOR_PAIR(color));
}

void desactivar(const unsigned short color){
	if (bold[color - 1]) attroff(A_BOLD);
	attroff(COLOR_PAIR(color));
}

//		Pantalla
void clear_screen(){
	activar(4);
	for (int posy = 0; posy != y; posy++) {
		for (int posx = 0; posx != x; posx++) mvaddch(posy, posx, ' ');
	}
	desactivar(4);
}

void message(const char *show, ...){
	showing_status = true;
	sprintf(status, show);
}

//		Archivos
void create_new_file(bool initialized){
	files[total_files].text.push_back("");
	files[total_files].lines = 1;
	files[total_files].new_file = true;
	total_files++;
	if (!initialized) {current_file = total_files - 1; message("New file created.");}
}

void open_file(const char *path, bool initialized){
	FILE *openfile;
	openfile = fopen(path, "r");
	if (openfile != NULL){
		char *linea; size_t length = 0;
		while (getline(&linea, &length, openfile) != -1){
			if (linea[strlen(linea) - 1] == '\n') {
				linea[strlen(linea) - 1] = '\0';
			}
			files[total_files].text.push_back(linea);
			files[total_files].lines++;
		}
		files[total_files].new_file = false;
		sprintf(files[total_files].name, path);
		total_files++;
		if (!initialized){current_file = total_files - 1; char filename[300]; sprintf(filename, "%s opened succesfully.", files[current_file].name); message(filename);}
		free(linea);
		fclose(openfile);
	}
	else {
		files[total_files].new_file = false;
		files[total_files].text.push_back("");
		files[total_files].lines = 0;
		sprintf(files[total_files].name, path);
		total_files++;
	}
}

void search_file(const char tecla[30]){
	clear_screen();
	if (strcmp(tecla, quit_var) == 0) {searching = false; memset(newfile, '\0', sizeof(newfile));}
	else if (strcmp(tecla, enter) == 0) {searching = false; open_file(newfile, false);}
	else if (strcmp(tecla, borrar) == 0) newfile[strlen(newfile) - 1] = '\0';
	else strncat(newfile, tecla, sizeof(newfile) - strlen(newfile) - 1);
	gui();
}

void save_new_file(){
	FILE *file;
	file = fopen(name_saving, "w");
	if (file != NULL) {
		char filename[300];
		sprintf(files[current_file].name, name_saving);
		files[current_file].new_file = false;
		for (const auto &line : files[current_file].text) {fprintf(file, "%s\n", line.c_str());}
		fclose(file);
		sprintf(filename, "%s created and saved succesfully.", files[current_file].name);
		message(filename);
	}
	else message("Error saving the new file.");
	memset(name_saving, '\0', sizeof(name_saving));
}

void save_file_as(){
	FILE *file;
	file = fopen(name_saving, "w");
	if (file != NULL){
		char filename[400];
		if (files[current_file].new_file) {sprintf(files[current_file].name, name_saving); files[current_file].new_file = false;}
		for (const auto &line : files[current_file].text) {fprintf(file, "%s\n", line.c_str());}
		fclose(file);
		sprintf(filename, "%s created and saved succesfully.", name_saving);
		message(filename);
	}
	else message("Error saving the file.");
	memset(name_saving, '\0', sizeof(name_saving));
}

void getting_name(const char tecla[30]){
	clear_screen();
	if (saving){
		if (strcmp(tecla, quit_var) == 0) {saving = false; memset(newfile, '\0', sizeof(name_saving));}
		else if (strcmp(tecla, enter) == 0) {saving = false; save_new_file();}
		else if (strcmp(tecla, borrar) == 0) name_saving[strlen(name_saving) - 1] = '\0';
		else strncat(name_saving, tecla, sizeof(name_saving) - strlen(name_saving) - 1);
	}
	else if (savingas){
		if (strcmp(tecla, quit_var) == 0) {saving = false; memset(newfile, '\0', sizeof(name_saving));}
		else if (strcmp(tecla, enter) == 0) {saving = false; save_file_as();}
		else if (strcmp(tecla, borrar) == 0) name_saving[strlen(name_saving) - 1] = '\0';
		else strncat(name_saving, tecla, sizeof(name_saving) - strlen(name_saving) - 1);
	}
	gui();
}

void close_file(){
	showing_status = true; sprintf(status, "%s closed.", files[current_file].name);
	if (current_file == 0) {for (int i = 0; i != total_files; i++) files[i] = files[i + 1]; files.erase(total_files);}
	else {for (int i = 0; i != total_files; i++) {if (i >= 0) files[i] = files[i]; else files[i] = files[i + 1];} files.erase(current_file); current_file--;}
	total_files--;
	clear_screen();
	gui();
}

void save_file(){
	if (files[current_file].new_file) saving = true;
	else {
		FILE *file;
		file = fopen(files[current_file].name, "w");
		if (file != NULL) {
			for (const auto &line : files[current_file].text) {fprintf(file, "%s\n", line.c_str());}
			fclose(file);
		}
		char filename[300];
		sprintf(filename, "%s saved.", files[current_file].name);
		message(filename);
	}
}

void edit_file(const char *chars){
	unsigned short length = files[current_file].text[files[current_file].current_line].length() + strlen(chars);
	char content[length];
	
	//Pegar
	strncpy(content, files[current_file].text[files[current_file].current_line].c_str(), files[current_file].current_char);
	strcpy(content + files[current_file].current_char, chars);
	strcpy(content + files[current_file].current_char + strlen(chars), files[current_file].text[files[current_file].current_line].c_str() + files[current_file].current_char);
    
    //Actualizar
    files[current_file].text[files[current_file].current_line] = content;
    files[current_file].current_char = files[current_file].current_char + strlen(chars);
    
    //Mostrar
    clear_screen();
    gui();
}

void remove_char(){
	const unsigned short length = files[current_file].text[files[current_file].current_line].length();	
	if (length > 0 && files[current_file].current_char != 0){
		char content[length];
		for (int i = 0, j = 0; i < length; i++) {
			if (i != files[current_file].current_char - 1) {
				content[j] = files[current_file].text[files[current_file].current_line][i];
				j++;
			}
		}
		content[length - 1] = '\0';
		files[current_file].text[files[current_file].current_line] = content;
		files[current_file].current_char--;
	}
	else if (files[current_file].current_line != 0){
		//Concatenar
		if (length > 0) {
			std::string content;
			content = files[current_file].text[files[current_file].current_line].substr(files[current_file].current_char, length);
			files[current_file].text[files[current_file].current_line - 1] += content;
		}
		//Reacomodar
		files[current_file].text.erase(files[current_file].text.begin() + files[current_file].current_line);
		files[current_file].current_char = files[current_file].text[files[current_file].current_line - 1].length(); 
		files[current_file].current_line--;
		files[current_file].lines--;
	}
	clear_screen();
	gui();
}

void add_new_line() {
	const unsigned short length = files[current_file].text[files[current_file].current_line].length();

    if (length == 0) {files[current_file].text.insert(files[current_file].text.begin() + files[current_file].current_line + 1, "");}
    else {
		std::string new_line = files[current_file].text[files[current_file].current_line].substr(files[current_file].current_char);
		files[current_file].text[files[current_file].current_line] = files[current_file].text[files[current_file].current_line].substr(0, files[current_file].current_char);
		files[current_file].text.insert(files[current_file].text.begin() + files[current_file].current_line + 1, new_line);
	}
	files[current_file].current_line++;
	files[current_file].current_char = 0;
	files[current_file].lines++;
	
	clear_screen();
	gui();
}


void right_file(){
	if(current_file < total_files - 1) current_file++;
}

void left_file(){
	if(current_file > 0) current_file--;
}

//		Construír/Ejecutar
void build_file(){
	char comando[600];
	sprintf(comando, "%s -e sh runner.sh %s build", terminal, files[current_file].name);
	system(comando);
}

void run_file(){
	char comando[600];
	sprintf(comando, "%s -e sh runner.sh %s run", terminal, files[current_file].name);
	system(comando);
}

//		Teclas
void teclas(const unsigned short key){
	char tecla[30];
	sprintf(tecla, keyname(key));
	if (searching == false && saving == false && savingas == false){
		//	Cursor
		//Izquierda
		if (strcmp(tecla, left) == 0){
			if (files[current_file].current_char == 0 && files[current_file].current_line != 0) {files[current_file].current_line--; files[current_file].current_char = files[current_file].text[files[current_file].current_line].length();}
			else if (files[current_file].current_char != 0) files[current_file].current_char--;
		}
		//Derecha
		else if (strcmp(tecla, right) == 0){
			if (files[current_file].current_char == files[current_file].text[files[current_file].current_line].length() && files[current_file].current_line != files[current_file].lines) {files[current_file].current_char = 0; files[current_file].current_line++;}
			else if (files[current_file].current_char != files[current_file].text[files[current_file].current_line].length()) files[current_file].current_char++;
		}
		//Arriba
		else if (strcmp(tecla, up) == 0){
			if (files[current_file].current_line != 0){
				if (files[current_file].current_char <= files[current_file].text[files[current_file].current_line - 1].length()) {files[current_file].current_line--;}
				else {files[current_file].current_line--; files[current_file].current_char = files[current_file].text[files[current_file].current_line].length();}
				if (files[current_file].current_line < files[current_file].positiony && files[current_file].lines > y) {files[current_file].positiony--; clear_screen(); gui();}
			}
		}
		//Abajo
		else if (strcmp(tecla, down) == 0){
			if (files[current_file].current_line != files[current_file].lines) {
				if (files[current_file].current_char <= files[current_file].text[files[current_file].current_line + 1].length()) {files[current_file].current_line++;}
				else {files[current_file].current_line++; files[current_file].current_char = files[current_file].text[files[current_file].current_line].length();}
				if (y - (files[current_file].current_line + 2) < files[current_file].positiony && files[current_file].lines > y) {files[current_file].positiony++; clear_screen(); gui();}
			}
		}
		//	Archivos
		else if (strcmp(tecla, open) == 0) {searching = true; memset(newfile, '\0', sizeof(newfile));}
		else if (strcmp(tecla, save) == 0) save_file();
		else if (strcmp(tecla, save_as) == 0) savingas = true;
		else if (strcmp(tecla, close) == 0) close_file();
		else if (strcmp(tecla, rightf) == 0) {right_file(); clear_screen();}
		else if (strcmp(tecla, leftf) == 0) {left_file(); clear_screen();}
		else if (strcmp(tecla, key_new) == 0) {clear_screen(); create_new_file(false); gui();}
		//	Edición
		else if (strcmp(tecla, borrar) == 0) remove_char();
		else if (strcmp(tecla, enter) == 0) add_new_line();
		//	Build/Run
		else if (strcmp(tecla, run) == 0) run_file();
		else if (strcmp(tecla, build) == 0) build_file();
		else if (strcmp(tecla, quit_var) == 0) quit = true;
		else edit_file(tecla);
	}
	else if (searching == true && saving == false && savingas == false){
		search_file(tecla);
	}
	else if (saving == true) getting_name(tecla);
	else if (savingas == true) getting_name(tecla);
}

//		GUI
//	Cursor
void show_cursor(const unsigned short adelantar){
	/*if (files[current_file].positiony < files[current_file].current_line) wmove(stdscr, files[current_file].current_line + title_bar + files[current_file].positiony, files[current_file].current_char + adelantar);
	else if (files[current_file].positiony == files[current_file].current_line - 1) wmove(stdscr, files[current_file].current_line + title_bar - files[current_file].positiony, files[current_file].current_char + adelantar);
	else wmove(stdscr, files[current_file].current_line + title_bar, files[current_file].current_char + adelantar);*/
	wmove(stdscr, files[current_file].current_line + title_bar, files[current_file].current_char + adelantar);
	wrefresh(stdscr);
}

//	Título
void title(){
	activar(1);
	for (int i = 0; i != x; i++) mvaddch(0, i, ' ');
	mvprintw(0, (x / 2) - (strlen(files[current_file].name) / 2), "%s", files[current_file].name);
	if (total_files > 1) mvprintw(0, 2, "[%d/%d]", current_file + 1, total_files);
	desactivar(1);
}

//	Estado
void show_status(){
	activar(3);
	mvprintw(y - 4, x / 2 - (strlen(status) / 2), "%s", status); 
	desactivar(3);
}

//	Números de línea
unsigned short show_lines(const unsigned lineas, const unsigned short principio){
	unsigned short digitos = lineas, contador = 0;
	while (digitos > 0){
		digitos = digitos / 10;
		contador++;
	}
	char caracteres[contador];
	for (int i = 0; i != contador; i++) caracteres[i] = ' ';
	caracteres[contador] = '\0';
	activar(2);
	for (unsigned short i = principio; i != lineas + 1; i++) {
		mvprintw(i + title_bar - principio, 0, caracteres);
		mvprintw(i + title_bar - principio, contador, "|");
		mvprintw(i + title_bar - principio, 0, "%d", i + position + 1);
	}
	desactivar(2);
	return contador + 1;
}

//	Mostrar archivo
void insert_content(const unsigned short minusx){
	const unsigned short lineas = files[current_file].lines;
	activar(5);
	for(int i = 0; i != lineas; i++){
		mvprintw(i + title_bar, minusx, "%s", files[current_file].text[i + files[current_file].positiony].c_str());
	}
	desactivar(5);
}

//	Mostrar
void gui(){
	unsigned short minusx = 0;
	if (title_bar == 1) title();
	if (line_numbers == 1) minusx = show_lines(files[current_file].lines, files[current_file].positiony);
	insert_content(minusx);
	if (showing_status) show_status();
	show_cursor(minusx);
	if (searching) {activar(3); for (int i = 0; i != x; i++) mvaddch(y - 1, i, ' '); mvprintw(y - title_bar, 0, "Open: %s", newfile); desactivar(3);}
	if (saving) {activar(3); for (int i = 0; i != x; i++) mvaddch(y - 1, i, ' '); mvprintw(y - title_bar, 0, "Save as: %s", name_saving); desactivar(3);}
	refresh();
}

//			Configuración
void load_config(){
	FILE *config;
	char line[200], variable[100], valor[100];
	
	config = fopen("negmanity.conf", "r");
	if (config != NULL){
		while (fgets(line, sizeof(line), config) != NULL){
			if (line[0] == '#') continue;
			if (sscanf(line, "%s = %s", variable, valor) == 2){
				
				//Configuración
				if (strcmp(variable, "title_bar") == 0) title_bar = atoi(valor);
				else if (strcmp(variable, "line_numbers") == 0) line_numbers = atoi(valor);
				
				//Cursor
				else if (strcmp(variable, "up") == 0) sprintf(up, valor);
				else if (strcmp(variable, "down") == 0) sprintf(down, valor);
				else if (strcmp(variable, "left") == 0) sprintf(left, valor);
				else if (strcmp(variable, "right") == 0) sprintf(right, valor);
				
				//Construir
				else if (strcmp(variable, "build") == 0) sprintf(build, valor);
				else if (strcmp(variable, "run") == 0) sprintf(run, valor);
				
				//Archivos
				else if (strcmp(variable, "open_file") == 0) sprintf(open, valor);
				else if (strcmp(variable, "save_file") == 0) sprintf(save, valor);
				else if (strcmp(variable, "close_file") == 0) sprintf(close, valor);
				else if (strcmp(variable, "right_file") == 0) sprintf(rightf, valor);
				else if (strcmp(variable, "left_file") == 0) sprintf(leftf, valor);
				else if (strcmp(variable, "save_as_file") == 0) sprintf(save_as, valor);
				
				//Edición
				else if (strcmp(variable, "copy") == 0) sprintf(copy, valor);
				else if (strcmp(variable, "paste") == 0) sprintf(paste, valor);
				else if (strcmp(variable, "undo") == 0) sprintf(undo, valor);
				else if (strcmp(variable, "redo") == 0) sprintf(redo, valor);
				
				//Colores
				else if (strcmp(variable, "title_colors") == 0) create_color(1, valor);
				else if (strcmp(variable, "lines_colors") == 0) create_color(2, valor);
				else if (strcmp(variable, "status_colors") == 0) create_color(3, valor);
				else if (strcmp(variable, "background_color") == 0) create_color(4, valor);
				else if (strcmp(variable, "file_content_color") == 0) create_color(5, valor);
				
				//Otros
				else if (strcmp(variable, "exit") == 0) sprintf(quit_var, valor);
				else if (strcmp(variable, "terminal") == 0) sprintf(terminal, valor);
			}
		}
		fclose(config);
	}
	else {
		create_color(1, "bold,white,blue");
		create_color(2, "bold,white,magenta");
		create_color(3, "bold,black,white");
		create_color(4, "black");
		create_color(5, "white,black");
		message("negmanity.conf not found, using default config.");
	}
}

//		Principal
int main(int argc, char **argv){
	initscr();
	start_color();
	noecho();
	keypad(stdscr, 1);
	load_config();
	getmaxyx(stdscr, y, x);
	clear_screen();
	
	//	Abrir archivos
	if(argc > 1) for (int i = 1; i != argc; i++) open_file(argv[i], true);
	else create_new_file(true);
	
	//	Bucle
	while(!quit){
		gui();
		key = getch();
		teclas(key);
		if (showing_status) pressed++;
		if (pressed == 20) {showing_status = false; pressed = 0; memset(status, '\0', sizeof(status)); clear_screen();}
	}
	endwin();
	return 0;
}
