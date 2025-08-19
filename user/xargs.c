#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE 512

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    char buf[MAX_LINE];
    char *xargv[MAXARG];
    int i, n;
    char ch;
    
    // 设置初始命令参数
    for (i = 1; i < argc; i++) {
        xargv[i-1] = argv[i];
    }
    xargv[argc-1] = 0;  // exec 需要以0结尾的参数数组

    while (1) {
        i = 0;
        // 读取一行输入
        while ((n = read(0, &ch, 1)) > 0 && ch != '\n') {
            if (i < MAX_LINE - 1) {
                buf[i++] = ch;
            }
        }
        
        if (n <= 0 && i == 0)  // 没有更多输入
            break;
            
        buf[i] = 0;  // 终止字符串
        
        // 添加输入行作为最后一个参数
        xargv[argc-1] = buf;
        
        // 创建子进程执行命令
        if (fork() == 0) {
            exec(xargv[0], xargv);
            fprintf(2, "exec %s failed\n", xargv[0]);
            exit(1);
        } else {
            wait(0);  // 等待子进程完成
        }
    }
    
    exit(0);
}