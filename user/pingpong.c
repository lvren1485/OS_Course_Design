#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) 
{
    // 创建两个管道
    // parent_to_child[0] 用于读取，parent_to_child[1] 用于写入
    int parent_to_child[2];
    int child_to_parent[2];
    char buf[1];
    
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) { // 子进程
        close(parent_to_child[1]); // 关闭不需要的写端
        close(child_to_parent[0]); // 关闭不需要的读端

        // 从父进程读取一个字节
        if (read(parent_to_child[0], buf, 1) != 1) {
            fprintf(2, "child read error\n");
            exit(1);
        }
        
        printf("%d: received ping\n", getpid());
        
        // 向父进程写入一个字节
        if (write(child_to_parent[1], buf, 1) != 1) {
            fprintf(2, "child write error\n");
            exit(1);
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else { // 父进程
        close(parent_to_child[0]); // 关闭不需要的读端
        close(child_to_parent[1]); // 关闭不需要的写端

        // 向子进程写入一个字节
        buf[0] = 'x';
        if (write(parent_to_child[1], buf, 1) != 1) {
            fprintf(2, "parent write error\n");
            exit(1);
        }

        // 从子进程读取一个字节
        if (read(child_to_parent[0], buf, 1) != 1) {
            fprintf(2, "parent read error\n");
            exit(1);
        }
        
        printf("%d: received pong\n", getpid());
        
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        exit(0);
    }
}