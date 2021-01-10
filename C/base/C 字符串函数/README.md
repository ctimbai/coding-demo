###  01 strchr

**原型：** char *strchr(const char *str, int c) 

**意思：** 在字符串 str 中查找字符 c 第一次出现的位置

**返回：** 若存在，返回从字符 c 开始之后的字符串，否则返回 NULL。



### 02 strrchr

**原型：** char *strrchr(const char *str, int c)

**意思：** 在字符串 str 中查找字符 c 最后一次出现的位置

**返回：** 若存在，返回从字符 c 开始之后的字符串，否则返回 NULL。



### 03 memchr

**原型：** void *memchr(const void *str, int c, size_t n)

**意思：** 在字符串 str 所指的前 n 个字符中查找字符 c 第一次出现的位置

**返回：** 若存在，返回从字符 c 开始之后的字符串，否则返回 NULL。



### 04 strstr

**原型：** char *strstr(const char *str, const char *substr)

**意思：** 在字符串 str 中查找第一次出现 substr 的位置，不包含终止符 '\0'

**返回：** 若存在，返回从字符串 substr 开始之后的字符串，否则返回 NULL。



### 04 memcmp

**原型：** int memcmp(const void *str1, const void *str2, size_t n)

**意思：** 比较 str1 和 str2 的前 n 个字节

**返回：** 若 str1 < str2，返回 <0；若 str1 > str2，返回 > 0; 若 str1 = str2，返回 = 0。



### 05 strcmp

**原型：** int strcmp(const char *str1, const char *str2)

**意思：** 比较 str1 和 str2 指向的字符串

**返回：** 若 str1 < str2，返回 <0；若 str1 > str2，返回 > 0; 若 str1 = str2，返回 = 0。



### 06 strncmp

**原型：** int strncmp(const char *str1, const char *str2)

**意思：** 比较 str1 和 str2 指向的字符串的前 n 个字节

**返回：** 若 str1 < str2，返回 <0；若 str1 > str2，返回 > 0; 若 str1 = str2，返回 = 0。



### 07 memcpy

**原型：** void *memcpy(void *dst, const void *src, size_t n)

**意思：** 从 src 复杂 n 个字符到 dst

**返回：** 返回指向 dst 的指针。



### 08 memmove

**原型：** void *memmove(void *str, const void *src, size_t n)

**意思：** 也是用于从 src 复制 n 个字符到 dst，**但 memmove 能够处理内存重叠，比比 memcpy 更安全**

**返回：** 返回指向 dst 的指针。



### 09 strncpy

**原型：** char *strncpy(void *dst, const char *src, size_t n)

**意思：** 从 src 复杂 n 个字符到 dst，当 src 长度小于 n 时，dst 的剩余部分将用空字节填充

**返回：** 最终复制的字符串



### 10 memset

**原型：** void *memchr(void *str, int c, size_t n)

**意思：** 复制字符 c 到 str 指向的字符串的前 n 个字符

**返回：** 指向 str 的指针



### 11 strcat

**原型：** char *strcat(char *dst, const char *src)

**意思：** 把 src 指向的字符串追加到 dst 指向字符串的结尾

**返回：** 指向 dst 的指针



### 12 strncat

**原型：** char *strcat(char *dst, const char *src, size_t n)

**意思：** 把 src 指向的字符串追加到 dst 指向字符串的结尾，直到 n 字符长度为止

**返回：** 指向 dst 的指针



### 13 strspn

**原型：** size_t strspn(const char *str1, const char *str2)

**意思：** 查找字符串 **str1** 中第一个不在字符串 **str2** 中出现的字符下标。

**返回：** str1 中第一个不在字符串 str2 中出现的字符下标。



### 14 strcspn

**原型：** size_t strcspn(const char *str1, const char *str2)

**意思：** 查找 str1 连续有几个字符不包含 str2 中的字符，也就是说下一个字符的位置就是 str2 的 index，**可以用这个函数来得到 str2 在 str1 中的 index**

**返回：** str1 开头连续都不含字符串 str2 中字符的字符数。



### 15 strerror

**原型：** char *strerror(int errnum)

**意思：** 根据错误号（errnum，众所周知的），返回一个指向错误消息字符串的指针

**返回：** 一个指向错误字符串的指针，该错误字符串描述了错误 errnum。



### 16 strpbrk

**原型：** char *strpbrk(const char *str1, const char *str2)

**意思：** 检索字符串 *str1* 中第一个匹配字符串 *str2* 中字符的字符，不包含空结束字符。也就是说，依次检验字符串 str1 中的字符，当被检验字符在字符串 str2 中也包含时，则停止检验，并返回该字符位置。

**返回：** 若存在，返回从 str1 中第一个匹配的字符开始之后的字符串，否则返回 NULL。



### 17 strtok

**原型：** char *strtok(char *str, const char *delim)

**意思：** 根据分隔符 delim 分解 str 为一组字符串

**返回：** 被分解的第一个子字符串，如果没有可检索的字符串，则返回 NULL。

**用法：**

```C
#include <string.h>
main(){
    char s[] = "ab-cd : ef;gh :i-jkl;mnop;qrs-tu: vwx-y;z";
    char *delim = "-: ";
    char *p;
    printf("%s ", strtok(s, delim));
    while((p = strtok(NULL, delim)))
        printf("%s ", p);
        printf("\n");
}
```

