const int RESERVED_CLUSTERS = 512 + (512 * 8);

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

int allocate_space(FILE *FS, int pos, int fssize, int clusters2allocate) 
{
	int freeclusters = 0;
	int occupiedcluster = 9;
	char reader;
	fseek(FS, pos, SEEK_SET);
	while(!feof(FS))
	{
		pos += 512;
		fread(&reader, sizeof(reader), 1, FS);
		fseek(FS, pos,SEEK_SET);
		if(reader != 0) 
		{
			occupiedcluster++;
			freeclusters = 0;
		} else {
			freeclusters++;
		}

		if(freeclusters == clusters2allocate) 
		{

			return occupiedcluster;		
		}
	}

	return -1;
}

void store_dir(FILE* FS, char* filename, int initialcluster, int filesize) {

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
	rd.attribute = 1;
	rd.initial_cluster = initialcluster + 1;
	rd.size_file = filesize;

	fseek(FS, 512, SEEK_SET);
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
}

void save_file(char *filename, FILE* SAVE, FILE *FS) {
	fseek(FS, RESERVED_CLUSTERS, SEEK_SET);

	int filesize = size(SAVE);
	int clusters = get_ncluster(filesize);
	int current_pos = ftell(FS);

	int initialcluster = allocate_space(FS, current_pos, 65536 * 512, clusters);

	if(initialcluster == -1) {
		printf("espaco insuficiente");
		exit(-1);
	}

	store_dir(FS, filename, initialcluster, filesize);

	fseek(FS, initialcluster * 512, SEEK_SET);

	//escrevendo conteudo
	char reader;
	while (!feof(SAVE)) {
		fread(&reader, sizeof(reader), 1, SAVE);
		fwrite(&reader, sizeof(reader), 1, FS);
	}
}

void list_subdir(FILE *FS, char *nome_dir)
{
	fseek(FS, 512, SEEK_SET);
	directory subdir, list;
	int cont = 0;
	while (1)
	{
		if (cont == 16)
		{
			printf("subdiretorio nao encontrado");
			break;
		}
		fread(&subdir, sizeof(subdir), 1, FS);
		if (strcmp(subdir.filename, nome_dir) == 0)
		{
			break;
		}
		cont++;
	}
	fseek(FS, subdir.initial_cluster * 512, SEEK_SET);

	while (1)
	{
		fread(&list, sizeof(list), 1, FS);
		if (list.filename[0] == 0)
			break;
		if (list.attribute == 1 && list.filename[0] != 0xe5)
			printf("%s\n", list.filename);
	}
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
