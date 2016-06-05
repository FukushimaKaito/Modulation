/*-------------------------------------------
変復調プログラム

5I30	福嶋開人
------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
//M_PI(円周率)を使えるようにする
#define _USE_MATH_DEFINES
#include <math.h>

#define L 500
#define Fs 44.1e3//サンプリング周波数
#define Fc 8.0e3//キャリア周波数
#define g1 100.0//ベースバンド周波数1
#define g2 200.0//ベースバンド周波数2

//グローバル変数
double t[L],//時刻
b1[3][L],//ベースバンド信号1
b2[3][L],//ベースバンド信号2
Re[L],//変調信号実部
Im[L],//変調信号虚部
m[L],//変調信号
TapSig[2][4095],
*Coe;

int Num;

/*関数プロトタイプ宣言*/
void LPF(int);

int main(void){
	FILE *fpr;

	/*フィルタファイルの取得*/
	if ((fpr = fopen("LPF.txt", "r")) == NULL){
		fprintf(stderr, "フィルタファイルをopenできません。\n");
		exit(1);
	}
	fscanf(fpr, "%d", &Num);  /* データの個数取得 */
	if ((Coe = (double *)malloc(Num*sizeof(double))) == NULL)
	{
		fprintf(stderr, "メモリの確保に失敗しました。\n");
		exit(1);
	}
	for (int i = 0; i < Num; i++)/* 係数読み込み */
	{
		fscanf(fpr, "%lf", &Coe[i]);
	}
	fclose(fpr);

	//for (int i = 0; i<Num; i++){      /* 読み込んだ値を表示してみる */
	//	printf("%23.15e\n", Coe[i]); /* 小数点以下15桁 */
	//}

	printf("変復調プログラム\n");
	printf("\n");
	printf("t[n]		 b1[0][n]	b2[0][n]	Re[n]		Im[n]		m[n]		b1[1][n]	b2[1][n]	b1[2][n]	b2[2][n]\n");
	for (int n = 0; n < L; n++)
	{
		//時刻t
		t[n] = n * (1 / Fs);//#1
		//ベースバンド信号
		b1[0][n] = cos(2 * M_PI * g1 * t[n]);//g1=cos(wt)	//#2
		b2[0][n] = cos(2 * M_PI * g2 * t[n]);//g2=sin(wt)	//#3
		//直交変調信号
		Re[n] = b1[0][n] * cos(2 * M_PI * Fc * t[n]);//g1(t)cos(wct)	//#4
		Im[n] = b2[0][n] * -sin(2 * M_PI * Fc * t[n]);//g2(t)-sin(wct)	//#5
		m[n] = Re[n] + Im[n];//#6
		//仮復調信号
		b1[1][n] = m[n] * cos(2 * M_PI * Fc * t[n]);//#7
		b2[1][n] = m[n] * -sin(2 * M_PI * Fc * t[n]);//#8

		//LPF
		LPF(n);//#9	//#10

		//表示
		printf("%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n", t[n], b1[0][n], b2[0][n], Re[n], Im[n], m[n], b1[1][n], b2[1][n], b1[2][n], b2[2][n]);
	}
	return 0;
}

/* LPFフィルタ */
void LPF(int n)
{
	/*--タップ信号シフト--*/
	for (int i = 0; i < Num - 1; i++){
		TapSig[0][Num - 1 - i] = TapSig[0][Num - 2 - i];
		TapSig[1][Num - 1 - i] = TapSig[1][Num - 2 - i];
	}
	TapSig[0][0] = b1[1][n];
	TapSig[1][0] = b2[1][n];
	/*--フィルタリング--*/
	b1[2][n] = 0.0;
	b2[2][n] = 0.0;
	for (int i = 0; i < Num; i++){
		b1[2][n] += TapSig[0][i] * Coe[i];
		b2[2][n] += TapSig[1][i] * Coe[i];
	}

	/* フィルタリング値を返す */
	b1[2][n] *= 2;
	b2[2][n] *= 2;
}