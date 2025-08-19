#include "kernel/types.h"
#include "user/user.h"

// 筛选素数的函数
void sieve(int in_fd) {
    int prime;
    int num;
    
    // 读取第一个数字，它一定是素数
    if (read(in_fd, &prime, sizeof(prime))) {
        printf("prime %d\n", prime);
        
        // 创建管道用于与下一个进程通信
        int p[2];
        pipe(p);
        
        if (fork() == 0) {
            // 子进程：关闭不需要的文件描述符
            close(p[1]);  // 关闭写端
            sieve(p[0]);  // 递归处理
            close(p[0]);
            exit(0);
        } else {
            // 父进程：关闭不需要的文件描述符
            close(p[0]);  // 关闭读端
            
            // 读取剩余数字，过滤掉能被prime整除的
            while (read(in_fd, &num, sizeof(num))) {
                if (num % prime != 0) {
                    write(p[1], &num, sizeof(num));
                }
            }
            
            close(p[1]);  // 关闭写端，通知子进程结束
            wait(0);      // 等待子进程结束
        }
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);
    
    if (fork() == 0) {
        // 子进程：开始筛选
        close(p[1]);  // 关闭写端
        sieve(p[0]);  // 开始筛选过程
        close(p[0]);
        exit(0);
    } else {
        // 父进程：生成数字2-35
        close(p[0]);  // 关闭读端
        
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }
        
        close(p[1]);  // 关闭写端，通知子进程结束
        wait(0);      // 等待子进程结束
    }
    
    exit(0);
}