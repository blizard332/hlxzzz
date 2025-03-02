#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<sstream>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#define MAX_SIZE 10

using namespace std;

struct OperationRecord {
    std::string operationType; 
    int value; 
};
std::vector<OperationRecord> operationRecords; // 用于存储操作记录的容器

struct Button
{
    int X;
    int Y;
    int hsize;			// 按钮的高度
    int lsize;			// 按钮的长度
    LPCTSTR str;		// 字符串
    COLORREF Strcolor;	// 字体颜色
    COLORREF Linecolor;	// 线的颜色
    COLORREF Bkcolor;	// 填充圆角矩形颜色		
};



// 循环队列结构
struct CircularQueue 
{
    int data[MAX_SIZE]; 
    int front;          
    int rear;          
    bool isDequeued[MAX_SIZE]; 
};

// 函数声明
void initQueue(CircularQueue* q);
bool isEmpty(const CircularQueue* q);
bool isFull(const CircularQueue* q);
bool enqueue(CircularQueue* q, int value);
bool dequeue(CircularQueue* q, int* value);
void drawQueue(const CircularQueue* q, int nextEnqueueValue, bool isPaused, int dequeuedValue);
void runAnimation(CircularQueue* queue);
void DrawTXT(int H);			
bool shouldExit = false;
void DrawButton(Button button);
int ButtonDown();		
Button button[2];

// 初始化队列
void initQueue(CircularQueue* q) {
    q->front = 0;
    q->rear = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
        q->isDequeued[i] = false;
    }
}

//判断是否队空
bool isEmpty(const CircularQueue* q) {
    return q->front == q->rear;
}

//判断是否队满
bool isFull(const CircularQueue* q) {
    return (q->rear + 1) % MAX_SIZE == q->front;
}

//入队
bool enqueue(CircularQueue* q, int value)
{
    if (isFull(q)) {
        int tempValue;
        dequeue(q, &tempValue); 
        printf("队列满，出队: %d\n", tempValue);

    }
    int index = q->rear;
    if (q->isDequeued[index]) {
        q->isDequeued[index] = false;
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % MAX_SIZE;

    OperationRecord record;
    record.operationType = "IN";
    record.value = value;
    operationRecords.push_back(record);
    return true;
}

//出队
bool dequeue(CircularQueue* q, int* value)
{
    if (isEmpty(q))
    {
        return false; 
    }
    *value = q->data[q->front];
    q->isDequeued[q->front] = true; 
    q->front = (q->front + 1) % MAX_SIZE; 

    OperationRecord record;
    record.operationType = "OUT";
    record.value = *value;
    operationRecords.push_back(record);
    return true; 
}


// 绘制队列状态
void drawQueue(const CircularQueue* q, int nextEnqueueValue, bool isPaused, int dequeuedValue) 
{
    setfillcolor(BLACK);
    solidrectangle(0, 0, getwidth(), getheight());
    TCHAR str[126];

    // 标题
    settextcolor(WHITE);
    settextstyle(30, 0, _T("Arial"));
    outtextxy(200, 20, _T("循环队列:顺序存储结构"));   
    settextcolor(WHITE);

    settextcolor(WHITE);
    settextstyle(30, 0, _T("Arial"));
    outtextxy(50, 70, _T("data[  ]"));
    int size = (q->rear - q->front + MAX_SIZE) % MAX_SIZE;
    int T = q->front; 


    // 标注队列槽和数据
    for (int i = 0; i < MAX_SIZE; i++)
    {
        setlinecolor(WHITE);
        fillrectangle(i * 50 + 50, 120, i * 50 + 100, 170);

        // 标示每个槽位的索引
        settextcolor(WHITE);
        settextstyle(22, 0, _T("Arial"));
        _stprintf_s(str, _T("%d"), i % MAX_SIZE);
        outtextxy(i * 50 + 55, 175, str); // 显示索引

        if (q->isDequeued[i])
        {
            settextcolor(CYAN); 
        }
        else
        {
            settextcolor(WHITE);
        }
        settextstyle(30, 0, _T("Arial"));
        if (q->data[i] < 0)
        {
            outtextxy(i * 50 + 60, 135, _T(""));
        }
        else
        {
            _stprintf_s(str, _T("%d"), q->data[(i) % MAX_SIZE]);
            outtextxy(i * 50 + 60, 135, str);
        }
    }

    // 绘制队头指针（Front）
    int frontPosX = ((q->front) * 50) + 75;
    setlinecolor(RED);
    line(frontPosX, 95, frontPosX, 105);
    settextcolor(WHITE);
    outtextxy(frontPosX - 30, 210, _T("Front"));
    
    // 绘制队尾指针（Rear）
    int rearPosX = ((q->rear + MAX_SIZE) % MAX_SIZE * 50) + 75;
    setlinecolor(GREEN);
    line(rearPosX, 95, rearPosX, 105);
    outtextxy(rearPosX - 30, 240, _T("Rear"));
    settextcolor(WHITE);

    // 绘制指向数组的箭头
    line(frontPosX, 170, frontPosX, 185); // Front指向
    line(rearPosX, 170, rearPosX, 185); // Rear指向

    if (isFull(q)) {
        settextcolor(RED);
        settextstyle(40, 0, _T("Arial"));
        outtextxy(570,110, _T("队已满"));
    }

    //操作记录区域
    setfillcolor(RGB(30, 30, 30));
    solidrectangle(50, 300, 600, 700);

    settextcolor(WHITE);
    settextstyle(20, 0, _T("Arial"));

    outtextxy(60, 310, _T("操作记录"));
    setbkmode(TRANSPARENT);

    const int maxCharsPerLine = 30;
    const int columnSpacing = 100;
    int currentColumnX = 60;
    int currentRowY = 330;

    std::ifstream file("data.txt");  // 打开操作记录文件，根据实际情况修改文件名
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string operationType;
            int value;
            ss >> operationType >> value;
            TCHAR str[126];
            _stprintf_s(str, _T("%s %d"), operationType.c_str(), value);
            outtextxy(currentColumnX, currentRowY, str);
            currentRowY += 20;
            if (currentRowY > 680)  // 判断是否超出操作记录区域显示范围，可根据实际调整
            {
                currentColumnX += columnSpacing;
                currentRowY = 330;
            }
        }
        file.close();
    }
    else
    {
        outtextxy(60, 330, _T("无法打开操作记录文件"));
    }

}


void runAnimation(CircularQueue* queue) 
{
    int pause = 0;
    int value;
    int dequeuedValue = -1;
    drawQueue(queue, 0, pause, dequeuedValue);
    
    // 绘制两个按钮,进行选择入队还是出队
    int NUMS[20] = { 10,32,54,66,11,67,87,43,83,28,76,30,14}; 
    DrawTXT(0);
    int MYS = 0;
    while (true)
    {

        int nums = ButtonDown();
        if (nums == 1)
        {

            if (isFull(queue)) {
                DrawTXT(1);
                Sleep(200);
                DrawTXT(4);
                Sleep(500);
                continue;
            }
                DrawTXT(1);
                Sleep(250);
            setbkmode(OPAQUE);
            enqueue(queue, NUMS[MYS]);
            drawQueue(queue, NUMS[MYS], pause, dequeuedValue);
            Sleep(100);
            MYS++;
            if (MYS >= sizeof(NUMS) / sizeof(NUMS[0]))
            {
                MYS = 0;
            }
            DrawTXT(6);
            Sleep(300);
            DrawTXT(7);
            Sleep(200);
        }
       else if (nums==0)
        {
            if (isEmpty(queue))
            {
                DrawTXT(11);
                Sleep(200);
                DrawTXT(12);
                Sleep(200);
                continue;
            }
                DrawTXT(10);
                Sleep(200);
                DrawTXT(11);
                Sleep(200);
            setbkmode(OPAQUE);
            dequeue(queue, &dequeuedValue);
            drawQueue(queue, 0, pause, dequeuedValue);
            DrawTXT(15);
            Sleep(200);
            DrawTXT(16);
            Sleep(200);
            DrawTXT(17);
            Sleep(200);
        }
        }
    }

// 主函数
int main()
{
    CircularQueue queue;
    initQueue(&queue);
    initgraph(1200, 1200); // 初始化图形窗口
    DrawTXT(0);
    runAnimation(&queue); // 开始动画
    closegraph(); // 关闭图形窗口
    return 0;
}

void DrawButton(Button button)
{
    settextcolor(button.Strcolor);
    settextstyle(button.hsize - 5, 0, _T("楷体"));
    setlinecolor(button.Linecolor);
    setlinestyle(PS_SOLID, 1);
    setfillcolor(button.Bkcolor);
    setbkmode(TRANSPARENT);
    fillroundrect(button.X, button.Y, button.X + button.lsize, button.Y + button.hsize, 5, 5);
    RECT r = { button.X,button.Y,button.X + button.lsize,button.Y + button.hsize };
    drawtext(button.str, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

int ButtonDown()
{
    button[0] = { 700,550,40, 480 ,_T("入队"),WHITE,BLUE,RED };
    button[1] = { 700,600,40,480,_T("出队"),WHITE,BLUE,RED };
    int buttonNUM = 2;
    BeginBatchDraw();
    ExMessage msg;
    int nums = 3;
    while (true)
    {
        while (peekmessage(&msg, EX_MOUSE))		
        {
            switch (msg.message)					
            {
            case WM_MOUSEMOVE:
                for (int i = 0; i < buttonNUM; i++)
                {
                    if (msg.x > button[i].X && msg.x < button[i].X + button[i].lsize && msg.y > button[i].Y && msg.y < button[i].Y + button[i].hsize)
                    {
                        button[i].Linecolor = YELLOW;
                    }
                    else
                    {
                        button[i].Linecolor = BLUE;
                    }
                }
                ; break;
            case WM_LBUTTONDOWN:
                for (int i = 0; i < buttonNUM; i++)
                {
                    if (msg.x > button[i].X && msg.x < button[i].X + button[i].lsize && msg.y > button[i].Y && msg.y < button[i].Y + button[i].hsize)
                    {

                        button[i].Strcolor = YELLOW;
                        button[i].Bkcolor = GREEN;
                    }

                }
                break;
            case WM_LBUTTONUP:

                for (int i = 0; i < buttonNUM; i++)
                {
                    if (msg.x > button[i].X && msg.x < button[i].X + button[i].lsize && msg.y > button[i].Y && msg.y < button[i].Y + button[i].hsize)
                    {
                        nums = i;
                        button[i].Strcolor = WHITE;
                        button[i].Bkcolor = RED;
                    }

                }
                break;
            }
        }
        for (int i = 0; i < buttonNUM; i++)
        {
            DrawButton(button[i]);
        }
        FlushBatchDraw();
        if (nums != 3)
        {
            nums = nums + 1;
            nums %= buttonNUM;
            break;
        }
    }
    EndBatchDraw();
    return nums;
}

void DrawTXT(int H)
{
    setfillcolor(RGB(30, 30, 30));
    solidrectangle(700, 50, 1400, 490);
    settextcolor(RGB(181, 198, 130));
    int stary = 100;
    if (H != 0)
    {
        setfillcolor(RGB(38, 79, 125));
        solidrectangle(700, 100 + H * 20, 1140, 100 + H * 20 + 15);
    }
    int starx = 710;
    settextstyle(20,0, _T("宋体"));
    setbkmode(TRANSPARENT);

    std::ifstream file("code display.txt");  
    if (file.is_open())
    {
        std::string line;
        int stary = 100;
        while (std::getline(file, line))
        {
            outtextxy(710, stary, _T(line.c_str())); 
            stary += 20;
        }
        file.close();
    }
    else
    {
        outtextxy(710, 100, _T("无法打开算法代码文件")); 
    }
}
