#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STACK_SIZE 1024
#define Pl 3.1415926

int subs = 0;	//outtextの添字
int error = 0;	//エラーフラグ、１でフラグon

void set_outtext(char*, const char);
void set_outtext(char *str, const char x) {

	str[subs] = x;
	subs++;
}

int str_length(const char s[]);
int str_length(const char s[]) {	//入力された数式の長さを測定（'=','\0'を含む）

	int length;

	for(length = 0; s[length] != '\0'; length++);

	if(s[length - 1] != '=') {

		length++;
	}

	return length;
}

double calculation(const char *, int, double);		//四則演算関数を宣言
double calculation(const char *s, int x, double answer) {

	char cptext[x];				//引数配列のコピー先
	char *d = cptext;
	char outtext[STACK_SIZE];		//出力先配列
	char *t = outtext;
	int judge_operator[STACK_SIZE];		//演算子の優先順位を記録

	char stack[STACK_SIZE][2];		//スタック配列。スタックした演算子とその優先度を記録。
	int n = 0;				//スタック配列用の添字

	double number[STACK_SIZE];		//数字をstackし、演算
	int fg = 1;				//フラグオンでatoi関数に値を渡す
	int i = 0;				//走査用の添字

	//演算の準備

	while(*d++ = *s++);		//引数配列をcptextへコピー

	if(cptext[x - 1] != '=') {	//入力された数式に'='がない場合付け加えてコピー

		cptext[x - 1] = '=';
		cptext[x] = '\0';
	}

	//デバッグ用
	//printf("%c\n", cptext[x]);
	//printf("%s\n",cptext);

	for(i = 0; i < STACK_SIZE; i++) {				//各配列の初期化

		judge_operator[i] = 1;
		stack[i][0] = 0;
		stack[i][1] = 0;
	}


	//入力された中置記法の数列を逆ポーランド記法へ変換する


	for(i = 0; cptext[i] != '\0'; i++) {

		switch(cptext[i]) {		//演算子を優先度付

			case '+'	:
			case '-'	:	judge_operator[i] = 2;	break;
			case '*'	:	judge_operator[i] = 3;	break;

			case '/'	:	if(cptext[i + 1] == '0') {

							printf("0では割れません。\n");
							error = 1;			//エラーフラグオン

							return 0;
						}

						judge_operator[i] = 4;	break;
			case '('	:	judge_operator[i] = 5;	break;
			case ')'	:	judge_operator[i] = 6;	break;
			case '='	:	judge_operator[i] = 7;	break;
		}

		if(judge_operator[i] == 1) {			//演算子以外は未出力スペースへそのまま出力

			set_outtext(outtext, cptext[i]);
		}

		else if(judge_operator[i] < 7) {			//演算子の場合

			if(i == 0) {

				set_outtext(outtext, 'A');
				set_outtext(outtext, 'N');
				set_outtext(outtext, 'S');
			}

			if(judge_operator[i] == 5) {			//'('のとき

				set_outtext(outtext, ' ');

				if(judge_operator[i - 1] == 1) {	//'*'を省略された場合

					if(stack[n][1] < 3) {		//スタック先頭より'*'の優先度が高いなら
									//演算子とその優先度をスタックにプッシュ
						n++;
						stack[n][0] = '*';		//'*'をプッシュ
						stack[n][1] = 3;		//優先度をプッシュ

						set_outtext(outtext, ' ');
						subs++;
					}

					else {							//スタック先頭より優先が高くないとき

						for(;n > 0 && judge_operator[i] <= stack[n][1]; n--) {	//スタック先頭の優先度が低くなるまでポップ

							set_outtext(outtext, ' ');
							set_outtext(outtext, stack[n][0]);
							set_outtext(outtext, ' ');
							stack[n][0] = 0;	//出力した演算子を削除
							stack[n][1] = 0;	//出力した演算子の優先度を初期化
						}

						n++;
						stack[n][0] = '*';			//演算子をプッシュ
						stack[n][1] = 3;			//優先度をプッシュ
					}
				}
				
				n++;
				stack[n][0] = cptext[i];		//演算子をプッシュ
				stack[n][1] = 0;			//優先度は邪魔なので初期化
			}

			else if(judge_operator[i] == 6) {		//')'のとき

				set_outtext(outtext, ' ');

				for(;n > 0 && stack[n][0] != '('; n--) {	//スタックをブッシュ

					set_outtext(outtext, stack[n][0]);
					stack[n][0] = 0;	//出力した演算子を初期化
					stack[n][1] = 0;	//出力した演算子の優先度を初期化
				}

				stack[n][0] = 0;
				n--;
			}

			else if(judge_operator[i] > stack[n][1]) {		//スタック先頭より優先度が高いなら演算子とその優先度をスタックにプッシュ

				n++;
				stack[n][0] = cptext[i];		//演算子をプッシュ
				stack[n][1] = judge_operator[i];	//優先度をプッシュ

				set_outtext(outtext, ' ');
			}

			else {							//スタック先頭より優先が低いとき

				//デバッグ用
				//printf("b\n%s\n",outtext);

				for(;n > 0 && judge_operator[i] <= stack[n][1]; n--) {	//スタック先頭の優先度が低くなるまでポップ

					set_outtext(outtext, ' ');
					set_outtext(outtext, stack[n][0]);
					set_outtext(outtext, ' ');
					stack[n][0] = 0;	//出力した演算子を削除
					stack[n][1] = 0;	//出力した演算子の優先度を初期化
				}

				stack[++n][0] = cptext[i];			//演算子をプッシュ
				stack[n][1] = judge_operator[i];		//優先度をプッシュ
			}
		}

		else {		//'='の時

			for(;n > 0; n--) {	//スタックをブッシュ

				set_outtext(outtext, ' ');
				set_outtext(outtext, stack[n][0]);
			}

			set_outtext(outtext, ' ');
			set_outtext(outtext, '=');
			set_outtext(outtext, '\0');
		}
	}

	//デバッグ用
	//printf("answer = %f\n", answer);
	//printf("%c\n", outtext[0]);
	//printf("%s\n", outtext);

	//ここから逆ポーランド記法にされた数式の演算を行う

	*t = *outtext;

	for(n = 0; *t != '\0'; t++) {

		switch(*t) {

			case '+'	:	n--;	number[n - 1] += number[n];	number[n] = 0;	break;
			case '-'	:	n--;	number[n - 1] -= number[n];	number[n] = 0;	break;
			case '*'	:	n--;	number[n - 1] *= number[n];	number[n] = 0;	break;
			case '/'	:	n--;	number[n - 1] /= number[n];	number[n] = 0;	break;
			case ' '	:	fg = 1;	break;
			case '='	:	fg = 0;	break;

			case 's'	:	t++;

						if(*t == 'i') {

							t++;

							if(*t == 'n' && fg) {

								t++;
								number[n] = sin( atoi( t ) * Pl / 180.0);

								//デバッグ用
								//printf("%s\n",t);
								//printf("sin t = %d\n",atoi(t));
								//printf("sin = %.3f\n", number[n]);
								//printf("%d\n",n);

								n++;
								fg = 0;
							}
						}

						break;

			case 'c'	:	t++;

						if(*t == 'o') {

							t++;

							if(*t == 's' && fg) {

								t++;
								number[n] = cos( atoi( t ) * Pl / 180.0);
								n++;
								fg = 0;
							}
						}

						break;

			case 't'	:	t++;

						if(*t == 'a') {

							t++;

							if(*t == 'n' && fg) {

								t++;
								number[n] = tan( atoi( t ) * Pl / 180.0);
								n++;
								fg = 0;
							}
						}

						break;

			case	'A'	:	t++;

						if(*t == 'N') {

							t++;

							if(*t == 'S') {

								number[n] = answer;
								n++;
							}
						}

						break;

			default		:	if(fg) {

							number[n] = atoi( t );
							fg = 0;
							n++;

							//デバッグ用
							//printf("%d\n",outtext[i]);
							//printf("%s\n",outtext);
							//printf("%g\n",number[n - 1]);
							//printf("%d\n",n);
						}

						break;
		}
	}

	//デバッグ用
	//printf("%d\n",n);

	return number[n - 1];
}

int main(void)
{
	int fn;		//使いたい機能を判別するための数字を格納する
	char str[STACK_SIZE];	//入力された数式を格納
	int len;		//入力された数式の長さを格納
	double ans;		//前回の演算結果を格納

	double n = 0;

	while(1) {

		printf("使用したい機能の数字を入力してください。\n");
		printf("1 : 四則演算\n");
		printf("2 : メートル単位変換\n");
		scanf("%d" , &fn);

		switch(fn) {

			case 1 :	printf("数式を入力してください。終了するならばcを入力してください。\n");

					while(1) {

						scanf("%s" , str);

						if(str[0] == 'c' && str[1] != 'o') {

							printf("終了しました。\n");

							return 0;
						}

						len = str_length(str);

						//デバッグ用
						//printf("%d\n", len);

						ans = calculation(str, len, ans);

						if(error) {

							printf("入力が不正です。\n");
							error = 0;

							break;
						}

						if(str[len - 1] != '=') {

							printf("=");
						}

						printf("%.1f\n\n", ans);
						subs = 0;
					}

					break;

			case 2 :	fn = 0;
					printf("変換する長さの単位の数字を入力してください。\n");
					printf("1 : cm → in\n");
					printf("2 : m → ft\n");
					printf("3 : m → yd\n");
					printf("4 : km → mile\n");
					printf("5 : m → 尺\n");
					printf("6 : km → 海里\n");
					printf("7 : 戻る\n");
					printf("8 : 終了\n");
					scanf("%d", &fn);

					switch(fn) {

						case 1	:	printf("何cm？\n");
								scanf("%lf", &n);
								printf("cm→ %.3lfin\n", n/2.54);	break;

						case 2	:	printf("何m？\n");
								scanf("%lf", &n);
								printf("m→ %.3lfft\n", n/0.3048);	break;

						case 3	:	printf("何m？\n");
								scanf("%lf", &n);
								printf("m→ %.3lfyd\n", n/0.9144);	break;

						case 4	:	printf("何km？\n");
								scanf("%lf", &n);
								printf("km→ %.3lfmile\n", n/1.60934);	break;

						case 5	:	printf("何m？\n");
								scanf("%lf", &n);
								printf("m→ %.3lf尺\n", n/0.30303);	break;

						case 6	:	printf("何km？\n");
								scanf("%lf", &n);
								printf("km→ %.3lf海里\n", n/1.852);	break;

						case 7	:	break;

						case 8	:	return 0;
					}

			default	:		printf("入力が不正です。\n");
				
						return 0;
		}
	}

	return 0;
}
