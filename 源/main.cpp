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
int judge(int map[ROW][COL],int row,int col);
int main()
{
    initgraph(350,350,EX_SHOWCONSOLE);
    loadResource();
    srand((unsigned)time(NULL));

    BeginBatchDraw();

    //一局结束后询问是否再来，点“否”就退出
    while(true)
    {
        int map[ROW][COL];
        init(map);

        draw(map);
        FlushBatchDraw();

        int state = 0;  //0=进行中，1=胜利，-1=失败
        //游戏主循环
        while(state==0)
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
                    //左键翻开格子后判断输赢
                    if(msg.message==WM_LBUTTONDOWN)
                    {
                        int c = msg.x / 35;
                        int r = msg.y / 35;
                        state = judge(map,r,c);
                        //已经分出胜负，不再处理这次积压的其它点击
                        if(state!=0)
                        {
                            break;
                        }
                    }
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

        //游戏结束：失败时把所有雷翻开，胜利时把所有雷插旗
        for(int i=0;i<ROW;i++)
        {
            for(int k=0;k<COL;k++)
            {
                if(map[i][k]==19 || map[i][k]==39)
                {
                    map[i][k] = (state==1) ? 39 : -1;
                }
            }
        }
        draw(map);
        FlushBatchDraw();

        //在窗口上显示输赢结果
        setfillcolor(WHITE);
        solidrectangle(0,150,350,200);
        setbkmode(TRANSPARENT);
        settextcolor(state==-1 ? RGB(200,0,0) : RGB(0,150,0));
        const char* text = (state==-1) ? "GAME OVER" : "YOU WIN";
        outtextxy((350-textwidth(text))/2, 160, text);
        FlushBatchDraw();

        cout << (state==-1 ? "GAME OVER!" : "YOU WIN!") << endl;

        //弹窗询问是否再来一局
        int ret;
        if(state==1)
        {
            ret = MessageBoxW(GetHWnd(), L"恭喜通关！要不要再试一把？", L"扫雷", MB_YESNO | MB_ICONQUESTION);
        }
        else
        {
            ret = MessageBoxW(GetHWnd(), L"踩到雷了！要不要再试一次？", L"扫雷", MB_YESNO | MB_ICONQUESTION);
        }
        if(ret != IDYES)
        {
            break;
        }
        system("cls");
    }

    closegraph();
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

//游戏结束条件
int judge(int map[ROW][COL],int row,int col)
{
    //越界保护
    if(row<0||row>=ROW||col<0||col>=COL)
    {
        return 0;
    }
    //点到了雷，游戏结束
    if(map[row][col]==-1||map[row][col]==19)
    {
        return -1;
    }
    //统计已翻开的数字格，全部翻开则胜利
    int cnt=0;
    for(int i=0;i<ROW;i++)
    {
        for(int k=0;k<COL;k++)
        {
            if(map[i][k]>=0 && map[i][k]<=8)
            {
                ++cnt;
            }
        }
    }
    if(ROW*COL-MineNum==cnt)
    {
        return 1;
    }
    return 0;
}

