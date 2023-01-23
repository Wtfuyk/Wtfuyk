#include "ramfs.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

// #include <stdbool.h>
typedef struct node {
    enum {
        FILE_NODE, DIR_NODE
    } type;
    struct node **dirents; // if it's root dir, there's subentries
    void *content; // if it's root file, there's data content
    int nrde; // number of subentries for dir
    int size; // size of file
    char *name; // it's short name
} Node;

Node *root;

typedef struct FD {
    int state; //状态 1 正在打开， 2已经关闭
    int offset;//偏移量
    int flags;//打开方式
    Node *f;
} FD;

FD v[10000000];
int nn = 0;

char tmp1[40], tmp2[40];//tmp1第一个文件夹， tmp2第二个文件(夹)

Node *search(char name[], Node *a) {
    Node *temp = NULL;
    int i = 0;
    if (a != NULL) {
        if (strcmp(name, a->name) == 0) // 如果名字匹配
        {
            temp = a;
        } else // 如果不匹配，则查找其子节点
        {
            for (i = 0; i < a->nrde && temp == NULL/*如果temp不为空，则结束查找*/; ++i) {
//                printf("i : %d\n", i); //todo
                temp = search(name, a->dirents[i]); // 递归查找子节点
            }
        }
    }

    return temp; // 将查找到的节点指针返回，也有可能没有找到，此时temp为NULL
}

void insert(Node *n1, Node *n2) {
    n2->type = DIR_NODE;
    n1->dirents[n1->nrde] = (Node *) malloc(sizeof(Node));
    strcpy(n1->dirents[n1->nrde]->name, tmp2);
    n1->dirents[n1->nrde] = n2;
    n1->nrde = n1->nrde + 1;
}

char s1[40], s2[40]; //s1倒数第二个文件夹， s2叶节点文件(夹)

int is_right(char *pathname) {
    int f = 0, l = 0, r = 0;
    int tot1 = 0, tot2 = 0;
    for (int i = 0; i < strlen(pathname); i++) { //标记f, l, r
        if (pathname[i] == '/') {
            if (f == 0) f = l = i;
            else if (pathname[i - 1] == '/') l = i;
            else if ((pathname[i + 1] == '/' && pathname[i] != '/') || i == strlen(pathname))r = i;
        }
    }
    for (int i = 0; i < f; ++i) {
        tmp1[tot1++] = *(pathname + i);
    }
    for (int i = l; i <= r; ++i) {
        tmp2[tot2++] = *(pathname + i);
    }
    Node *n1 = search(tmp1, root);
    Node *n2 = search(tmp2, root);
    if (n1->type == FILE_NODE) return 2;
    if (n2 == NULL) {
        if (strcmp(tmp2, s2) == 0) return 1;
        return 2;
    }
    char s[2000];
    strncpy(s, pathname + l, strlen(pathname) - l - 1);
    return is_right(s);
    // 0 找到
    // 1 父节点找到，子节点不存在
    // 2 父节点也不存在 | 不合法
}

int deal(char *pathname) {
    for (int i = 0; i < strlen(pathname); i++) {
        if ((pathname[i] <= 'z' && pathname[i] >= 'a') || (pathname[i] <= 'Z' && pathname[i] >= 'A') ||
            (pathname[i] <= '9' && pathname[i] >= '0') || pathname[i] == '/' || pathname[i] == '.')
            continue;
        else return 2;
    }
    if (*pathname == '/') {
        char *s = (char *) calloc(2000, sizeof(char));
        strcpy(s, "root");
        strcat(s, pathname);
        strcpy(pathname, s);
    }
    return is_right(pathname);
}

void make_node(char *pathname) {
    int f = 0, l = 0, r = 0, m = 0;
    int tot1 = 0, tot2 = 0;
    for (int i = strlen(pathname); i > 0; i--) { //标记f, l, r, m
        if (pathname[i] != '/') {
            m = i;
            for (int j = i; j > 0; j--) {
                if (pathname[j] == '/') {
                    if (f == 0) f = l = j;
                    else if (pathname[j - 1] == '/') l = j;
                    else if ((pathname[j] != '/' && pathname[j + 1] == '/') || j == strlen(pathname)) r = j;
                }
            }
            break;
        }
    }
    for (int i = m; i > f; --i) {
        tmp1[tot1++] = *(pathname + i);
    }
    for (int i = l; i >= r; --i) {
        tmp2[tot2++] = *(pathname + i);
    }
    Node *n1 = search(tmp1, root);
    Node *n2 = search(tmp2, root);
    n2->type = FILE_NODE;
    n1->dirents[n1->nrde] = (Node *) malloc(sizeof(Node));
    strcpy(n1->dirents[n1->nrde]->name, tmp2);
    n1->dirents[n1->nrde] = n2;
    n1->nrde = n1->nrde + 1;
    n2->content = (char *) malloc(100 * (sizeof(char)));
    n2->size = 100;
    //创建
}

int ropen(char *pathname, int flags) {
    FD *fd = &v[nn++];
    if (flags & O_CREAT) {
        if (deal(pathname) == 0) return -1;// if(is_right() == 0)
        if (deal(pathname) == 1) { make_node(pathname); } //赋值type
        if (deal(pathname) == 2) return -1;
    }
    Node *n = search(tmp1, root);
    if (n == NULL)return -1;
    fd->f = n;
    // fd->flags = flags; //
    if (flags & O_WRONLY) fd->flags = O_WRONLY;
    else if (flags & O_RDWR) fd->flags = O_RDWR;
    else fd->flags = O_RDONLY;
    fd->offset = 0;
    fd->state = 1;
    if (flags & O_TRUNC && flags & O_WRONLY) {
        memset(n->content, 0, sizeof(*(n->content)));
    }
    if (flags & O_APPEND) {
        fd->offset = strlen(n->content);
    }
    return nn - 1;
}

int rclose(int fd) {
    if (v[fd].state == 1) v[fd].state = 2;
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
    if (fd > nn - 1 || v[fd].state == 2) return -1;
    if ((v[fd].flags & O_WRONLY) || (v[fd].flags & O_RDWR)) {
        if (strlen(v[fd].f->content) - 1 + count > v[fd].f->size) {
            v[fd].f->content = (char *) realloc(v[fd].f->content, 2 * count);
            v[fd].f->size += 2 * count;
        }
        memcpy(v[fd].f->content + v[fd].offset, buf, count);
        v[fd].offset += count;
        return count;
    }
    return -1;
    // if (v[fd].flags == 01 || v[fd].flags == 02 || v[fd].flags == 01001 || v[fd].flags == 0101 || v[fd].flags == 02001) {
    // } else {
    //     return -1;
    // }
}

ssize_t rread(int fd, void *buf, size_t count) {
    if (fd > nn - 1 || v[fd].state == 2 || v[fd].flags == O_WRONLY) return -1;
    int n = 0;
    buf = malloc(100 * sizeof(char));
    if (v[fd].f->name == NULL || v[fd].f->type == DIR_NODE || v[fd].flags == O_WRONLY)return -1;
    else {
        n = strlen(v[fd].f->content) - 1 - v[fd].offset;
        if (n >= count) {
            memcpy(buf, v[fd].f->content + v[fd].offset, count - v[fd].offset);
            return count;
        } else {
            memcpy(buf, v[fd].f->content + v[fd].offset, n);
            return n;
        }
    }
}

off_t rseek(int fd, off_t offset, int whence) {
    if (whence == SEEK_SET)v[fd].offset = offset;
    if (whence == SEEK_CUR)v[fd].offset = v[fd].offset + offset;
    if (whence == SEEK_END) {
        v[fd].offset = strlen(v[fd].f->content) - 1 + offset;
        char buf[50000];  //todo
        for (int i = 0; i < offset; ++i) {
            *(buf + i) = '\0';
        }
        if (strlen(v[fd].f->content) - 1 + offset > v[fd].f->size) {
            v[fd].f->content = (char *) realloc(v[fd].f->content, 2 * offset);
            v[fd].f->size += 2 * offset;
        }
        memcpy(v[fd].f->content + v[fd].offset, buf, offset);
    }
}

int rmkdir(char *pathname) {
    if (deal(pathname) == 0 || deal(pathname) == 2) return -1;
    if (deal(pathname) == 1) {
        Node *n1 = search(tmp1, root);
        Node *n2 = search(tmp2, root);
        insert(n1, n2);
    }
}

int rrmdir(char *pathname) {
    if (deal(pathname) == 0) {
        Node *tmp3 = search(tmp2, root);
        if (tmp3->dirents == NULL) {
            for (int i = 0; i <= nn; ++i) {
                if (strcmp(v[i].f->name, tmp2) == 0)return -1;
            }
        }
        if (tmp3->dirents != NULL)return -1;
        else free(tmp3);
    } else return -1;
}

int runlink(char *pathname) {
    if (deal(pathname) == 0) {
        Node *tmp3 = search(tmp2, root);
        Node *tmp4 = search(tmp1, root);
        if (tmp3->dirents == NULL) {
            for (int i = 0; i <= nn; ++i) {
                if (strcmp(v[i].f->name, tmp2) == 0)return -1;
            }
        }
        if (tmp3->dirents != NULL)return -1;
        else {
            free(tmp3);
            tmp4->nrde -= 1;
        }
    } else return -1;
}

void init_ramfs() {
    root = (Node *) calloc(1, sizeof(Node));
    root->type = DIR_NODE;
    root->name = "/";
    root->dirents = 0;
    root->nrde = 0;
}
