#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define bool int
#define false 0
#define true 1
#define N 4
#define KN 15
#define TEST_SIZE 65
#define TRAIN_SIZE 85
int dataNum;
typedef struct {
    double value;
    int clusterID;
} Distance;
typedef struct {
    double sepalLength;
    double sepalWidth;
    double petalLength;
    double petalWidth;
    int clusterID;
} Iris;
Iris temp[150];
Iris testSet[TEST_SIZE];
Iris forecastSet[TEST_SIZE];
Iris trainSet[TRAIN_SIZE];
Distance distance[TRAIN_SIZE];
void makeRand(Iris iris[], int n)
{
    Iris t;
    int i, n1, n2;
    srand((unsigned int)time(NULL));
    for (i = 0; i < n; i++)
    {
        n1 = (rand() % n);
        n2 = (rand() % n);
        if (n1 != n2)
        {
            t = iris[n1];
            iris[n1] = iris[n2];
            iris[n2] = t;
        }
    }
}
void label123(char *type, int *ID)
{
    if (strcmp(type, "Iris-setosa") == 0)
        *ID = 1;
    if (strcmp(type, "Iris-versicolor") == 0)
        *ID = 2;
    if (strcmp(type, "Iris-virginica") == 0)
        *ID = 3;
}
bool Inputs(){
    char fname[256];
    char name[20];
    printf("请输入存放数据的文件名:");
    scanf("%s",fname);
    printf("\n样本数目:\n");
    scanf("%d",&dataNum);
    FILE *fp= fopen(fname,"rb");
    if(fp==NULL){
        printf("不能打开输入的文件\n");
        return false;
    }
    for (int i = 0; i < dataNum; ++i) {
        fscanf(fp,"%lf,%lf,%lf,%lf,%s",&temp[i].sepalLength,&temp[i].sepalWidth,&temp[i].petalLength,&temp[i].petalWidth,name);
        label123(name,&temp[i].clusterID);
    }
    makeRand(temp,150);
    fclose(fp);
    return true;
}
void loadData()
{
    int i,n = 0, m = 0;
    for (i = 0; i < 150; i++)
    {
        if (i < TEST_SIZE)
        {
            testSet[n].sepalLength=temp[i].sepalLength;
            testSet[n].sepalWidth=temp[i].sepalWidth;
            testSet[n].petalLength=temp[i].petalLength;
            testSet[n].petalWidth=temp[i].petalWidth;
            testSet[n].clusterID = temp[i].clusterID;
            n++;
        }
        else
        {
            trainSet[m].sepalLength=temp[i].sepalLength;
            trainSet[m].sepalWidth=temp[i].sepalWidth;
            trainSet[m].petalLength=temp[i].petalLength;
            trainSet[m].petalWidth=temp[i].petalWidth;
            trainSet[m].clusterID = temp[i].clusterID;
            m++;
        }
    }
}
double EuclideanDistance(Iris *iris1,Iris *iris2, int n)
{
    double result = 0.0;
    result += pow(iris1->sepalLength - iris2->sepalLength, 2.0);
    result += pow(iris1->sepalWidth - iris2->sepalWidth, 2.0);
    result += pow(iris1->petalLength - iris2->petalLength, 2.0);
    result += pow(iris1->petalWidth - iris2->petalWidth, 2.0);
    result = sqrt(result);
    return result;
}
int compareID(int a, int b, int c)
{
    if (a > b && a > c)
    {
        return 1;
    }
    if (b > a && b > c)
    {
        return 2;
    }
    if (c > a && c > b)
    {
        return 3;
    }
    return 0;
}
int countID(int *count, int k, int forecastID)
{
    int i;
    int sumA = 0, sumB = 0, sumC = 0; //分别统计距离最邻近的三类标签出现的频数
    for (i = 0; i < k; i++)
    {
        switch (distance[i].clusterID)
        {
            case 1:
                sumA++;
                break;
            case 2:
                sumB++;
                break;
            case 3:
                sumC++;
                break;
        }
    }
    /* 检测出现频数最高的标签与测试集标签是否相等 */
    int maxID = compareID(sumA, sumB, sumC);
    if (maxID == forecastID)
    {
        (*count)++;
    }
    return maxID;
}
int cmp(const void *d1, const void *d2)
{
    Distance D1 = *(Distance *)d1;
    Distance D2 = *(Distance *)d2;
    return D1.value > D2.value ? 1 : -1;
}
void Sort(Distance d1[],int num)
{
    int i,j;
    Distance temp;
    for(i=0;i<num;i++)
    {
        for(j=num-1;j>i;j--)
        {
            if(d1[j].value<d1[j-1].value)
            {
                temp=d1[j];
                d1[j]=d1[j-1];
                d1[j-1]=temp;
            }
        }
    }
}
void printResult(int k, int count)
{
    int i;
    printf("对比结果:\n");
    /* 打印每个K值对应的概率 */
    printf("K = %d     P = %.2lf%%\n", k, (100.0 * count) / TEST_SIZE);
    printf("原有标签:");
    printf("[%d", testSet[0].clusterID);
    for (i = 1; i < TEST_SIZE; i++)
        printf(",%d", testSet[i].clusterID);
    printf("]\n");
    printf("预测标签:");
    printf("[%d", forecastSet[0].clusterID);
    for (i = 1; i < TEST_SIZE; i++)
        printf(",%d", forecastSet[i].clusterID);
    printf("]\n\n");
}
int main()
{
    int i, j;
    int k;		   // k值
    int count = 0; //用于统计预测正确的标签数量
    Inputs();
    loadData();
    printf("\n\n测试集:%d组  训练集:%d组\n\n", TEST_SIZE, TRAIN_SIZE);

    for (k = 1; k <= KN; k += 2) // k值：1--KN(取奇数)  KN = 15(宏定义)
    {
        for (i = 0; i < TEST_SIZE; i++) // 遍历测试集
        {
            for (j = 0; j < TRAIN_SIZE; j++) // 遍历训练集
            {
                /* 把计算欧几里得距离依次存入distance结构体数组的value中 */
                distance[j].value = EuclideanDistance(&testSet[i], &trainSet[j], N);
                /* 将训练集标签与计算好的距离绑定在一块 */
                distance[j].clusterID = trainSet[j].clusterID;
            }
            /* 用qsort函数从小到大排序(距离,训练集标签) */
            //qsort(distance, TRAIN_SIZE, sizeof(distance[0]), cmp);
            Sort(distance,TRAIN_SIZE);
            /* 统计与测试集标签距离最邻近的k个标签出现的频数 并返回频数最后高标签 即预测的标签 */
            forecastSet[i].clusterID = countID(&count, k, testSet[i].clusterID);
        }
        /* 打印结果 */
        printResult(k, count);
        count = 0; //重置
    }
    return 0;
}