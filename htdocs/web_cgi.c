#include <stdio.h>
#include <stdlib.h>
#include<string.h>
         int main(void)
         {
             char *data;
             long m,n;
	     char c[10];
	     char a[2];
             printf("Content-type: text/html\n\n");
             printf("<TITLE> Result</TITLE>");
	     printf("<HEAD><meta charset='UTF-8'></HEAD>");
           printf("<H3>计算结果：</H3>");
          data = getenv("QUERY_STRING");
          if(data == NULL)
              printf("<P>有数据为空，请输入数据！！！");
	  else 
	  {
		 
		  sscanf(data,"m=%ld&n=%ld",&m,&n);
		  int i=0;
		  int j=0;
		  for(i=0;i<strlen(data);i++)
		  {
			  if(data[i]=='e'&& data[i+1]=='=')
			  {
			
	               for(j=0;j<3;j++){
			       c[j]=data[i+2+j];
		       }
		
       }
  }
if(c[0]=='a'){
	printf("%ld + %ld = %ld",m,n,m+n);
}if(c[0]=='s'){
	printf("%ld - %ld = %ld",m,n,m-n);
}if(c[0]=='m'){
	printf("%ld × %ld = %ld",m,n,m*n);
}if(c[0]=='d'){
	printf("%ld ÷ %ld = %ld",m,n,m/n);
}
	  printf("<br><h>感谢使用本服务器！！！</h1>");
 
          return 0;
    }
}
