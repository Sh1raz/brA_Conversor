/*******************************
* brA Conversor TXT para SQL
* Autor:	Shiraz
* Data:		22/10/2012
* Updated:	08/11/2015
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
void convert_castle_db(void);
void convert_create_arrow_db(void);
void convert_elemental_db(void);
void convert_elemental_skill_db(void);
void convert_guild_skill_tree_db(void);
void convert_homunculus_db(void);
void convert_homun_skill_tree_db(void);
void convert_mercenary_db(void);
void convert_mercenary_skill_db(void);
void convert_mob_avail_db(void);
void convert_mob_chat_db(void);
void convert_mob_classchange_db(void);
void convert_mob_item_ratio_db(void);
void convert_pet_db(void);
void convert_produce_db(void);
void convert_skill_changematerial_db(void);
void convert_skill_improvise_db(void);
void convert_skill_reproduce_db(void);
void convert_spellbook_db(void);
void convert_quest_db(void);
void convert_const_db(void);
void convert_job_db2(void);
void convert_sc_config(void);

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
	convert_castle_db();
	convert_create_arrow_db();
	convert_elemental_db();
	convert_elemental_skill_db();
	convert_guild_skill_tree_db();
	convert_homunculus_db();
	convert_homun_skill_tree_db();
	convert_mercenary_db();
	convert_mercenary_skill_db();
	convert_mob_avail_db();
	convert_mob_chat_db();
	convert_mob_classchange_db();
	convert_mob_item_ratio_db();
	convert_pet_db();
	convert_produce_db();
	convert_skill_changematerial_db();
	convert_skill_improvise_db();
	convert_skill_reproduce_db();
	convert_spellbook_db();
	convert_quest_db();
	convert_const_db();
	convert_job_db2();
	convert_sc_config();
	
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
	for (i = 0; i < size; i++) {
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
void convert_abra_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/abra_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/abra_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 4; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += ((i == 1)) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO abra_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo castle_db.txt para SQL.
void convert_castle_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/castle_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/castle_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 5; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += ((i >= 1 && i <= 3)) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO castle_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", "db/castle_db.txt", count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo create_arrow_db.txt para SQL.
void convert_create_arrow_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/create_arrow_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/create_arrow_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 11; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (token == NULL) ? sprintf(pos, "%s", "NULL") : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO create_arrow_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", "db/create_arrow_db.txt", count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo elemental_db.txt para SQL.
void convert_elemental_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/elemental_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/elemental_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 26; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 1 || i == 2) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO elemental_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo elemental_skill_db.txt para SQL.
void convert_elemental_skill_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/elemental_skill_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/elemental_skill_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 4; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 3) ? sprintf(pos, "%d", atoi(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO elemental_skill_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo guild_skill_tree.txt para SQL.
void convert_guild_skill_tree_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/guild_skill_tree.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/guild_skill_tree.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 12; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 11) ? sprintf(pos, "%d", atoi(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO guild_skill_tree_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo homunculus_db.txt para SQL.
void convert_homunculus_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/homunculus_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/homunculus_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 50; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 2) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO homunculus_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo homun_skill_tree.txt para SQL.
void convert_homun_skill_tree_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/homun_skill_tree.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/homun_skill_tree.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 14; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 13) ? sprintf(pos, "%d", atoi(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO homun_skill_tree_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mercenary_db.txt para SQL.
void convert_mercenary_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mercenary_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mercenary_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 26; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += ((i == 1 || i == 2)) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mercenary_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mercenary_skill_db.txt para SQL.
void convert_mercenary_skill_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mercenary_skill_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mercenary_skill_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 3; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 2) ? sprintf(pos, "%d", atoi(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mercenary_skill_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mob_avail.txt para SQL.
void convert_mob_avail_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mob_avail.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mob_avail.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 12; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (token == NULL) ? sprintf(pos, "%s", "NULL") : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mob_avail VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mob_chat_db.txt para SQL.
void convert_mob_chat_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mob_chat_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mob_chat_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 3; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 2) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mob_chat_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mob_classchange.txt para SQL.
void convert_mob_classchange_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mob_classchange.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mob_classchange.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 3; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 1) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mob_classchange_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo mob_item_ratio.txt para SQL.
void convert_mob_item_ratio_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/mob_item_ratio.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/mob_item_ratio.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 3; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (token == NULL) ? sprintf(pos, "%s", "NULL") : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO mob_item_ratio_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo pet_db.txt para SQL.
void convert_pet_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/pet_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/pet_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, **script, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		explode(&script, (char *)line, '{');
		token = strtok(line, ",");

		for (i = 0; i < 22; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			if (i < 20)
				pos += ((i == 1 || i == 2)) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			else
				pos += (i == 21) ? sprintf(pos, "'{%s'", escape_str(script[2])) : sprintf(pos, "'{%s'", replace_str(escape_str(script[1]), "},", "}"));

			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO pet_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo produce_db.txt para SQL.
void convert_produce_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/produce_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/produce_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 28; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (token == NULL) ? sprintf(pos, "%s", "NULL") : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO produce_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo skill_changematerial_db.txt para SQL.
void convert_skill_changematerial_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/skill_changematerial_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/skill_changematerial_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 12; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (token == NULL) ? sprintf(pos, "%s", "NULL") : sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO skill_changematerial_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo skill_improvise_db.txt para SQL.
void convert_skill_improvise_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/skill_improvise_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/skill_improvise_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 2; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO skill_improvise_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo skill_reproduce_db.txt para SQL.
void convert_skill_reproduce_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0;

	sprintf(path, "%s", "db/skill_reproduce_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/skill_reproduce_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char write[1024];

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';

		snprintf(write, sizeof(write), "REPLACE INTO skill_reproduce_db VALUES(%d);\n", atoi(line));
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo spellbook_db.txt para SQL.
void convert_spellbook_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/spellbook_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/spellbook_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 3; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO spellbook_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo quest_db.txt para SQL.
void convert_quest_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/quest_db.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/quest_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 18; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 17) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%s", token);
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO quest_db VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo const.txt para SQL.
void convert_const_db(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0;

	sprintf(path, "%s", "db/const.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/const_db.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char write[1024], name[1024], val[1024];
		int type = 0;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';

		if(sscanf(line,"%1023[A-Za-z0-9/_],%1023[A-Za-z0-9/_-],%d",name,val,&type)>=2 ||
		   sscanf(line,"%1023[A-Za-z0-9/_] %1023[A-Za-z0-9/_-] %d",name,val,&type)>=2){
			if(type)
				snprintf(write, sizeof(write), "REPLACE INTO const_db VALUES('%s','%s',%d);\n", name, val, type);
			else
				snprintf(write, sizeof(write), "REPLACE INTO const_db VALUES('%s','%s',%s);\n", name, val, "NULL");
			fprintf(fwrite, write);
			count++;
		}
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo job_db2.txt para SQL.
void convert_job_db2(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0;

	sprintf(path, "%s", "db/job_db2.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/job_db2.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';

		fprintf(fwrite, "REPLACE INTO job_db2 VALUES('%s');\n", line);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}

// Converte o arquivo sc_config.txt para SQL.
void convert_sc_config(void)
{
	FILE *fread, *fwrite;
	char line[1024], path[256];
	int count = 0, i;

	sprintf(path, "%s", "db/sc_config.txt");

	if (!(fread = fopen(path, "r")))
		return;

	fwrite = fopen("sql/sc_config.sql", "w+");

	while (fgets(line, sizeof(line), fread) != NULL) {
		char *token, buf[1024], write[1024], *pos = buf;

		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
			continue;

		line[strlen(line)-1] = '\0';
		token = strtok(line, ",");

		for (i = 0; i < 2; i++) {
			if (i) {
				pos += sprintf(pos, ",");
			}

			pos += (i == 0) ? sprintf(pos, "'%s'", escape_str(token)) : sprintf(pos, "%d", atoi(token));
			token = strtok(NULL, ",");
		}

		snprintf(write, sizeof(write), "REPLACE INTO sc_config VALUES(%s);\n", buf);
		fprintf(fwrite, write);
		count++;
	}

	show_message(LIGHT_GREEN, "File %s successfully converted! rows affected: %d\n", path, count);
	fclose(fread);
	fclose(fwrite);
	file_count++;
}
