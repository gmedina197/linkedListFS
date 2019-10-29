#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "data-structs.h"
#include "makeFs.h"
#include "utils.h"

#define clear() printf("\033[H\033[J")


void make_menu();

int main(int argc, char **argv) {
    FILE *FILE_FS, *save;
	if (exists("fs.img") == 0)
	{
		int fssize = 65536 * 512;
		FILE_FS = fopen("fs.img", "wb+");
		
		fseek(FILE_FS, fssize , SEEK_SET);
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

	//TODO: verificar cluster inicial, possivelmente tem 1 a mais apos a segunda insercao
	while(1) {
		char choose[100];
		//make_menu();
		printf("> ");
		scanf("%100s", choose);
		fflush(stdin); 

		if(strstr(choose, "add") != NULL) {
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
			save_file(filename, SAVE, FILE_FS);

			fclose(SAVE);
		}
		else if (strstr(choose, "ls") != NULL) {
			list(FILE_FS);
		}
		else if(strstr(choose, "sair") != NULL) {
			printf("saiu\n");
			exit(0);
		} else {
			//clear();
			printf("opcao invalida\n");
		} 
	}

    fclose(FILE_FS);
}

void make_menu() {
	printf("----------- SISTEMA DE AQUIVOS -------------\n");
	printf("1 - Adicionar Arquivo \n");
	printf("2 - Listar Arquivos \n");
	printf("3 - Criar subdir \n");
	printf("9 - Sair\n");
	printf("selecione: ");
}
