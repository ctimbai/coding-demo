## C 结构体初始化的 3 种方法

1. {} 赋值
2. .val = x 前缀“点”赋值
3. memset 或 bzero 初始化为空



**例子：** 

```c
#include <stdio.h>

struct st1 {
    double val;
};

struct st2 {
    int i;
    char c;
    struct st1 s;
};

static void print(struct st2 s)
{
    fprintf(stdout, "the result is st2.i=%d,st2.c=%c,st2.st1.val=%f\n", 
        s.i, s.c, s.s.val);
}

int main(int argc, char const *argv[])
{
    /* 方法一 {} */
    struct st2 s1 = {1, 'a', {1.2}};
    print(s1);

    /* 方法二 指定初始化，成员顺序可以不定，linux内核多采用此方式 */
    struct st2 s2 = {
        .i = 2,
        .c = 'b',
        .s = {
            .val = 2.1
        }
    };
    print(s2);

    /* 方法三 memset bzero */
    return 0;
}
```

