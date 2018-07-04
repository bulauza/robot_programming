#include <stdio.h>
#include <string.h>

#define SIZE 5
#define NUMBER_LEN 8
#define MAX_LEN 100

struct addressdate {

	char no[NUMBER_LEN];	// 学生番号
	char name[MAX_LEN];	// 氏名
	long phone;	// 電話番号
};

int main(void)
{
	int fg = 0; //　フラグ
	int i;	//構造体の添字

	FILE *fp;
	int a;
	int sci;
	char search_char[MAX_LEN];
	char baffer[MAX_LEN];
	long search_number;

	int search_len = 0;
	int ture = 1;		//一致判定、0でフラグオン

	struct addressdate member[SIZE];

	printf("入力するなら1\n");
	printf("リストするなら2\n");
	printf("検索するなら3\n");
	scanf("%d", &fg);

	switch(fg) {

		case 1	:
			fp = fopen("addressdata.txt","w");

			for(i = 0; i < SIZE; i++) {

				printf("\n-----------%d人目-----------\n", i+1);
				printf("学籍番号を入力してください。\n");
				scanf("%s", member[i].no);

				printf("\n氏名を入力してください。\n");
				scanf("%s", member[i].name);

				printf("\n電話番号を入力してください。\n");
				scanf("%ld", &member[i].phone);

				fprintf(fp,"%s\t", member[i].no);
				fprintf(fp,"%s\t", member[i].name);
				fprintf(fp,"%ld\n", member[i].phone);
			}

			fclose(fp);
			break;

		case 2	:
			fp = fopen("addressdata.txt", "r");

			printf("\n");
			printf("学籍番号\t電話番号\t氏名\n");

			for(i = 0; i < SIZE; i++) {

				fscanf(fp,"%s", member[i].no);
				fscanf(fp,"%s", member[i].name);
				fscanf(fp,"%ld", &member[i].phone);
				printf("%s\t\t%011ld\t%s\t\n",member[i].no, member[i].phone, member[i].name);
			}

			fclose(fp);
			break;

		case 3	:
			fp = fopen("addressdata.txt", "r");

			fg = 0;
			printf("\n何で検索しますか?\n");
			printf("1:学生番号\n2:氏名\n3:電話番号\n");
			scanf("%d",&fg);

			printf("\n入力してください。\n");
			if(fg == 1 || 2) {

				scanf("%s", search_char);

				for(sci = 0; search_char[sci] != '\0'; sci++) search_len++;	//入力された文字列の要素を検査
			}

			if(fg == 3) scanf("%ld", &search_number);


			for(i = 0; i < SIZE; i++) {

				fscanf(fp,"%s", member[i].no);
				fscanf(fp,"%s", member[i].name);
				fscanf(fp,"%ld", &member[i].phone);

				switch(fg) {

					case 1 :
						for(a = 0; a < search_len; a++)	baffer[a] = member[i].no[a];
						baffer[a] = '\0';
						ture = strcmp(search_char, baffer);	//文字列検索関数、一致で0を返す
						break;

					case 2 :
						for(a = 0; a < search_len; a++)	baffer[a] = member[i].name[a];
						baffer[a] = '\0';
						ture = strcmp(search_char, baffer);
						break;

					case 3 :
						if(search_number == member[i].phone) ture = 0;
						break;
				}

				if(ture == 0) {
					printf("\n学籍番号:%s\t電話番号:%011ld\t氏名:%s\n", member[(i)].no, member[(i)].phone,
					member[(i)].name);
				}
			}

			fclose(fp);
			break;

		default	:
			fg = 0;
			break;
	}

	return 0;
}
