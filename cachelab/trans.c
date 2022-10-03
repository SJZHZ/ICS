//1900017702 朱越

/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
	int i, j, k;
	int x1, x2, x3, x4, x5, x6, x7, x8;
	if (M == 32)
	{
		for (i = 0; i < 32; i += 8)
			for (j = 0; j < 32; j += 8)
				for (k = i; k < i + 8; k++)//每次B不会同组，可以简单分块
				{
					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					x5 = A[k][j + 4];
					x6 = A[k][j + 5];
					x7 = A[k][j + 6];
					x8 = A[k][j + 7];
					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;
					B[j + 4][k] = x5;
					B[j + 5][k] = x6;
					B[j + 6][k] = x7;
					B[j + 7][k] = x8;

				}
	}
	if (M == 64)
	{
		for (i = 0; i < 64; i += 8)
			for (j = 0; j < 64; j += 8)
			{
				//不能直接分8*8的块，因为每跨四行之间是同组的，每块最多四行，最多八列
				//本质上还是一个4*4的块，只不过利用了它在8*8中的一些性质
				//第一步解决左上角
				//反正右上角和左上角同处一块，在不增加miss的情况下，可以用B的右上角暂存A的右上角的数据
					//（虽然填的位置是不对的）
				//这部分错位的数据在第二步可以发挥作用：
					//（本该各自独立，先解决右上角再解决左下角）
				//不过我们已经把左下角该有的存到右上角了
				//所以我们可以在要写右上角之前，把它的值读出来
				//反正它要被写，提前读不会增加miss
				//读出来之后，可以留待后面写左下角

				//相当于解决左下角时，读是免费的！


				for (k = i; k < i + 4; k++)	//复制一行到两个半列
				{
					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					x5 = A[k][j + 4];
					x6 = A[k][j + 5];
					x7 = A[k][j + 6];
					x8 = A[k][j + 7];

					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;
					B[j][k + 4] = x5;
					B[j + 1][k + 4] = x6;
					B[j + 2][k + 4] = x7;
					B[j + 3][k + 4] = x8;
				}
								//关键！可以在取的同时存，有效减少miss
				for (k = 0; k < 4; k++)
				{
					x5 = A[i + 4][j + k];
					x6 = A[i + 5][j + k];
					x7 = A[i + 6][j + k];
					x8 = A[i + 7][j + k];
								//取出右上一行&更新为正确值
					x1 = B[j + k][i + 4];
					B[j + k][i + 4] = x5;
					x2 = B[j + k][i + 5];
					B[j + k][i + 5] = x6;
					x3 = B[j + k][i + 6];
					B[j + k][i + 6] = x7;
					x4 = B[j + k][i + 7];
					B[j + k][i + 7] = x8;
								//旧的值放到左下
					B[j + 4 + k][i] = x1;
					B[j + 4 + k][i + 1] = x2;
					B[j + 4 + k][i + 2] = x3;
					B[j + 4 + k][i + 3] = x4;
				}
				for (k = j + 4; k < j + 8; k++)	//行列反过来略微变差，还没想出来为啥
				{
					x1 = A[i + 4][k];
					x2 = A[i + 5][k];
					x3 = A[i + 6][k];
					x4 = A[i + 7][k];
					B[k][i + 4] = x1;
					B[k][i + 5] = x2;
					B[k][i + 6] = x3;
					B[k][i + 7] = x4;
				}
			}
	}
	if (M == 60)//M=60,N=68
	{
								//68*60恰好使得8*8分块地址不会重复
								//压缩思路：
								//1按行复制到列和按列复制到行，任意组合
								//2增加空间局部性：走迷宫
								//3末尾改编：8个一组，发现没有更好

        for (i = 0; i < 64; i += 8)
        {
	//8*8分块部分
            for (j = 0; j < 56; j += 8)
                for (k = j; k < j + 8; k++)
                {
                    x1 = A[i][k];
                    x2 = A[i + 1][k];
                    x3 = A[i + 2][k];
                    x4 = A[i + 3][k];
                    x5 = A[i + 4][k];
                    x6 = A[i + 5][k];
                    x7 = A[i + 6][k];
                    x8 = A[i + 7][k];
                    B[k][i] = x1;
                    B[k][i + 1] = x2;
                    B[k][i + 2] = x3;
                    B[k][i + 3] = x4;
                    B[k][i + 4] = x5;
                    B[k][i + 5] = x6;
                    B[k][i + 6] = x7;
                    B[k][i + 7] = x8;
                }
            //j = 56;
	//行末的小块
            for (k = j; k < j + 4; k++)
            {
                    x1 = A[i][k];
                    x2 = A[i + 1][k];
                    x3 = A[i + 2][k];
                    x4 = A[i + 3][k];
                    B[k][i] = x1;
                    B[k][i + 1] = x2;
                    B[k][i + 2] = x3;
                    B[k][i + 3] = x4;
            }
            for (k = j; k < j + 4; k++)
            {
                    x1 = A[i + 4][k];
                    x2 = A[i + 5][k];
                    x3 = A[i + 6][k];
                    x4 = A[i + 7][k];
                    B[k][i + 4] = x1;
                    B[k][i + 5] = x2;
                    B[k][i + 6] = x3;
                    B[k][i + 7] = x4;
            }
        }
        //i = 64;
	//最后一小行
        for (j = 56; j >= 0; j -= 4)
            for(k = j; k < j + 4; k++)
            {
                    x1 = A[i][k];
                    x2 = A[i + 1][k];
                    x3 = A[i + 2][k];
                    x4 = A[i + 3][k];
                    B[k][i] = x1;
                    B[k][i + 1] = x2;
                    B[k][i + 2] = x3;
                    B[k][i + 3] = x4;                
            }

//6+6比8+4要差，可能是因为第二个6会跨页？
/*
备用代码段
		j = 56;
		for (i = 0; i < 64; i += 4)
			for (k = i; k < i + 4; k++)
			{
					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;
			}

					x1 = A[i][k];
					x2 = A[i + 1][k];
					x3 = A[i + 2][k];
					x4 = A[i + 3][k];
					B[k][i] = x1;
					B[k][i + 1] = x2;
					B[k][i + 2] = x3;
					B[k][i + 3] = x4;

					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;


8+4尝试改6+6，变差
		for (j = 0; j < 60; j += 6)
			for(i = 56; i < 68; i += 6)
			for(k = i; k < i + 6; k++)
			{
					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					x5 = A[k][j + 4];
					x6 = A[k][j + 5];
					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;				
					B[j + 4][k] = x5;				
					B[j + 5][k] = x6;				
			}
		for (i = 0; i < 56; i += 6)
			for(j = 48; j < 60; j += 6)
			for (k = i; k < i + 6; k++)
			{
					x1 = A[k][j];
					x2 = A[k][j + 1];
					x3 = A[k][j + 2];
					x4 = A[k][j + 3];
					x5 = A[k][j + 4];
					x6 = A[k][j + 5];
					B[j][k] = x1;
					B[j + 1][k] = x2;
					B[j + 2][k] = x3;
					B[j + 3][k] = x4;				
					B[j + 4][k] = x5;				
					B[j + 5][k] = x6;
			}
*/
	}
    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

 /*
  * trans - A simple baseline transpose function, not optimized for the cache.
  */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

