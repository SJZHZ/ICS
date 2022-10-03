//1900017702 朱越
#include"cachelab.h"
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

int s, E, b;
int S, B;
char t[100];
int hc = 0, ec = 0, mc = 0;
//要设计一种结构，包含标记位F、时间戳T
//然后分配一个数组A[S][E]
typedef struct
{
	int Ti;//时间戳&标记位
	long flag;//标记位
}CacheBlock;


CacheBlock* Pointer = NULL;//一维数组指针
CacheBlock** PP = NULL;//二维数组指针
void init()//初始化，分配空间并设置时间戳&有效位
{
	S = 1 << s;
	B = 1 << B;
	Pointer = (CacheBlock*)malloc(sizeof(CacheBlock) * S * E);//一维数组
	PP = (CacheBlock**)malloc(sizeof(CacheBlock*) * S);//指针数组
	CacheBlock* temp = Pointer;
	for (int i = 0; i < S; i++, temp += E)
		PP[i] = temp;//指针数组初始化
	for (int i = 0; i < S * E; i++)
		Pointer[i].Ti = 1 << 20;//一维数组初始化
	return;
}
void fini()//结束
{
	free(PP);
	free(Pointer);
	return;
}
void timeplus()//更新时间戳
{
	for (int i = 0; i < S * E; i++)
		Pointer[i].Ti++;
	return;
}
int LRU(int SS)//输入第SS组，给出其中的LRU结果(找到时间戳最大位置)
{
	int temp = 0;
	int MTi = PP[SS][0].Ti;
	for (int i = 1; i < E; i++)
		if (PP[SS][i].Ti > MTi)
		{
			temp = i;
			MTi = PP[SS][i].Ti;
		}
	return temp;
}

void VISIT(long ad, int size)//对给定地址和大小（实际上没有跨页），访问高速缓存
{
	//long lastgroup = -1;
	//for (int j = 0; j < size; j++)		//这个循环是为了size很大而B很小而设计的
	//{
		long Flag = ad >> (b + s);//标记位
		long Group = (ad >> b) & (S - 1);//组索引
		//long Bias = ad & (B - 1);//块偏移
		//if (lastgroup == Group)//如果这个字节和上一个还在同一组
		//	continue;
		//lastgroup = Group;
		for (int i = 0; i < E; i++)//检测是否名中
			if ((PP[Group][i].flag == Flag) && (PP[Group][i].Ti < (1 << 20)))//命中
			{
				hc++;
				PP[Group][i].Ti = 0;
				return;
			}
		mc++;//不命中
		int temp = LRU(Group);
		if (PP[Group][temp].Ti < (1 << 20))//不是冷不命中
			ec++;
		PP[Group][temp].Ti = 0;
		PP[Group][temp].flag = Flag;
		//ad++;
	//}
	//timeplus();
	return;
}


void ReadaLine()//按行读入文件
{
	FILE* fp = NULL;
	fp = fopen(t, "r");
	char c;
	long ad;
	int size;
	while(fscanf(fp, "%c%lx,%d\n", &c, &ad, &size) != EOF)
	{
		if (c == 'M')
		{
			VISIT(ad,size);
			VISIT(ad,size);
		}
		if (c == 'S' || c == 'L')
			VISIT(ad,size);
		timeplus();
	}
	fclose(fp);
	return;
}


int main(int argc, char* const argv[])
{
	int ch;
	while((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1)
	{
		switch(ch)
		{
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				strcpy(t, optarg);
				break;
			default:
				break;
		}
	}
	init();
	ReadaLine();
	fini();
	printSummary(hc, mc, ec);
	return 0;
}
