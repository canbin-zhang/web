 #include <stdio.h>
 #include <stdlib.h>
         int main(void)
         {
             char *data;
             long m,n;
             printf("Content-type: text/html\n\n");
             printf("<TITLE>Mult Result</TITLE>");
	     printf("<HEAD><meta charset='UFT-8'></HEAD>");
           printf("<H3>计算结果：</H3>");
  
          data = getenv("QUERY_STRING");
          if(data == NULL)
              printf("<P>请输入数字！");
          else if(sscanf(data,"m=%ld&n=%ld",&m,&n)!=2)
             printf("<P>数据错误，请输入正确的数字格式！");
          else
              printf("<P>%ld and %ld result: %ld", m, n, m * n);
          printf("<br><h>感谢使用本服务器！！！</h1>");
 
          return 0;
    }

