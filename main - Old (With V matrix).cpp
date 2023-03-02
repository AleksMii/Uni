//free(root)
#define _CRT_SECURE_NO_WARNINGS
#define EPS 0.001
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void read_file(FILE* fp, int size, float* array)
{
	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < size; ++i)
	{
		fscanf(fp, "%f", &array[i]);
	}
}

int count_number(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	int counter = 0;
	while (1)
	{
		int value;
		if (fscanf(fp, "%d", &value) == 1)
			counter++;
		if (feof(fp))
			break;
	}
	return counter;
}

int phi(int* X, int x0, int i, int n)
{
	int val = 1;
	for (int j = 0; j < n; ++j)
	{
		if (j == i)
			continue;
		val *= (x0 - X[j]) / (X[i] - X[j]);
	}
	return val;
}

int pol(int x0, int* X, int* Y, int n)
{
	int val = 0;
	for (int j = 0; j < n; ++j)
	{
		val += Y[j] * phi(X, x0, j, n);
	}
	return val;
}


void key_element(int p, float** V, float* Y, int n, int* queue)
{
	int i_max = p, j_max = p;
	float temp;

	//Finding maximum abs element
	for (int i = p; i < n; ++i)
	{
		for (int j = p; j < n; ++j)
		{
			if (fabs(V[i_max][j_max]) < fabs(V[i][j]))
			{
				i_max = i;
				j_max = j;
			}
		}
	}
	
	//Changing rows' order
	for (int j = p; j < n; ++j)
	{
		temp = V[p][j];
		V[p][j] = V[i_max][j];
		V[i_max][j] = temp;
	}
	temp = Y[p];
	Y[p] = Y[i_max];
	Y[i_max] = temp;
	//Changing tables' order
	for (int i = 0; i < n; ++i)
	{
		temp = V[i][p];
		V[i][p] = V[i][j_max];
		V[i][j_max] = temp;
	}
	
	//Changing roots' order
	temp = queue[p];
	queue[p] = queue[j_max];
	queue[j_max] = temp;
	
}

float* slae(float** V, float* Y, int n)
{
	float* root = malloc(n * sizeof(float));
	float* ordered_root = malloc(n * sizeof(float));
	int* queue = malloc(n * sizeof(int));
	//int i, j, k;

	//Placing all roots in order
	for (int i = 0; i < n; ++i)
		queue[i] = i;


	//Forward move 
	for (int p = 0; p < n; ++p)
	{
		key_element(p, V, Y, n, queue);
		if (fabs(V[p][p]) < EPS)
		{
			printf("System does not have the only one solution");
			return 0;
		}


		//Deviding tables
		Y[p] /= V[p][p];
		for (int j = n - 1; j >= p; --j)
		{
			V[p][j] /= V[p][p];
		}
		//Deviding rows
		for (int i = p + 1; i < n; ++i)
		{
			Y[i] -= Y[p] * V[i][p];
			for (int j = n - 1; j >= p; --j)
				V[i][j] -= V[p][j] * V[i][p];
		}
	}


	//Reverse move
	for (int i = 0; i < n; ++i)
	{
		root[i] = Y[i];
	}
	for (int i = n - 2; i >= 0; --i)
		for (int j = i + 1; j < n; ++j)
		{
			root[i] -= root[j] * V[i][j];
		}

	
	//Result:
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
			if (i == queue[j])
			{
				//printf("%f\n", root[j]);
				ordered_root[i] = root[j];
				break;
			}
	}


	free(root);
	free(queue);
	return ordered_root;
}



float func(float x, float* ordered_root)
{
	return ordered_root[0] * pow(x, 3) + ordered_root[1] * pow(x, 2) + ordered_root[2] * x + ordered_root[3];
	//For test with google_collab:
	//return ordered_root[0] * pow(x, 4) + ordered_root[1] * pow(x, 3) + ordered_root[2] * pow(x, 2) + ordered_root[3] * x + ordered_root[4];
}

int main()
{
	FILE* fp1;
	if ((fp1 = fopen("X.txt", "rb")) == NULL)
	{
		printf("Error while opening file.\n");
		exit(1);
	}
	FILE* fp2;
	if ((fp2 = fopen("Y.txt", "rb")) == NULL)
	{
		printf("Error while opening file.\n");
		exit(1);
	}
	

	int n = count_number(fp1);
	if (n != count_number(fp2))
	{
		printf("Different points number");
		exit(1);
	}
	float* X = malloc(n * sizeof(float));
	float* Y = malloc(n * sizeof(float));

	read_file(fp1, n, X);
	read_file(fp2, n, Y);
	fclose(fp1);
	fclose(fp2);
	/*//Printing X & Y
	for (int i = 0; i < n; ++i)
	{
		printf("%f %f\n", X[i], Y[i]);
	}*/

	//Creating and filling gamma matrix
	float** G = malloc((n - 2) * sizeof(float*));
	for (int section = 0; section < n - 2; ++section)
	{
		G[section] = malloc(n - 2 * sizeof(float));
	}
	//for (int i = 0; i < n - 2; ++i)
	//{
	//	for (int j = 0; j < n - 2; ++j)
	//	{
	//		//if (j == 0 || j == n - 1)
	//		G[i][j] = 0;
	//		/*else*/ if (j == i - 1 || j == i + 1)
	//			G[i][j] = 1.0 / 6.0;
	//		else if (j == i)
	//			G[i][j] = 2.0 / 3.0;
	//		else
	//			G[i][j] = 0;
	//		printf("%f ", G[i][j]);
	//	}
	//	printf("\n");
	//}
	G[0][0] = 2.0 / 3.0;
	G[0][1] = 1.0 / 6.0;
	G[1][0] = 2.0 / 3.0;
	G[1][1] = 1.0 / 6.0;


	float* B = malloc(n * sizeof(float));
	for (int i = 0; i < n; ++i)
	{
		//if (i == 0)
		//	//B[i] = 0;
		//	B[i] = Y[i + 1] - Y[i] - Y[i];
		//else if (i == n - 1)
		//	//B[i] = 0;
		//	B[i] = -Y[i] - Y[i] - Y[i - 1];
		//else
		B[i] = (Y[i + 1] - Y[i]) - (Y[i] - Y[i - 1]);
		printf("%f\n", B[i]);
	}

	float* root = slae(G, B, n - 2);
	//Printing root
	for (int i = 0; i < n; ++i)
		printf("root[%d]=%f\n", i, root[i]);

	
	//// Creating and filling matrix with points
	//float** M = malloc(n * sizeof(float*));
	//for (int i = 0; i < n; ++i)
	//{
	//	M[i] = malloc(n * sizeof(float));
	//}
	//for (int i = 0; i < n; ++i)
	//{
	//	for (int j = 0; j < n; ++j)
	//	{
	//		if (j <= i + 1 && j >= i - 1)
	//			M[i][j] = X[j];
	//		else
	//			M[i][j] = 0;
	//		//printf("%f ", M[i][j]);
	//	}
	//	//printf("\n");
	//}
	//
	//float* B = malloc(n * sizeof(float));
	//B = Y;
	


	//// Creating and filling Vandermonde matrix
	//float** V = malloc(n * sizeof(float*));
	//for (int i = 0; i < n; ++i)
	//{
	//	V[i] = malloc(n * sizeof(float));
	//}
	//for (int i = 0; i < n; ++i)
	//{
	//	for (int j = 0; j < n; ++j)
	//	{
	//		if (i == 0 || j == n - 1) 
	//			V[i][j] = X[0];
	//		else
	//			V[i][j] = pow(X[i], (n - 1 - j));
	//		printf("%f	", V[i][j]);
	//	}
	//	printf("\n");
	//}


	//float* ordered_root = slae(V, Y, n);
	////Printing ordered_root
	//for (int i = 0; i < n; ++i)
	//{
	//	printf("%f!\n", ordered_root[i]);
	//}

	/*Test
	printf("\n");
	printf("%f\n", func(3.3, ordered_root));*/

	
	

	
	
	
	free(X);
	free(Y);
	//for (int i = 0; i < n; ++i)
	//{
	//	free(V[i]);
	//}
	//free(V);
	//free(ordered_root);
	return 0;
}









/*
For tests:
5 1 2 3 4 5 1 1.32 1.55 1.74 1.90
*/
