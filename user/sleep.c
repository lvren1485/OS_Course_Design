#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) 
{
    // 检查参数数量
    if(argc != 2) {
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    // 将字符串参数转换为整数
    int ticks = atoi(argv[1]);

    if (ticks < 0) {
        fprintf(2, "Error: ticks must be a positive integer\n");
        exit(1);
    }
    
    // 调用系统调用 sleep
    sleep(ticks);
    
    exit(0);
}