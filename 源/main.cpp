#include<iostream>
#include<cstdlib>
#include<ctime>
#include<iomanip>
#include<string>
#include<easyx.h>

#define ROW 10
#define COL 10
#define MineNum 10
IMAGE imgs[12];
using namespace std;

void init(int map[ROW][COL]);
void show(int map[ROW][COL]);
void draw(int map[ROW][COL]);
void loadResource();
void mouseMsg(ExMessage* msg,int map[][COL]);
void boomBlank(int map[ROW][COL],int row,int col);
int main()
{
    initgraph(350,350,EX_SHOWCONSOLE);
    loadResource();
    int map[ROW][COL];
    init(map);

    BeginBatchDraw();
    draw(map);
    FlushBatchDraw();

    //游戏主循环
    while(true)
    {
        ExMessage msg;
        bool changed = false;
        //处理消息
        while(peekmessage(&msg,EX_MOUSE))
        {
            //只处理鼠标按下事件
            if(msg.message==WM_LBUTTONDOWN || msg.message==WM_RBUTTONDOWN)
            {
                mouseMsg(&msg,map);
                changed = true;
            }
        }
        //有操作才重绘，避免空转占满 CPU
        if(changed)
        {
            draw(map);
            FlushBatchDraw();
        }
        Sleep(10);
    }
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
    //加密格子
    for(int i=0;i<ROW;i++)
    {
        for(int k=0;k<COL;k++)
        {
            map[i][k]+=20;
        }
    }
}

void loadResource()
{
    for(int i=0;i<12;i++)
    {
        std::string path = "..\\images\\" + std::to_string(i) + ".png";
        if(loadimage(&imgs[i], path.c_str()) != 0)
        {
            std::cout << "load failed: " << path << std::endl;
        }
    }
}

void draw(int map[ROW][COL])
{
    for(int i=0;i<ROW;i++)
    {   
        for(int k=0;k<COL;k++)
        {
            if(map[i][k]>=0 &&map[i][k]<=8)
            {
                putimage(k*35,i*35,&imgs[map[i][k]]);
            }
            else if(map[i][k]==-1)
            {
                putimage(k*35,i*35,&imgs[9]);
            }
            else if(map[i][k]>=19&&map[i][k]<=28)
            {
                putimage(k*35,i*35,&imgs[10]);
            }
            else if(map[i][k]>=39)
            {
                putimage(k*35,i*35,&imgs[11]);
            }
        }
    }
}

void mouseMsg(ExMessage* msg,int map[ROW][COL])
{
    //先根据鼠标点击的坐标求出对应数组的下标
    int c = msg->x / 35;
    int r = msg->y / 35;
    //越界保护
    if(r<0||r>=ROW||c<0||c>=COL) return;
    //左键打开格子
    if(msg->message == WM_LBUTTONDOWN)
    {
        if(map[r][c]>=19 && map[r][c]<=28)
        {
            map[r][c]-=20;
            //如果是空白格，连环翻开周围的格子
            if(map[r][c]==0)
            {
                boomBlank(map,r,c);
            }
            system("cls");
            show(map);
        }
    }
    //右键标记雷区
    else if(msg->message == WM_RBUTTONDOWN)
    {
        //是否能标记
        if(map[r][c]>=19 && map[r][c]<=28)
        {
            map[r][c]+=20;   //插旗：19~28 -> 39~48
            system("cls");
            show(map);
        }
        else if(map[r][c]>=39)
        {
            map[r][c]-=20;   //取消插旗：39~48 -> 19~28
            system("cls");
            show(map);
        }
    }
}

//点击空白格子，连环翻开周围所有的空白格子
void boomBlank(int map[ROW][COL],int row,int col)
{
    //只处理空白格：数字格不再向外扩展
    if(map[row][col]!=0)
    {
        return;
    }
    //遍历周围 8 个格子
    for(int r=row-1;r<=row+1;r++)
    {
        for(int c=col-1;c<=col+1;c++)
        {
            //越界跳过
            if(r<0||r>=ROW||c<0||c>=COL)
            {
                continue;
            }
            //只翻未翻开的数字格：跳过雷(19)和已插旗(39+)
            if(map[r][c]>=20 && map[r][c]<=28)
            {
                map[r][c]-=20;
                boomBlank(map,r,c);
            }
        }
    }
}




