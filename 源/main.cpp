#include<iostream>
#include<cstdlib>
#include<ctime>
#include<iomanip>
#include<easyx.h>

#define ROW 10
#define COL 10
#define MineNum 10
using namespace std;

void init(int map[ROW][COL]);
void show(int map[ROW][COL]);
int main()
{
    initgraph(350,350);
    int map[ROW][COL];
    init(map);
    show(map);
    getchar();
    return 0;
}

void show(int map[ROW][COL])
{
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COL;j++)
        {
            cout<<setw(3)<<map[i][j]<<" ";
        }
        cout<<endl;
    }
    return;
}

void init(int map[ROW][COL])
{
    srand((unsigned)time(NULL));
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COL;j++)
        {
            map[i][j]=0;
        }
    }
    for(int i=0;i<MineNum;i++)
    {
        int x=rand()%ROW;
        int y=rand()%COL;
        if(map[x][y]==0)
        {
            map[x][y]=-1;
        }
        else
        {
            i--;
        }
    }
    
    for(int i=0;i<ROW;i++)
    {
        for(int k=0;k<COL;k++)
        {
            if(map[i][k]==-1)
            {
                for(int r=i-1;r<=i+1;r++)
                {
                    for(int c=k-1;c<=k+1;c++)
                    {
                        if((r>=0&&r<ROW&&c>=0&&c<COL)&&map[r][c]!=-1)
                        {
                            ++map[r][c];
                        }
                    }
                }
            }
        }
    }
}


