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
	int set_pos = 0;
	char reader;

	for (int i = pos; i < fssize; i++) 
	{
		fread(&reader, sizeof(reader), 1, FS);

		if(reader != 0) 
		{
			freeclusters = 0;
		} 

		if(freeclusters == clusters2allocate) 
		{
			return freeclusters + 9;		
		}

		if(i % 512 == 0) 
		{
			freeclusters++;
		}
	}
	
}

void save_file(char *filename, FILE* SAVE, FILE *FS) {
	fseek(FS, RESERVED_CLUSTERS, SEEK_SET);

	int filesize = size(SAVE);
	int clusters = get_ncluster(filesize);
	int current_pos = ftell(FS);

	int last = allocate_space(FS, current_pos, 65536 * 512, clusters);
	printf("%d\n", clusters);
	printf("%d\n", current_pos);
	printf("cluster = %d\n", last);

	/* directory rd;
	for (int i = 0; i < 25; i++)
	{
		if (i < strlen(name))
		{
			rd.filename[i] = name[i];
		}
		else
			rd.filename[i] = 0x00;
	}
	rd.attribute = 1;
	rd.initial_cluster = cluster;
	rd.size_file = size(SAVE);

	fseek(fat, 131584, SEEK_SET);
	directory dir;
	while (1)
	{
		fread(&dir, sizeof(dir), 1, fat);
		if (strcmp(dir.filename, "") == 0)
		{
			fseek(fat, ftell(fat) - 32, SEEK_SET);
			break;
		}
	}

	fwrite(&rd, sizeof(rd), 1, fat); */

}