## getopt

**头文件：** `#include <unistd.h>`

**原型：** `int getopt(int argc, char * const argv[], const char * optstr);`

**说明：** 解析 main 传入的命令行参数，argc 和 argv 分别是 main 传入的参数个数和内容，optstr 表示欲处理的选项字符串

**返回：** 返回在 argv 中下一个和 optstr 对应的选项字符，如果参数不包含在 optstr 中的选项字母，则返回 "?" 字符，分析结束则返回 -1。

**注意：** 这个函数还隐藏着一个全局变量 optarg，当 optstr 指向的选项字符串里的字符后接着冒号 ":"，则表示还有相关参数，optarg 即指向该参数。

**用法：**

```C
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
   int ch;
   opterr = 0;
   while((ch = getopt(argc, argv, "a:bcde")) != -1)
   switch(ch)
   {
      case 'a':
         printf("option a:'%s'\n", optarg);  break;
      case 'b':
         printf("option b :b\n");  break;
      default:
         printf("other option :%c\n", ch);
   }
   printf("optopt +%c\n", optopt);
}

# 执行：
$. /getopt -b
option b:b

$. /getopt -c
other option:c

$. /getopt -a
other option :?
    
$. /getopt -a12345
option a:'12345'
```

