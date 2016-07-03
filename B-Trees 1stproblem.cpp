#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
struct student
{
	int type;
	int rollno;
	char name[28];
	char college[28];
};

struct non_leaf_page
{
	int type;
	int offset[3];
	int key[2];
	char unused[40];
};

void get_student_by_id(int id)
{
	struct non_leaf_page nlp;
	struct student st;
	int offset = 0;
	FILE * fp = fopen("storage.bin", "rb");
	fseek(fp, 0, SEEK_END);
	int pos = ftell(fp);
	pos -= 64;
	fseek(fp, pos, SEEK_SET);
	while (true)
	{
		fread(&nlp, sizeof(struct non_leaf_page), 1, fp);
		if (nlp.type)
			break;
		if (nlp.key[0] > id)
		{
			offset = nlp.offset[0];
		}
		else if (nlp.key[0] <= id&&nlp.key[1] > id)
		{
			offset = nlp.offset[1];
		}
		else
		{
			offset = nlp.offset[2];
		}
		fseek(fp, offset * 64, SEEK_SET);
	}

	fseek(fp, -64, SEEK_CUR);
	fread(&st, sizeof(struct student), 1, fp);
	if (st.rollno != id)
		printf("No Student Found");
	else
		printf("\n%d,%s,%s", st.rollno, st.name, st.college);
	fclose(fp);

}
int get_cube(int number)
{
	int cube=3;
	while (number&&number>cube)
	{
		cube *= 3;
	}
	return cube;
}

void read_data2(FILE * fp)
{
	int flag = 1, student_count = 0, offset_count = 0, keys[1000], key_counter = 0;
	struct student st;
	FILE * fp_b = fopen("storage.bin", "wb");
	char * buffer = (char *)malloc(sizeof(char) * 100);
	fgets(buffer, 100, fp);
	
	while (!feof(fp))
	{
		memset(&st, '\0', sizeof(struct student));
		fscanf(fp, "%d,%[^,],%[^\n]", &st.rollno, st.name, st.college);
		//printf("%d,%s,%s",st.rollno, st.name, st.college);
		keys[key_counter++] = st.rollno;
		st.type = 1;
		fwrite(&st, sizeof(struct student), 1, fp_b);
	}

	struct non_leaf_page nlp;
	student_count = ftell(fp_b) / 64;
	printf("Total Records Read: %d", student_count);
	int level_count = get_cube(student_count);

	while (student_count < level_count)
	{
		memset(&st, '\0', sizeof(struct student));
		//printf("%d,%s,%s", st.rollno, st.name, st.college);
		keys[key_counter++] = 10000;
		st.type = 1; student_count++;
		fwrite(&st, sizeof(struct student), 1, fp_b);
	}

	while (level_count != 1)
	{
		key_counter = 1;
		int key_count = 1;
		level_count = level_count / 3;
		int i;
		for (i = 0; i < level_count; i++)
		{
			memset(&nlp, '\0', sizeof(struct non_leaf_page));
			nlp.key[0] = keys[key_count++]; nlp.key[1] = keys[key_count++]; nlp.type = 0;
			nlp.offset[0] = offset_count++; nlp.offset[1] = offset_count++; nlp.offset[2] = offset_count++;
			fwrite(&nlp, sizeof(struct non_leaf_page), 1, fp_b);
			keys[key_counter++] = keys[key_count++];
		}

	}
	fclose(fp_b);
}


void groupbycollege(char * college_name, struct non_leaf_page nlp, FILE * fp,int Query,int lower,int upper)
{
	struct non_leaf_page inter_nlp;
	struct student st;
	int offset = 0;
	if (nlp.type)
	{
		fseek(fp, -64, SEEK_CUR);
		fread(&st, sizeof(struct student), 1, fp);
		switch (Query)
		{
		case 2:
			if (st.rollno>0 && !strcmp(st.college, college_name))
				printf("\n%d,%s,%s", st.rollno, st.name, st.college);
			break;
		case 3:
			if (st.rollno>0 && st.rollno>=lower&&st.rollno<=upper)
				printf("\n%d,%s,%s", st.rollno, st.name, st.college);
			break;
		case 4:
			if (st.rollno>0 && st.rollno >= lower&&st.rollno <= upper&&!strcmp(st.college, college_name))
				printf("\n%d,%s,%s", st.rollno, st.name, st.college);
			break;

		}
	}
	else
	{
		offset = nlp.offset[0];
		fseek(fp, offset * 64, SEEK_SET);
		fread(&inter_nlp, sizeof(struct non_leaf_page), 1, fp);
		groupbycollege(college_name, inter_nlp, fp,Query,lower,upper);

		offset = nlp.offset[1];
		fseek(fp, offset * 64, SEEK_SET);
		fread(&inter_nlp, sizeof(struct non_leaf_page), 1, fp);
		groupbycollege(college_name, inter_nlp, fp, Query, lower, upper);
		
		offset = nlp.offset[2];
		fseek(fp, offset * 64, SEEK_SET);
		fread(&inter_nlp, sizeof(struct non_leaf_page), 1, fp);
		groupbycollege(college_name, inter_nlp, fp, Query, lower, upper);
	}
}


void Query_set(char * college_name, int Query, int lower, int upper)
{
	
	struct non_leaf_page nlp;
	FILE * fp = fopen("storage.bin", "rb");
	fseek(fp, 0, SEEK_END);
	int pos = ftell(fp);
	pos -= 64;
	fseek(fp, pos, SEEK_SET);
	fread(&nlp, sizeof(struct non_leaf_page), 1, fp);
	groupbycollege(college_name, nlp,fp,Query,lower,upper);
	fclose(fp);
}

int main()
{
	int choise,i,lower,upper;
	char college_name[50];
	FILE * fp = fopen("data_27.csv", "r");
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
			printf("\nEnter College Name");
			gets(college_name);
			Query_set(college_name,choise,0,0);
			break;
		case 3:
			printf("\nEnter Lower Range:");
			scanf("%d", &lower);
			printf("\nEnter Upper Range:");
			scanf("%d", &upper);
			Query_set(NULL,choise,lower,upper);
			break;
		case 4:
			printf("\nEnter Lower Range:");
			scanf("%d", &lower);
			printf("\nEnter Upper Range:");
			scanf("%d", &upper);
			printf("\nEnter College Name:");
			gets( college_name);
			Query_set(college_name,choise,lower,upper);
			break;
		case 5:exit(0);
		default:printf("\n--------Invalied Choise-----");
		}
	}

	fclose(fp);
	getch();
	return 0;
}
