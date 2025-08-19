#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 比较文件名是否匹配
char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;
    
    // 找到最后一个斜杠后的文件名
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    
    // 返回文件名部分
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
    return buf;
}

// 递归查找文件
void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    // 打开目录
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    // 获取文件状态
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    switch(st.type) {
    case T_FILE:
        // 如果是文件，检查是否匹配
        if(strcmp(fmtname(path), filename) == 0) {
            printf("%s\n", path);
        }
        break;
        
    case T_DIR:
        // 如果是目录，递归处理
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        
        // 读取目录项
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            
            // 跳过 "." 和 ".."
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
                
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            
            // 递归查找子目录
            find(buf, filename);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }
    
    find(argv[1], argv[2]);
    exit(0);
}