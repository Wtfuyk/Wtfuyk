#include "ramfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>//查找？fd整形？

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
    int offset;//偏移量
    int flags;//打开方式
    Node *f;
} FD;

//fd与FD关系

FD v[10000000];
int nn = 0;

char tmp1[40];
char tmp2[40];

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
                temp = search(name, a->dirents[i]); // 递归查找子节点
            }
        }
    }

    return temp; // 将查找到的节点指针返回，也有可能没有找到，此时temp为NULL
}//实现

int is_right(const char *pathname) {
    int Size = strlen(pathname);
    int n = 0, b = 0;
    for (int j = 0; j < Size; ++j) {
        int tot1 = 0;
        int tot2 = 0;
        if (*(pathname + j) == '/' && *(pathname + j + 1) != '/') {
            for (int l = j + 1; l < Size; ++l) {
                if (*(pathname + j + 1) == '/') {
                    n = l;
                    break;
                } else n = Size;
            }
            if (n != Size) {
                for (int m = j + 1; m <= n; ++m) {
                    if (*(pathname + m - 1) == '.')
                        return -1;
                }
                for (int m = j + 1; m <= n; ++m) {
                    tmp1[tot1++] = *(pathname + m - 1);
                }
                Node *temp1 = search(tmp1, root);//匹配
                for (int i = n; i < Size; ++i) {
                    if (*(pathname + i) == '/' && *(pathname + i + 1) != '/') {
                        for (int k = i + 1; k < Size; ++k) {
                            if (*(pathname + k + 1) == '/') {
                                n = k;
                                break;
                            } else n = Size;
                        }
                        if (n != Size) {
                            for (int k = i + 1; k <= n; ++i) {
                                if (*(pathname + k - 1) == '.')
                                    return -1;
                            }
                            for (int k = i + 1; k <= n; ++k) {
                                tmp2[tot2++] = *(pathname + k - 1);
                            }
                            Node *temp2 = search(tmp2, root);//匹配
                            for (int k = 0; k < temp1->nrde; ++k) {
                                if (temp1->dirents[k] == temp2) {
                                    b = 1;
                                }
                            }
                            if (b == 1)return 0;
                            else return -1;//匹配/1/2
                        }
                    }
                }

            } else {
                for (int k = k + 1; k <= n; ++k) {
                    tmp1[tot1++] = *(pathname + k - 1);
                }
                Node *temp2 = search(tmp2, root);
                for (int k = 0; k < root->nrde; ++k) {
                    if (root->dirents[k] == temp2) {
                        b = 1;
                    }
                }
                if (b == 1)return 0;
                else return -1;//匹配
            }
        }
    }
}//查看是否合法(实现)

int ropen(const char *pathname, int flags) {
    if (is_right(pathname) == 0) {
        int Size = strlen(pathname);
        int head, nail;
        char tmp[40];
        int tot = 0;
        if (*(pathname + Size - 1) == '/') {
            for (int i = Size - 1; i >= 0; ++i) {
                if (*(pathname + i) != '/') {
                    nail = i;
                    break;
                }
            }
            for (int i = nail; i >= 0; ++i) {
                if (*(pathname + i) == '/') {
                    head = i + 1;
                    break;
                }
            }
            for (int i = head; i <= nail; i++) {
                tmp[tot++] = *(pathname + i - 1);
            }
            v[nn++].f->name = tmp;
        }
        if (*(pathname + Size - 1) != '/') {
            int m = 0;
            for (int i = Size - 1; i > 0; ++i) {
                if (*(pathname + i) == '/') {
                    head = i + 1;
                    break;
                }
            }
            for (int i = head; i <= nail; i++) {
                if (*(pathname + i - 1) == '.') { m = 1; }
                tmp[tot++] = *(pathname + i - 1);
            }
            if (m == 1) {
                v[nn++].f->name = tmp;
                v[nn - 1].flags = flags;
                v[nn - 1].offset = 0;
            } else {
                v[nn++].f->name = tmp;
            }
        }
        return nn - 1;
    } else {
        return -1;
    }
}

int rclose(int fd) {
    v[fd].f->name = NULL;
    //对结构体初始化
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
    // TODO();
}

ssize_t rread(int fd, void *buf, size_t count) {
}

off_t rseek(int fd, off_t offset, int whence) {
    if (whence == 0)v[fd].offset = offset;
    if (whence == 1)v[fd].offset = v[fd].offset + offset;
    if (whence == 2)v[fd].offset = strlen(v[fd].f->content) - 1 + offset;
}

int rmkdir(const char *pathname) {
    if (is_right(pathname) == 0) {
        return -1;
    } else {
        int Size = strlen(pathname);
        int head, nail, head1, nail1;
        int tot = 0;
        if (*(pathname + Size - 1) == '/') {
            for (int i = Size - 1; i > 0; --i) {
                if (*(pathname + i) != '/') {
                    nail = i;
                }
            }
            for (int i = nail; i > 0; --i) {
                if (*(pathname + i) == '/') {
                    head = i;
                }
            }
            for (int i = head + 1; i <= nail; i++) {
                tmp1[tot++] = *(pathname + i - 1);
            }
            if (head == 0) {

                root->dirents[root->nrde] = (Node *) malloc(sizeof(Node));
                strcpy(root->dirents[root->nrde]->name, tmp1);
                root->nrde = root->nrde + 1;
                root->dirents[root->nrde]->type = DIR_NODE;
            } else {

                for (int i = head; i > 0; --i) {
                    if (*(pathname + i) != '/') {
                        nail1 = i;
                    }
                }
                for (int i = nail1; i > 0; --i) {
                    if (*(pathname + i) == '/') {
                        head1 = i;
                    }
                }
                for (int i = head1 + 1; i <= nail1; i++) {
                    tmp2[tot++] = *(pathname + i - 1);
                }
                Node *temp = search(tmp2, root);
                if (temp == NULL)return -1;
                else {
                    temp->dirents[temp->nrde] = (Node *) malloc(sizeof(Node));
                    strcpy(temp->dirents[temp->nrde]->name, tmp2);
                    temp->nrde = temp->nrde + 1;
                    temp->dirents[temp->nrde]->type = DIR_NODE;
                }
            }
        } else {
            nail = Size - 1;
            for (int i = nail; i > 0; --i) {
                if (*(pathname + i) == '/') {
                    head = i;
                }
            }
            for (int i = head + 1; i <= nail; i++) {
                tmp1[tot++] = *(pathname + i - 1);
            }
            if (head == 0) {
                root->dirents[root->nrde] = (Node *) malloc(sizeof(Node));
                strcpy(root->dirents[root->nrde]->name, tmp2);
                root->nrde = root->nrde + 1;
                root->dirents[root->nrde]->type = DIR_NODE;
            } else {

                for (int i = head; i > 0; --i) {
                    if (*(pathname + i) != '/') {
                        nail1 = i;
                    }
                }
                for (int i = nail1; i > 0; --i) {
                    if (*(pathname + i) == '/') {
                        head1 = i;
                    }
                }
                for (int i = head1 + 1; i <= nail1; i++) {
                    tmp2[tot++] = *(pathname + i - 1);
                }
                Node *temp = search(tmp2, root);
                if (temp == NULL)return -1;
                else {
                    temp->dirents[temp->nrde] = (Node *) malloc(sizeof(Node));
                    strcpy(temp->dirents[temp->nrde]->name, tmp2);
                    temp->nrde = temp->nrde + 1;
                    temp->dirents[temp->nrde]->type = DIR_NODE;
                }
            }
        }
    }
}

int rrmdir(const char *pathname) {
    if (is_right(pathname) == 0) {
        int Size = strlen(pathname);
        int head, nail;
        char tmp[40];
        int tot = 0;
        if (*(pathname + Size - 1) == '/') {
            for (int i = Size - 1; i >= 0; ++i) {
                if (*(pathname + i) != '/') {
                    nail = i;
                    break;
                }
            }
            for (int i = nail; i >= 0; ++i) {
                if (*(pathname + i) == '/') {
                    head = i + 1;
                    break;
                }
            }
            memset(tmp, 0, sizeof(tmp));
            for (int i = head; i <= nail; i++) {
                tmp[tot++] = *(pathname + i - 1);
            }
        }
        if (*(pathname + Size - 1) != '/') {
            int m = 0;
            for (int i = Size - 1; i > 0; ++i) {
                if (*(pathname + i) == '/') {
                    head = i + 1;
                    break;
                }
            }
            for (int i = head; i <= nail; i++) {
                if (*(pathname + i - 1) == '.') {
                    m = 1;
                    tmp[tot++] = *(pathname + i - 1);
                }
            }
            if (m == 1) {
                return -1;
            } else {
                Node *tmp3 = search(tmp, root);//对应结构体
                if (tmp3->dirents != NULL)return -1;
                else {
                    free(tmp3);
                }
            }
        }
    } else {
        return -1;
    }
}//删除(是否打开？)

int runlink(const char *pathname) {
    if (is_right(pathname) == 0) {
        int Size = strlen(pathname);
        int head, nail;
        char tmp[40];
        int tot = 0;
        if (*(pathname + Size - 1) == '/') {
            return -1;
        }
        if (*(pathname + Size - 1) != '/') {
            int m = 0;
            for (int i = Size - 1; i > 0; ++i) {
                if (*(pathname + i) == '/') {
                    head = i + 1;
                    break;
                }
            }
            for (int i = head; i <= nail; i++) {
                if (*(pathname + i - 1) == '.') {
                    m = 1;
                    tmp[tot++] = *(pathname + i - 1);
                }
            }
            if (m != 1) {
                return -1;
            } else {
                Node *tmp3 = search(tmp, root);//是否打开？
                free(tmp3);
            }
        }
    } else {
        return -1;
    }


}//是否打开？

void init_ramfs() {
    root = (Node *) malloc(sizeof(Node));
    root->type = DIR_NODE;
    root->name = "/";
    root->dirents = 0;
    root->nrde = 0;
    root->size = 0;
}
