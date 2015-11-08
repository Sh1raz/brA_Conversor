/*******************************
*	brA Conversor TXT para SQL
*	Autor:		Shiraz
*	Data:		22/10/2012
*	Updated:	08/11/2015
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Macros
#define ARRAY_SIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// Funções de Utilidade
void show_message(int color, const char *msg, ...);
char *replace_str(char *str, char *orig, char *rep);
char *escape_str(char *str);
int explode(char ***arr_ptr, char *str, char delimiter);

// Variáveis de Utilidade
static unsigned int file_count;

// Funções de Conversão
void convert_abra_db(void);

// Tabelas de Cores
enum txt_colors {
	DEFAULT = 0xF,
	GREEN = 0x2F,
	YELLOW = 0xE,
	RED = 0xC,
	PINK = 0xD,
	WHITE = 0x7,
	LIGHT_GREEN = 0xA
};

// Função Inicial
void main(void) {
	show_message(GREEN, "                                                                               \n");
	show_message(GREEN, "                 _           ___  _   _                                        \n");
	show_message(GREEN, "                | |         / _ \\| | | |                                       \n");
	show_message(GREEN, "                | |__  _ __/ /_\\ \\ |_| |__   ___ _ __    _ _                   \n");
	show_message(GREEN, "                | '_ \\| '__|  _  | __| '_ \\ / _ \\ '_ \\ / _` |                  \n");
	show_message(GREEN, "                | |_) | |  | | | | |_| | | |  __/ | | | (_| |                  \n");
	show_message(GREEN, "                |_.__/|_|  \\_| |_/\\__|_| |_|\\___|_| |_|\\__,_|                  \n");
	show_message(GREEN, "                                                                               \n");
	show_message(GREEN, "                      Project brAthena (c) 2008 - 2015                         \n");
	show_message(GREEN, "                              www.brathena.org                                 \n");
	show_message(GREEN, "                                                                               \n\n");
	
	convert_abra_db();
	
	show_message(YELLOW, "Converted files: %d\n", file_count);
	system("pause");
	return;
}

// Exibe uma mensagem colorida.
void show_message(int color, const char *msg, ...) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	va_list args;

	va_start(args, msg);
	vfprintf(stdout, msg, args);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
}

// Escapa uma string para evitar erros de MySQL.
char *escape_str(char *str)
{
	char *new_str;
	char *source, *target;
	char *end;
	int new_length;
	int length = strlen(str);

    if (length == 0) {
		char *p = (char *)malloc(1);
		if (p == NULL)
			return p;
		p[0] = 0;
		return p;
    }

	new_str = (char *)malloc(2 * length + 1);
	if (new_str == NULL)
        return new_str;
	source = str;
	end = source + length;
	target = new_str;

	while (source < end) {
		switch (*source) {
			case '\0':
				*target++ = '\\';
				*target++ = '0';
				break;
			case '\'':
			case '\"':
			case '\\':
				*target++ = '\\';
			default:
				*target++ = *source;
				break;
		}
		source++;
	}

	*target = 0;
	new_length = target - new_str;
	new_str = (char *)realloc(new_str, new_length + 1);
	return new_str;
}

// Retorna uma matriz de strings, cada uma como substring.
int explode(char ***arr_ptr, char *str, char delimiter)
{
	char *src = str, *end, *dst;
	char **arr;
	int size = 1, i;

	while ((end = strchr(src, delimiter)) != NULL) {
		++size;
		src = end + 1;
	}

	arr = malloc(size * sizeof(char *) + (strlen(str) + 1) * sizeof(char));

	src = str;
	dst = (char *)arr + size * sizeof(char *);
	for (i = 0; i < size; ++i) {
		if ((end = strchr(src, delimiter)) == NULL)
			end = src + strlen(src);
		arr[i] = dst;
		strncpy(dst, src, end - src);
		dst[end - src] = '\0';
		dst += end - src + 1;
		src = end + 1;
	}
	*arr_ptr = arr;

	return size;
}

// Retorna uma string ou um array com todas as ocorrências de char *str.
char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *p;

	if (!(p = strstr(str, orig)))
		return str;

	strncpy(buffer, str, p-str);
	buffer[p-str] = '\0';

	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
	return buffer;
}

// Converte o arquivo abra_db.txt para SQL.
void convert_abra_db()
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;
	
	sprintf(path, "%s/%s", "db", "abra_db.txt");
	
	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/abra_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token = strtok(line, ",");
		char *rows[3];

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		for (i = 0; i < 4; ++i) {
			rows[i] = token;
			token = strtok(0, ",");
		}

		fprintf(fwrite, "REPLACE INTO abra_db VALUES(%d,'%s',%d,%d);\n", atoi(rows[0]), escape_str(rows[1]), atoi(rows[2]), atoi(rows[3]));
		count++;
	}
	
	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}
