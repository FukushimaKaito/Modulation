/*-------------------------------------------
�ϕ����v���O����

5I30	�����J�l
------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
//M_PI(�~����)���g����悤�ɂ���
#define _USE_MATH_DEFINES
#include <math.h>

#define L 500
#define Fs 44.1e3//�T���v�����O���g��
#define Fc 8.0e3//�L�����A���g��
#define g1 100.0//�x�[�X�o���h���g��1
#define g2 200.0//�x�[�X�o���h���g��2

//�O���[�o���ϐ�
double t[L],//����
b1[3][L],//�x�[�X�o���h�M��1
b2[3][L],//�x�[�X�o���h�M��2
Re[L],//�ϒ��M������
Im[L],//�ϒ��M������
m[L],//�ϒ��M��
TapSig[2][4095],
*Coe;

int Num;

/*�֐��v���g�^�C�v�錾*/
void LPF(int);

int main(void){
	FILE *fpr;

	/*�t�B���^�t�@�C���̎擾*/
	if ((fpr = fopen("LPF.txt", "r")) == NULL){
		fprintf(stderr, "�t�B���^�t�@�C����open�ł��܂���B\n");
		exit(1);
	}
	fscanf(fpr, "%d", &Num);  /* �f�[�^�̌��擾 */
	if ((Coe = (double *)malloc(Num*sizeof(double))) == NULL)
	{
		fprintf(stderr, "�������̊m�ۂɎ��s���܂����B\n");
		exit(1);
	}
	for (int i = 0; i < Num; i++)/* �W���ǂݍ��� */
	{
		fscanf(fpr, "%lf", &Coe[i]);
	}
	fclose(fpr);

	//for (int i = 0; i<Num; i++){      /* �ǂݍ��񂾒l��\�����Ă݂� */
	//	printf("%23.15e\n", Coe[i]); /* �����_�ȉ�15�� */
	//}

	printf("�ϕ����v���O����\n");
	printf("\n");
	printf("t[n]		 b1[0][n]	b2[0][n]	Re[n]		Im[n]		m[n]		b1[1][n]	b2[1][n]	b1[2][n]	b2[2][n]\n");
	for (int n = 0; n < L; n++)
	{
		//����t
		t[n] = n * (1 / Fs);//#1
		//�x�[�X�o���h�M��
		b1[0][n] = cos(2 * M_PI * g1 * t[n]);//g1=cos(wt)	//#2
		b2[0][n] = cos(2 * M_PI * g2 * t[n]);//g2=sin(wt)	//#3
		//����ϒ��M��
		Re[n] = b1[0][n] * cos(2 * M_PI * Fc * t[n]);//g1(t)cos(wct)	//#4
		Im[n] = b2[0][n] * -sin(2 * M_PI * Fc * t[n]);//g2(t)-sin(wct)	//#5
		m[n] = Re[n] + Im[n];//#6
		//�������M��
		b1[1][n] = m[n] * cos(2 * M_PI * Fc * t[n]);//#7
		b2[1][n] = m[n] * -sin(2 * M_PI * Fc * t[n]);//#8

		//LPF
		LPF(n);//#9	//#10

		//�\��
		printf("%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n", t[n], b1[0][n], b2[0][n], Re[n], Im[n], m[n], b1[1][n], b2[1][n], b1[2][n], b2[2][n]);
	}
	return 0;
}

/* LPF�t�B���^ */
void LPF(int n)
{
	/*--�^�b�v�M���V�t�g--*/
	for (int i = 0; i < Num - 1; i++){
		TapSig[0][Num - 1 - i] = TapSig[0][Num - 2 - i];
		TapSig[1][Num - 1 - i] = TapSig[1][Num - 2 - i];
	}
	TapSig[0][0] = b1[1][n];
	TapSig[1][0] = b2[1][n];
	/*--�t�B���^�����O--*/
	b1[2][n] = 0.0;
	b2[2][n] = 0.0;
	for (int i = 0; i < Num; i++){
		b1[2][n] += TapSig[0][i] * Coe[i];
		b2[2][n] += TapSig[1][i] * Coe[i];
	}

	/* �t�B���^�����O�l��Ԃ� */
	b1[2][n] *= 2;
	b2[2][n] *= 2;
}