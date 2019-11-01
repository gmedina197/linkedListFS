int size(FILE *fp)
{
	fseek(fp, 0L, SEEK_END);
	int s = ftell(fp);
	rewind(fp);
	return s;
}

int exists(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
}

const char *get_name(char *path, int rmv)
{
	char *ret;
	ret = strrchr(path, rmv);
	ret++;
	return ret;
}

int get_ncluster(int size)
{
	double n = size;
	n = n / 512;
	return (int)ceil(n);
}

int allocate_space(int clusters2allocate, node_t **head)
{
	node_t *current = (*head);
	int firstcluster = current->number;
	int allocatedclusters = 0;

	while (current != NULL)
	{
		if (allocatedclusters == clusters2allocate)
		{
			(*head) = current;
			return firstcluster;
		}

		//printf("%d\n", current->number);
		current = current->next;
		allocatedclusters++;
	}

	return -1;
}

directory store_dir(FILE *FS, char *filename, int initialcluster, int filesize, int attr, int pos)
{

	directory rd;
	for (int i = 0; i < 25; i++)
	{
		if (i < strlen(filename))
		{
			rd.filename[i] = filename[i];
		}
		else
			rd.filename[i] = 0x00;
	}
	rd.attribute = attr;
	rd.initial_cluster = initialcluster;
	rd.size_file = filesize;

	fseek(FS, pos, SEEK_SET);
	directory dir;
	while (1)
	{
		fread(&dir, sizeof(dir), 1, FS);
		if (strcmp(dir.filename, "") == 0)
		{
			fseek(FS, ftell(FS) - 32, SEEK_SET);
			break;
		}
	}

	fwrite(&rd, sizeof(rd), 1, FS);

	return rd;
}

directory save_file(char *filename, FILE *SAVE, FILE *FS, node_t **head, int RESERVED_CLUSTERS)
{
	fseek(FS, RESERVED_CLUSTERS, SEEK_SET);

	int filesize = size(SAVE);
	int clusters = get_ncluster(filesize);
	int current_pos = ftell(FS);

	printf("%d\n", clusters);

	int initialcluster = allocate_space(clusters, &(*head));

	if (initialcluster == -1)
	{
		printf("espaco insuficiente");
		exit(-1);
	}

	directory wd = store_dir(FS, filename, initialcluster, filesize, 1, 512);

	fseek(FS, initialcluster * 512, SEEK_SET);

	//escrevendo conteudo
	char reader;
	while (!feof(SAVE))
	{
		fread(&reader, sizeof(reader), 1, SAVE);
		fwrite(&reader, sizeof(reader), 1, FS);
	}

	return wd;
}

void list(FILE *FS)
{
	fseek(FS, 512, SEEK_SET);
	directory list;
	while (1)
	{
		fread(&list, sizeof(list), 1, FS);
		if (strcmp(list.filename, "") == 0)
			break;

		printf("%s\n", list.filename);
	}
}

char** str_split(char* a_str, const char a_delim)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/* 
void criarSubdir(Diretorio atual, List<String> diretorios, int profundidade) // diretorios seria uma lista ['dir1', 'dir2', 'dir3']
{
	se (não existe subdiretorio chamado diretorios[profundidade] dentro do cluster do arquivo)
		criar subdiretorio chamado diretorios[profundidade];

	while(!feof(FILESYSTEM))
               struct subdir  = leitura do arquivo
		se (subdir.nome == diretorios[profundidade])
			criarSubdir(subdir, diretorios, profundidade + 1);
} */

int check_dir(FILE* FS, char* dirname, int pos) {
	fseek(FS, pos, SEEK_SET);
	directory list;

	while(!feof(FS)) {
		fread(&list, sizeof(list), 1, FS);

		if(strcmp(list.filename, dirname) == 0 && list.attribute == 2) {
			return 1;
		}
	}

	return 0;
}

directory write_dir(FILE *FS, char* dirname, node_t **head, int pos) {
		int initialcluster = allocate_space(1, &(*head));

		if (initialcluster == -1)
		{
			printf("espaco insuficiente");
			exit(-1);
		}
		directory dc = store_dir(FS, dirname, initialcluster, 512, 2, pos);

		return dc;
}

void create_subdir(char* current, char **subdirs, int depth, int pos, node_t **head, FILE* FS, int array_size) {
	if(depth < array_size) {
		int exists = check_dir(FS, current, pos);

		directory subdir;
		if(exists == 0) {
			printf("%d\n", pos);
			fseek(FS, pos, SEEK_SET);


			subdir = write_dir(FS, current, &(*head), pos);
			printf("%d\n", ftell(FS));
		}

		directory list;

		fseek(FS, pos, SEEK_SET);
		while(!feof(FS)) {
			fread(&list, sizeof(list), 1, FS);

			if(strcmp(list.filename, subdirs[depth]) == 0) {
				create_subdir(subdirs[depth+1], subdirs, depth + 1, list.initial_cluster * 512, &(*head), FS, array_size);	
			}
		}
	}
}

void make_subdir(FILE *FS, char* subdirname, node_t **head, int offset) {
	fseek(FS, offset, SEEK_SET);

	char** subdir_names = str_split(subdirname, '/');

	if(subdir_names) {
		int array_size = 0;
		for (int i = 0; *(subdir_names + i); i++)
		{
			//printf("%s\n", subdir_names[i]);
			array_size++;
		}
		create_subdir(subdir_names[0], subdir_names, 0, offset, &(*head), FS, array_size);

	} else {
		printf("erro");
	}
}

 /* 
		fseek(FS, initialcluster * 512, SEEK_SET);
		directory root;
		for (int i = 0; i < 25; i++)
		{
			root.filename[i] = '.';
			if(i > 0) {
				root.filename[i] = 0x00;
			}
		}
		root.attribute = 1;
		root.initial_cluster = 2;
		root.size_file = 0;
		fwrite(&root, sizeof(root), 1, FS); */