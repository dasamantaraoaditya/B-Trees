#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
struct student
{
	int rollno;
	char name[24];
	char college[20];
};

struct non_leaf_page
{
	int type;
	int key[63];
	int offset[64];
};

struct leaf_page
{
	int type;
	struct student data[10];
	char unused[28];
};
struct tablesector
{
	int tableid[8];
	int rootpageid[8];
};

void get_student_by_id(int id)
{
	struct leaf_page lp;
	struct non_leaf_page nlp;
	struct tablesector ts;
	int offset = 0, i;
	FILE * fp = fopen("storage.bin", "rb");
	fread(&ts, sizeof(tablesector), 1, fp);
	fseek(fp, ts.rootpageid[0], SEEK_SET);
	fread(&nlp, sizeof(struct non_leaf_page), 1, fp);
	for (i = 0; i < 63; i++)
		if (nlp.key[i] > id)
			break;
	fseek(fp, sizeof(tablesector)+(nlp.offset[i] * 512), SEEK_SET);
	fread(&lp, sizeof(struct non_leaf_page), 1, fp);
	for (i = 0; i < 10; i++)
		if (lp.data[i].rollno == id)
		{
			printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
			break;
		}
	if (lp.data[i].rollno != id)
		printf("No Student Found");
	fclose(fp);

}

void read_data2(FILE * fp)
{
	int ts_counter=0,i, flag = 0, student_count = 0, offset_count = 0, keys[1000], key_counter = 0,root_position=0,table_size=8;
	struct leaf_page lp;
	FILE * fp_b = fopen("storage.bin", "wb+");
	char * buffer = (char *)malloc(sizeof(char) * 100);
	fgets(buffer, 100, fp);
	struct tablesector ts;
	memset(&ts, '\0', sizeof(struct tablesector));
	fwrite(&ts,sizeof(tablesector),1,fp_b);
	struct non_leaf_page nlp;
	memset(&nlp, '\0', sizeof(struct non_leaf_page));
	while (!feof(fp))
	{
		memset(&lp, '\0', sizeof(struct leaf_page));
		for (i = 0; i < 10 && !feof(fp); i++)
			fscanf(fp, "%d,%[^,],%[^\n]", &lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
		lp.type = 1;
		fwrite(&lp, sizeof(struct leaf_page), 1, fp_b);
		if (flag)
		{
			nlp.key[key_counter++] = lp.data[0].rollno;
		}
		nlp.offset[offset_count] = offset_count++;
		flag = 1;
	}
	for (i = key_counter; i < 63; i++)
		nlp.key[key_counter] = 0;
	nlp.type = 2;
	root_position = ftell(fp_b);
	fwrite(&nlp, sizeof(struct non_leaf_page), 1, fp_b);

	fseek(fp_b, 0, SEEK_SET);
	fread(&ts, sizeof(struct tablesector), 1, fp_b);
	while (ts_counter < table_size&&ts.tableid[ts_counter] != 0)ts_counter++;
	ts.tableid[ts_counter] = nlp.type;
	ts.rootpageid[ts_counter] = root_position;
	fseek(fp_b, 0, SEEK_SET);
	fwrite(&ts, sizeof(tablesector), 1, fp_b);
	fclose(fp_b);
}

void group_by_college(char * college_name, int lower, int upper, int Query)
{
	struct leaf_page lp;
	struct non_leaf_page nlp;
	struct tablesector ts;
	int offset = 0, i,table_id=0;
	FILE * fp = fopen("storage.bin", "rb");
	fread(&ts, sizeof(tablesector), 1, fp);
	for (i = 0; i < 8; i++)if (ts.tableid[i] == table_id)break;
	fseek(fp, ts.rootpageid[i], SEEK_SET);
	fread(&nlp, sizeof(struct non_leaf_page), 1, fp);
	fseek(fp, (640*512*table_id)+sizeof(tablesector), SEEK_SET);
	while (true)
	{
		fread(&lp, sizeof(struct non_leaf_page), 1, fp);
		if (lp.type!=1)break;
		for (i = 0; i < 10; i++)
		{
			switch (Query)
			{
			case 2:
				if (lp.data[i].rollno != 0 && !strcmp(lp.data[i].college, college_name))
				{
					printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
				}
				break;
			case 3:
				if (lp.data[i].rollno != 0 && lp.data[i].rollno >= lower && lp.data[i].rollno <= upper)
				{
					printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
				}
				break;

			case 4:
				if (lp.data[i].rollno != 0 && lp.data[i].rollno >= lower && lp.data[i].rollno <= upper&&!strcmp(lp.data[i].college, college_name))
				{
					printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
				}
				break;
			}
		}
	}
	fclose(fp);
}

int main()
{
	int choise, i, lower, upper;
	char college_name[50];
	FILE * fp = fopen("data_640.csv", "r");
	read_data2(fp);
	while (1)
	{
		printf("\n\n1.Get student by RollNo\n2.Group students by College\n3.Get students in range group by college\n4.Get students in range and Group by College\n5.Exit\nEnter ur choise:");
		scanf("%d", &choise);
		switch (choise)
		{
		case 1:
			printf("\nEnter Students Roll NO");
			scanf("%d", &i);
			get_student_by_id(i);
			break;
		case 2:
			printf("\nEnter College Name:");
			scanf("%s", college_name);
			group_by_college(college_name, 0, 0, choise);
			break;
		case 3:
			printf("\nEnter Lower Range:");
			scanf("%d", &lower);
			printf("\nEnter Upper Range:");
			scanf("%d", &upper);
			group_by_college(NULL, lower, upper, choise);
			break;
		case 4:
			printf("\nEnter Lower Range:");
			scanf("%d", &lower);
			printf("\nEnter Upper Range:");
			scanf("%d", &upper);
			printf("\nEnter College Name:");
			scanf("%s", college_name);
			group_by_college(college_name, lower, upper, choise);
			break;
		case 5:exit(0);
		default:printf("\n--------Invalied Choise-----");
		}
	}

	fclose(fp);
	getch();
	return 0;
}
