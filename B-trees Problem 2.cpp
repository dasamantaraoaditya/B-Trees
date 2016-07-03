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

void get_student_by_id(int id)
{
	struct leaf_page lp;
	struct non_leaf_page nlp;
	int offset = 0,i;
	FILE * fp = fopen("storage.bin", "rb");
	fseek(fp, 0, SEEK_END);
	int pos = ftell(fp);
	pos -= 512;
	fseek(fp, pos, SEEK_SET);
	
	fread(&nlp, sizeof(struct non_leaf_page), 1, fp);
		for (i = 0; i < 63; i++)
			if (nlp.key[i] > id)
				break;
		fseek(fp, nlp.offset[i]*512, SEEK_SET);
		fread(&lp, sizeof(struct non_leaf_page), 1, fp);
		for (i = 0; i < 10;i++)
			if (lp.data[i].rollno == id)
			{
				printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
				break;
			}
		if (lp.data[i].rollno!=id)
		printf("No Student Found");	
	fclose(fp);

}

void read_data2(FILE * fp)
{
	int i, flag = 0, student_count = 0, offset_count = 0, keys[1000], key_counter = 0;
	struct leaf_page lp;
	FILE * fp_b = fopen("storage.bin", "wb+");
	char * buffer = (char *)malloc(sizeof(char) * 100);
	fgets(buffer, 100, fp);
	struct non_leaf_page nlp;
	memset(&nlp, '\0', sizeof(struct non_leaf_page));
	while (!feof(fp))
	{
		memset(&lp, '\0', sizeof(struct leaf_page));
		for (i = 0; i < 10&&!feof(fp); i++)
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
	for (i = key_counter; i < 63;i++)
		nlp.key[key_counter] = 0;
	nlp.type = 0;
	fwrite(&nlp, sizeof(struct non_leaf_page), 1, fp_b);
	fclose(fp_b);
}

void group_by_college(char * college_name,int lower,int upper,int Query)
{
	struct leaf_page lp;
	int offset = 0, i;
	FILE * fp = fopen("storage.bin", "rb");
	while (true)
	{
		fread(&lp, sizeof(struct non_leaf_page), 1, fp);
		if (!lp.type)break;
		for (i = 0; i < 10; i++)
		{
			switch (Query)
			{
			case 2:
				if (lp.data[i].rollno!=0&&!strcmp(lp.data[i].college, college_name))
				{
					printf("\n%d,%s,%s", lp.data[i].rollno, lp.data[i].name, lp.data[i].college);
				}
				break;
			case 3:
				if (lp.data[i].rollno != 0 && lp.data[i].rollno >=lower && lp.data[i].rollno <=upper )
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
			group_by_college(college_name,0,0,choise);
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
			scanf("%s",college_name);
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
