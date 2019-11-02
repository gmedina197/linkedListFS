#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "linkedList.c"
#include "data-structs.h"
#include "makeFs.h"
#include "utils.h"

int main(int argc, char **argv)
{
	FILE *FILE_FS, *save;
	node_t *start = NULL;

	int RESERVED_CLUSTERS = 512 * 9;

	//65527
	int FREE_CLUSTERS = 65527;
	for (int i = FREE_CLUSTERS - 1; i >= 0; i--)
	{
		push(&start, i + 9);
	}

	if (exists("fs.img") == 0)
	{
		int fssize = 65536 * 512;
		FILE_FS = fopen("fs.img", "wb+");

		fseek(FILE_FS, fssize, SEEK_SET);
		fputc('\0', FILE_FS);
		rewind(FILE_FS);

		if (FILE_FS == NULL)
		{
			printf("Erro!");
			exit(1);
		}
		boot_sec(FILE_FS);
		make_dir(FILE_FS);
		fclose(FILE_FS);
	}
	FILE_FS = fopen("fs.img", "rb+");

	while (1)
	{
		char choose[100];
		printf("> ");
		scanf("%100s", choose);
		fflush(stdin);

		if (strstr(choose, "add") != NULL)
		{
			char filepath[100], filename[100];
			FILE *SAVE;

			printf("Digite o arquivo: ");

			scanf("%100s", filepath);
			fflush(stdin);

			printf("%s\n", filepath);
			SAVE = fopen(filepath, "rb");
			if (SAVE == NULL)
			{
				printf("file cannot be open\n");
				exit(-1);
			}
			strcpy(filename, get_name(filepath, '/'));
			save_file(filename, SAVE, FILE_FS, &start, 512);

			fclose(SAVE);
		}
		else if (strstr(choose, "ls") != NULL) {
			list_files(FILE_FS, 512, 0);
		}
		else if (strstr(choose, "mkdir") != NULL) {
			char nome[25];

			printf("Digite o subdir: ");

			scanf("%100s", nome);
			fflush(stdin);

			make_subdir(FILE_FS, nome, &start, 512);
		}
		else if(strstr(choose, "filesb") != NULL) {
			char filepath[100], filename[25], subdirname[25];
			FILE *SAVE;

			printf("Digite o arquivo: ");

			scanf("%100s", filepath);
			fflush(stdin);

			SAVE = fopen(filepath, "rb");
			if (SAVE == NULL)
			{
				printf("file cannot be open\n");
				exit(-1);
			}
			strcpy(filename, get_name(filepath, '/'));
			
			printf("Digite o subdir: ");

			scanf("%s", subdirname);
			fflush(stdin);

			verify_subdir(FILE_FS, SAVE, subdirname, filename, &start);
			//save_file(filename, SAVE, FILE_FS, &start, RESERVED_CLUSTERS);

			fclose(SAVE);
		}
		else if (strstr(choose, "sair") != NULL)
		{
			printf("saiu\n");
			exit(0);
		}
		else if(strstr(choose, "formatar") != NULL) {
			int numsector;

			printf("Numero de setores: ");
			scanf("%d", &numsector);

			format(FILE_FS, numsector);
		}
		else if(strstr(choose, "exportar") != NULL) {
			char filepath[100], filename[25];
			FILE *SAVE;

			printf("Digite o arquivo: ");

			scanf("%100s", filepath);
			fflush(stdin);

			SAVE = fopen(filepath, "wb");
			if (SAVE == NULL)
			{
				printf("file cannot be open\n");
				exit(-1);
			}

			strcpy(filename, get_name(filepath, '/'));
			

			fflush(stdin);

			export_dir(FILE_FS, SAVE, filename);

			fclose(SAVE);
		}
		else if(strstr(choose, "ajuda") != NULL) {
			printf("add - adiciona arquivo ao root\n");
			printf("ls - lista arquivos\n");
			printf("mkdir - criar diretorios de n niveis\n");
			printf("filesb - adiciona arquivo em subdir\n");
			printf("exportar - exporta um arquivo\n");
		}
		else
		{
			printf("opcao invalida\n");
		}
	}

	fclose(FILE_FS);
}