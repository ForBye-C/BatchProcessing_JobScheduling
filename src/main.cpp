//通过批处理作业调度算法来模拟作业调度程序在计算机系统中执行的进程
//该程序将通过不同作业调度和进程调度算法分析不同调度策略的性能，并以作业平均周转时间和带权周转时间来反应
//作业调度算法包含：FCFS（先来先服务）、SJF（短作业优先）、HRN（最高响应比优先）、HPF（最高优先级优先）
//进程调度算法包含：FCFS（先来先服务）、SPF（短进程优先）、SRT（最短剩余时间优先）、HPF（最高优先级优先）

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <windows.h>

using namespace std;

constexpr int OVER = -1;    //表示作业已完成的状态
constexpr int NOT_ENTER = 0;    //表示作业未进入的状态
constexpr int INPUT_WELL = 1;    //表示作业在输入井中等待的状态
constexpr int IN_STOR = 2;    //表示作业在内存中等待的状态
constexpr int CPU = 3;    //表示作业正在被CPU执行的状态

struct Job {
    int enterTime{};    //作业进入输入井等待被调度的时间，将时间全部换算成从0点开始一共多少分钟
    int estimatedRunningTime{};    //作业的估计运行时间
    int priority{};    //作业的优先数
    int state{};    //作业当前的状态（NOT_ENTER、INPUT_WELL、IN_STOR、CPU、OVER共5种）
    int enInsTime{};    //作业进入内存中的时间
    int startTime = -1;    //作业被调度到CPU的开始时间
    int runTime{};    //作业已经被CPU执行的时间
    int endTime{};    //作业的结束时间
    int TurnaroundTime{};    //作业的周转时间
    double TurnaroundTime_withPower{};    //作业的带权周转时间
};

static int FCFS(Job jobLine[], const int jobNum, const string& S) {    //先来先服务作业/进程调度
    int earliest(1440);    //以理论最大进入时间初始化
    int ear_loc{};    //标定最先来的作业的位置
    if (S == "JOB") {    //若为作业调度
        for (int i = 0; i < jobNum; i++) {    //遍历寻找最早进入输入井的作业
            if (jobLine[i].state == INPUT_WELL && jobLine[i].enterTime < earliest) {
                earliest = jobLine[i].enterTime;
                ear_loc = i;
            }
        }
    }
    else if (S == "PROCESS") {    //若为进程调度
        for (int j = 0; j < jobNum; j++) {    //遍历寻找最早进入内存的作业
            if (jobLine[j].state >= IN_STOR && jobLine[j].enInsTime < earliest) {
                earliest = jobLine[j].enInsTime;
                ear_loc = j;
            }
        }
    }
    else exit (-1);    //错误指令报错并退出
    return ear_loc;    //返回最早作业的位置
}

static int SJF(Job jobLine[], const int jobNum, const string& S) {    //短作业优先作业调度
    if (S != "JOB") exit (-1);    //非法指令报错并退出
    int shortest(9999);    //以理论最大最短时间初始化
    int short_loc{};    //标定最短运行时间的作业的位置
    for (int i = 0; i < jobNum; i++) {    //遍历寻找最短运行时间的作业
        if (jobLine[i].state == INPUT_WELL && jobLine[i].estimatedRunningTime < shortest) {
            shortest = jobLine[i].estimatedRunningTime;
            short_loc = i;
        }
    }
    return short_loc;    //返回最短运行时间的作业的位置
}

static int HRN(Job jobLine[], const int jobNum, const string& S, int curTime) {    //最高响应比优先作业调度
    if (S != "JOB") exit (-1);    //非法指令报错并退出
    double answer(0);    //以最小响应比初始化
    int ans_loc{};    //标定最高响应比的作业的位置
    for (int i = 0; i < jobNum; i++) {    //遍历寻找最高响应比的作业
        double R = 1 + (curTime - jobLine[i].enterTime)*1.0 / jobLine[i].estimatedRunningTime;    //求出当前作业的响应比
        if (jobLine[i].state == INPUT_WELL && R > answer) {
            answer = R;
            ans_loc = i;
        }
    }
    return ans_loc;    //返回最短运行时间的作业的位置
}

static int HPF(Job jobLine[], const int jobNum, const string& S) {    //最高优先级优先作业/进程调度
    int highest(9999);    //以理论最小最高优先级初始化
    int high_loc{};    //标定最高优先级作业的位置
    if (S == "JOB") {    //若为作业调度
        for (int i = 0; i < jobNum; i++) {    //遍历寻找输入井中最高优先级的作业
            if (jobLine[i].state == INPUT_WELL && jobLine[i].priority < highest) {
                highest = jobLine[i].priority;
                high_loc = i;
            }
        }
    }
    else if (S == "PROCESS") {    //若为进程调度
        for (int j = 0; j < jobNum; j++) {    //遍历寻找内存中最高优先级的作业
            if (jobLine[j].state >= IN_STOR && jobLine[j].priority < highest) {
                highest = jobLine[j].priority;
                high_loc = j;
            }
        }
    }
    else exit (-1);    //非法指令报错并退出
    return high_loc;    //返回最高优先级的作业的位置
}

static int SPF(Job jobLine[], const int jobNum, const string& S) {    //短进程优先进程调度
    if (S != "PROCESS") exit (-1);    //非法指令报错并退出
    int shortest(9999);    //以理论最大最短时间初始化
    int short_loc{};    //标定最短运行时间的作业的位置
    for (int i = 0; i < jobNum; i++) {    //遍历寻找最短运行时间的作业进程
        if (jobLine[i].state >= IN_STOR && jobLine[i].estimatedRunningTime < shortest) {
            shortest = jobLine[i].estimatedRunningTime;
            short_loc = i;
        }
    }
    return short_loc;    //返回最短运行时间的作业进程的位置
}

static int SRT(Job jobLine[], const int jobNum, const string& S) {    //最短剩余时间优先进程调度
    if (S != "PROCESS") exit (-1);    //非法指令报错并退出
    int leftTime(9999);    //以理论最长最短剩余时间初始化
    int left_loc{};    //标定最短剩余时间的作业的位置
    for (int i = 0; i < jobNum; i++) {    //遍历寻找最短剩余时间的作业进程
        if (jobLine[i].state >= IN_STOR && jobLine[i].estimatedRunningTime - jobLine[i].runTime < leftTime) {
            leftTime = jobLine[i].estimatedRunningTime - jobLine[i].runTime;
            left_loc = i;
        }
    }
    return left_loc;    //返回最短剩余时间的作业进程的位置
}

static void Dispatch(const int jobNum, Job jobLine[], const int ISC, const string& JS, const string& PS) {
    int endFlag = 0;    //作业全部结束的标志
    int beginTime = 1440;    
    for (int i = 0; i < jobNum; i++) {    //整个作业调度的开始时间即为最早作业的进入时间
        if (jobLine[i].enterTime < beginTime) beginTime = jobLine[i].enterTime;
    }

    int currentTime = beginTime;    //用currentTime的增加来表示时间的进行
    while (endFlag == 0) {    //在所有作业完成之前时间随着循环进行下去
        for (int i = 0; i < jobNum; i++) {    //将达到进入时间的作业送入输入井
            if (jobLine[i].enterTime == currentTime) {
                jobLine[i].state = INPUT_WELL;
            }
        }

        int ONum = 0, INum = 0, CNum = 0;    //每个时间步根据 state 重算计数
        for (int i = 0; i < jobNum; i++) {
            if (jobLine[i].state == INPUT_WELL) ONum++;
            else if (jobLine[i].state == IN_STOR) INum++;
            else if (jobLine[i].state == CPU) CNum++;
        }

        while (ONum > 0 && INum + CNum < ISC) {  //当内存作业进程未满并且输入井中有等待的作业时
            int loc;    //标定对应作业调度算法选择的作业的位置

            if (JS == "FCFS") loc = FCFS(jobLine, jobNum, "JOB");    //识别作业调度算法并定位目标作业
            else if (JS == "SJF") loc = SJF(jobLine, jobNum, "JOB");
            else if (JS == "HRN") loc = HRN(jobLine, jobNum, "JOB", currentTime);
            else if (JS == "HPF") loc = HPF(jobLine, jobNum, "JOB");
            else {    //非法输入报错并退出
                cout << "Error job scheduling!" << endl;
                exit(-1);
            }
            jobLine[loc].state = IN_STOR;    //变更目标作业的实时状态，从输入井转入内存
            jobLine[loc].enInsTime = currentTime;    //记录作业进入内存的时间

            ONum--;
            INum++;
        }

        if (INum + CNum > 0) {    //当内存中有作业进程时
            int loc;    //标定对应进程调度算法选择的作业的位置

            if (PS == "FCFS") loc = FCFS(jobLine, jobNum, "PROCESS");    //识别进程调度算法并定位目标作业
            else if (PS == "SPF") loc = SPF(jobLine, jobNum, "PROCESS");
            else if (PS == "SRT") loc = SRT(jobLine, jobNum, "PROCESS");
            else if (PS == "HPF") loc = HPF(jobLine, jobNum, "PROCESS");
            else {    //非法输入报错并退出
                cout << "Error process scheduling!" << endl;
                exit(-1);
            }

            if (jobLine[loc].state == IN_STOR) {    //若目标作业现态为在内存中等待状态
                for (int j = 0; j < jobNum; j++) {    //接触（若有）正在被CPU执行作业对CPU的占用，并调至内存中等待
                    if (jobLine[j].state == CPU) jobLine[j].state = IN_STOR;
                }
                jobLine[loc].state = CPU;    //目标作业占用CPU
                if (jobLine[loc].startTime == -1) jobLine[loc].startTime = currentTime;    //若作业首次占用CPU，记录其开始时间
            }

            jobLine[loc].runTime++;    //每个时间片都计入实际运行时间

            if (jobLine[loc].runTime == jobLine[loc].estimatedRunningTime) {    //若作业已将运行完成
                jobLine[loc].state = OVER;    //更改作业状态
                jobLine[loc].endTime = currentTime + 1;    //作业完成于本时间片结束时，记录作业结束时间
                jobLine[loc].TurnaroundTime = jobLine[loc].endTime - jobLine[loc].enterTime;    //计算周转时间及带权周转时间
                jobLine[loc].TurnaroundTime_withPower = 1.0*jobLine[loc].TurnaroundTime / jobLine[loc].runTime;
            }
        }

        int finish = 0;    //记录已结束的作业进程数
        for (int i = 0; i < jobNum; i++) {    //统计作业结束情况
            if (jobLine[i].state == OVER) finish++;
        }
        if (finish == jobNum) endFlag = 1;    //判断作业是否全部结束并更改全进程结束标识

        currentTime++;    //模拟时间进行
    }
}

static void PerformAnalysis(const int jobNum, Job jobLine[], double &TT_aver, double &TTP_aver) {    //计算作业的平均周转时间和平均带权周转时间
    int TT_sum = 0;
    double TTP_sum = 0;

    for (int i = 0; i < jobNum; i++) {
        TT_sum += jobLine[i].TurnaroundTime;
        TTP_sum += jobLine[i].TurnaroundTime_withPower;
    }
    TT_aver = TT_sum*1.0 / jobNum;
    TTP_aver = TTP_sum / jobNum;
}

static string timeStr(int minutes) {    //将总分钟格式化为 H:MM 字符串，分钟补零
    string s = to_string(minutes / 60) + ":";
    int m = minutes % 60;
    if (m < 10) s += '0';
    return s + to_string(m);
}

static string doubleStr(double v) {    //按默认精度将浮点数转为字符串，与直接输出保持一致
    ostringstream oss;
    oss << v;
    return oss.str();
}

static int displayWidth(const string& s) {    //计算字符串在控制台中的显示宽度，中文及全角字符按 2 列计
    int width = 0;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = (unsigned char)s[i];
        int len = 1;
        unsigned int cp = c;
        if ((c & 0x80) == 0) { len = 1; cp = c; }
        else if ((c & 0xE0) == 0xC0) { len = 2; cp = c & 0x1Fu; }
        else if ((c & 0xF0) == 0xE0) { len = 3; cp = c & 0x0Fu; }
        else if ((c & 0xF8) == 0xF0) { len = 4; cp = c & 0x07u; }
        else { i++; width++; continue; }
        for (int k = 1; k < len; k++)
            cp = (cp << 6) | ((unsigned char)s[i + k] & 0x3Fu);
        bool wide =    //East Asian 全角/宽字符区间
            (cp >= 0x1100 && (cp <= 0x115F || cp == 0x2329 || cp == 0x232A ||
             (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) ||
             (cp >= 0xAC00 && cp <= 0xD7A3) ||
             (cp >= 0xF900 && cp <= 0xFAFF) ||
             (cp >= 0xFE10 && cp <= 0xFE19) ||
             (cp >= 0xFE30 && cp <= 0xFE6F) ||
             (cp >= 0xFF00 && cp <= 0xFF60) ||
             (cp >= 0xFFE0 && cp <= 0xFFE6)));
        width += wide ? 2 : 1;
        i += len;
    }
    return width;
}

static void printCentered(const string& s, int width) {    //将字符串在指定宽度内居中对齐输出
    int w = displayWidth(s);
    int left = (width - w) / 2;
    cout << string(left, ' ') << s << string(width - w - left, ' ');
}

static void Print(const int jobNum, Job jobLine[], double TT_aver, double TTP_aver) {
    const string headers[] = {"作业", "到达时间", "估计运行时间（min）", "开始时间", "结束时间", "周转时间（min）", "带权周转时间"};
    constexpr int cols = 7;

    vector<vector<string>> rows(jobNum);    //将所有待打印数据统一转为字符串
    for (int i = 0; i < jobNum; i++) {
        rows[i] = {
            "JOB" + to_string(i + 1),
            timeStr(jobLine[i].enterTime),
            to_string(jobLine[i].estimatedRunningTime),
            timeStr(jobLine[i].startTime),
            timeStr(jobLine[i].endTime),
            to_string(jobLine[i].TurnaroundTime),
            doubleStr(jobLine[i].TurnaroundTime_withPower)
        };
    }

    int colW[cols]{};    //每列取表头与所有数据单元格的最大显示宽度
    for (int c = 0; c < cols; c++) {
        colW[c] = displayWidth(headers[c]);
        for (int i = 0; i < jobNum; i++)
            colW[c] = max(colW[c], displayWidth(rows[i][c]));
    }

    int totalWidth = 1;    //整张表的显示宽度：起始 '|' + 每列(左右各 1 空格 + 分隔 '|')
    for (int c = 0; c < cols; c++) totalWidth += colW[c] + 3;

    auto border = [&](char corner) {    //打印边框分隔线
        cout << corner;
        for (int c = 0; c < cols; c++)
            cout << string(colW[c] + 2, '-') << corner;
        cout << endl;
    };

    auto printRow = [&](const vector<string>& cells) {    //打印一行居中对齐的单元格
        cout << '|';
        for (int c = 0; c < cols; c++) {
            cout << ' ';
            printCentered(cells[c], colW[c]);
            cout << " |";
        }
        cout << endl;
    };

    auto printSpanRow = [&](const string& text) {    //打印跨整行的说明文字（左对齐，右侧补空格闭合边框）
        int inner = totalWidth - 2;
        cout << "| " << text << string(inner - 1 - displayWidth(text), ' ') << "|" << endl;
    };

    border('+');
    printRow(vector<string>(headers, headers + cols));
    border('+');
    for (int i = 0; i < jobNum; i++)
        printRow(rows[i]);
    border('+');
    printSpanRow("平均周转时间: " + doubleStr(TT_aver));
    printSpanRow("平均带权周转时间: " + doubleStr(TTP_aver));
    border('+');
}


int main() {
    SetConsoleOutputCP(CP_UTF8);
    int InStorage_maxCourse;    //内存中最多可等待调用处理的作业进程数
    int &ISC = InStorage_maxCourse;    //用ISMS简化InStorage_maxCourse命名
    string jobScheduling, processScheduling;    //分别表示作业调度算法和进程调度算法
    string &JS = jobScheduling, &PS = processScheduling;    //分别用JS和PS简化jobScheduling和processScheduling命名
    int jobNum;    //总共需要处理的作业数
    int h, m;    //分别接收输入时间的小时数和分钟数
    char colon;    //用来覆盖时间表示中的冒号

    cout << "设置内存中最多等待处理的进程数: ";
    cin >> ISC;
    cout << "设置系统作业调度算法: ";
    cin >> JS;
    cout << "设置系统进程调度算法: ";
    cin >> PS;
    cout << "输入待处理的作业数: ";
    cin >> jobNum;
    const auto jobLine = new Job[jobNum];    //连续存储待处理的作业便于管理操作

    for (int i = 0; i < jobNum; i++) {    //存储所有待处理的作业
        cout << "JOB" << i + 1 << ":" << endl;
        cout << "到达时间:";    //接收作业到达时间
        cin >> h >> colon >> m;    //colon用来吃掉输入时间的冒号
        jobLine[i].enterTime = h * 60 + m;    //将时间转化成总分钟便于存储和计算
        cout << "估计运行时间:";    //接收作业的估计运行时间
        cin >> jobLine[i].estimatedRunningTime;
        if (JS == "HPF" || PS == "HPF") {    //若采用最高优先级算法则还需记录作业的优先级
            cout << "优先级（数字越小优先级越高）:";
            cin >> jobLine[i].priority;
        }
    }

    Dispatch(jobNum, jobLine, ISC, JS, PS);    //进行批处理作业调度

    double TurnaroundTime_average{}, TurnaroundTime_withPower_average{};
    double& TT_aver = TurnaroundTime_average;
    double& TTP_aver = TurnaroundTime_withPower_average;

    PerformAnalysis(jobNum, jobLine, TT_aver, TTP_aver);    //对作业进行性能分析

    Print(jobNum, jobLine, TT_aver, TTP_aver);    //打印最终数据的统计表格

    delete[] jobLine;    //释放内存空间
    return 0;
}