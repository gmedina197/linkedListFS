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

directory save_file(char *filename, FILE *SAVE, FILE *FS, node_t **head, int pos)
{
	fseek(FS, pos, SEEK_SET);

	int filesize = size(SAVE);
	int clusters = get_ncluster(filesize);

	printf("%d\n", clusters);

	int initialcluster = allocate_space(clusters, &(*head));

	if (initialcluster == -1)
	{
		printf("espaco insuficiente");
		exit(-1);
	}

	directory wd = store_dir(FS, filename, initialcluster, filesize, 1, pos);

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

void list_files(FILE *FS, int pos, int space)
{
	fseek(FS, pos, SEEK_SET);
	directory list;

	fread(&list, sizeof(list), 1, FS);
		
	if (strcmp(list.filename, "") != 0) {
		for (int i = 0; i < space; i++) {
			printf(" "); 
		}
		printf("%s\n", list.filename); 
		if(list.attribute == 2) {
			list_files(FS, list.initial_cluster * 512, space + 1);
		}

		list_files(FS, pos+32, space);
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
			fseek(FS, pos, SEEK_SET);
			subdir = write_dir(FS, current, &(*head), pos);
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

directory get_subdir(FILE* FS, int pos, char *dirname) {
	fseek(FS, pos, SEEK_SET);
	directory dir;
	while (1)
	{
		fread(&dir, sizeof(dir), 1, FS);
		if (strcmp(dir.filename, dirname) == 0)
		{
			return dir;
		}
	}
}

void add_to_subdir(FILE* FS, FILE *SAVE, char *filename, char **subdirnames,int depth, int pos, node_t **head, int array_size) {
		int exists = check_dir(FS, subdirnames[depth], pos);

		directory subdir;
		if(exists == 1) {
			fseek(FS, pos, SEEK_SET);
			subdir = get_subdir(FS, pos, subdirnames[depth]);
			if(depth == array_size - 1) {
				save_file(filename, SAVE, FS, &(*head), subdir.initial_cluster * 512);
			} else {
				add_to_subdir(
					FS, 
					SAVE, 
					filename, 
					subdirnames, 
					depth+1, 
					subdir.initial_cluster* 512, 
					&(*head), 
					array_size
				);
			}
		}
}

void verify_subdir(FILE *FS, FILE *SAVE, char* subdirname,char *filename, node_t **head) {
	fseek(FS, 512, SEEK_SET);

	char** subdir_names = str_split(subdirname, '/');

	if(subdir_names) {
		int array_size = 0;
		for (int i = 0; *(subdir_names + i); i++)
		{
			//printf("%s\n", subdir_names[i]);
			array_size++;
		}
		add_to_subdir(FS,SAVE, filename, subdir_names, 0, 512, &(*head), array_size);

	} else {
		printf("erro");
	}
}

void export_dir(FILE *FS, FILE *SAVE, char *filename) {
	fseek(FS, 512, SEEK_SET);
	directory dir;
	while (!feof(FS))
	{
		fread(&dir, sizeof(dir), 1, FS);
		if (strcmp(dir.filename, filename) == 0)
		{
			fseek(FS, dir.initial_cluster * 512, SEEK_SET);
			char reader = 0;
			for (int i = 0; i < dir.size_file; i++)
			{
				fread(&reader, sizeof(reader), 1, FS);
				fwrite(&reader, sizeof(reader), 1, SAVE);
			}
			break;
		}
	}
}

void format(FILE *FS, int num_sectors) {
	int bytes_to_format = num_sectors * 512; //cluster = sector;

	unsigned char zero = 0;
	fseek(FS, 9 * 512, SEEK_SET);
	printf("Formatando...\n");
	for (int i = 0; i < bytes_to_format; i++)
	{
		fwrite(&zero, sizeof(zero), 1, FS);
	}

	boot_sec(FS);
	make_dir(FS);
	printf("Concluido\n");
}