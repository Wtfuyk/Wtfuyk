#include "ramfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>//查找？fd整形？

typedef struct node {
    enum { FILE_NODE, DIR_NODE } type;
    struct node *dirents; // if it's root dir, there's subentries
    void *content; // if it's root file, there's data content
    int nrde; // number of subentries for dir
    int size; // size of file
    char *name; // it's short name
} Node;

Node root;

typedef struct FD {
    int offset;//偏移量
    int flags;//打开方式
    Node *f;
} FD;

//fd与FD关系

FD v[1000];int nn=0;

char tmp1[40];char tmp2[40];

int isright(const char *pathname){//查看是否合法
    int Size = strlen(pathname);
    int n=0;
    for (int j = 0; j < Size; ++j) {
        int tot1 = 0;int tot2=0;
        if(*(pathname + j) == '/' && *(pathname + j + 1) != '/'){
            for (int l = j + 1; l < Size; ++l) {
                if(*(pathname + j + 1) == '/'){
                    n=l;
                    break;
                }
                else n=Size;
            }
            if(n!=Size) {
                for (int m = j + 1; m <= n; ++m) {
                    if(*(pathname + m - 1) == '.')
                        return -1;
                }
                for (int m = j + 1; m <= n; ++m) {
                    tmp1[tot1++] = *(pathname + m - 1);
                }
                //匹配
                for (int i = n; i < Size; ++i) {
                    if (*(pathname + i) == '/' && *(pathname + i + 1) != '/') {
                        for (int k = i + 1; k < Size; ++k) {
                            if (*(pathname + k + 1) == '/'){
                                n = k;
                                break;
                            }
                            else n = Size;
                        }
                        if (n != Size) {
                            for (int k = i + 1; k <= n; ++i) {
                                if (*(pathname + k - 1) == '.')
                                    return -1;
                            }
                            for (int k = i + 1; k <= n; ++k) {
                                tmp2[tot2++] = *(pathname + k - 1);
                            }
                            //匹配
                            //匹配/1/2
                            break;
                        } else{
                            //匹配
                            //匹配/1/2
                            break;
                        }
                    }
                }

            }else{
                for (int k = k + 1; k <= n; ++k) {
                tmp1[tot1++] = *(pathname + k - 1);
            }
            //匹配
            }

        }
    }
}

int ropen(const char *pathname, int flags) {
    isright(pathname);
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
                head = i+1;
                break;
            }
        }
        memset(tmp, 0, sizeof(tmp));
        for (int i = head; i <= nail; i++) {
            tmp[tot++] = *(pathname + i - 1);
        }
        v[nn++].f->name = tmp;
    }
    if (*(pathname + Size - 1) != '/') {int m=0;
        for (int i = Size-1; i > 0; ++i) {
            if (*(pathname + i) == '/') {
                head = i+1;
                break;
            }
        }
        for (int i = head; i <= nail; i++){
            if(*(pathname + i - 1)=='.'){m=1;}
            tmp[tot++] = *(pathname + i - 1);
        }
        if(m==1){
        v[nn++].f->name=tmp;
        v[nn-1].flags=flags;
        v[nn-1].offset=0;
        }
        else{
            v[nn++].f->name=tmp;
        }
    }
    return v[nn-1];
}
int rclose(int fd) {
    fd.f.name=NULL;
    //对结构体初始化
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
    // TODO();
}

ssize_t rread(int fd, void *buf, size_t count) {
  // TODO();
}

off_t rseek(int fd, off_t offset, int whence) {



    if(whence==0)fd.offset=offset;
    if(whence==0)fd.offset=offset;
    if(whence==0)fd.offset=offset;


}

int rmkdir(const char *pathname) {  //未判定pn是否合法
    isright(pathname);
    int Size = strlen(pathname);
    int head,nail;
    char tmp[40];
    int tot = 0;
    if(*(pathname+Size-1)=='/'){
        for (int i = Size-1; i > 0 ; ++i) {
            if(*(pathname+i)!='/'){
                nail=i;
            }
        }
        for (int i = nail; i > 0 ; ++i) {
            if(*(pathname+i)=='/'){
                head=i;
            }
        }
        memset(tmp, 0, sizeof(tmp));
        for (int i = head; i < nail; i++) {
            tmp[tot++] = *(pathname+i-1);
        }
        if (root == NULL) return NULL;
        if (strcmp(root->v, name) == 0) return root;//
        Node *tmp = search(root->L, name);
        if (tmp != NULL) return tmp;
        else return search(root->R, name);
        strcmp()
    } else {

    }

    // TODO();
}

int rrmdir(const char *pathname) {
    isright(pathname);
    size_t Size = strlen(pathname);
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
                head = i+1;
                break;
            }
        }
        memset(tmp, 0, sizeof(tmp));
        for (int i = head; i <= nail; i++) {
            tmp[tot++] = *(pathname + i - 1);
        }
        //对应结构体
    }if (*(pathname + Size - 1) != '/') {int m=0;
        for (int i = Size-1; i > 0; ++i) {
            if (*(pathname + i) == '/') {
                head = i+1;
                break;
            }
        }
        for (int i = head; i <= nail; i++){
            if(*(pathname + i - 1)=='.'){m=1;}
            tmp[tot++] = *(pathname + i - 1);
        }
        if(m==1){
            return -1;
        }
        else{
            //对应结构体
            if(Node.d!=NULL &&不存在)return -1;
            else{
                free();
            }
        }
    }

    // TODO();
}

int runlink(const char *pathname) {
    isright(pathname);
    int Size = strlen(pathname);
    if(*(pathname+Size-1)=='/')return -1;
    else {
        int head, nail;
        char tmp[40];
        int tot = 0;
        int m=0;
        for (int i = Size-1; i > 0; ++i) {
            if (*(pathname + i) == '/') {
                head = i+1;
                break;
            }
        }
        for (int i = head; i <= nail; i++){
            if(*(pathname + i - 1)=='.'){m=1;}
            tmp[tot++] = *(pathname + i - 1);
        }
        if(m==1){
            if()
        }
        if(m==0){
            return -1;
        }
    }


    // TODO();
}

void init_ramfs() {
    root.type=DIR_NODE;
    root.name="/";
    root.dirents=0;
    root.nrde=0;
    root.size=0;
}
