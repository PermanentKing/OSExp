#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_LINE 80
#define BUFFER_SIZE 50
char buffer[BUFFER_SIZE];
char *history[10][10];
int pos = 0;
int com_l[11] = {0};
 /* 每次输入的命令规定不超过80个字符 */
/* * setup() 用于读入下一行输入的命令，并将它分成没有空格的命令和参数存于数组args[]中，
 * 用NULL作为数组结束的标志 
 */
/*
 * setup() 用于读入下一行输入的命令，并将它分成没有空格的命令和参数存于数组args[]中，
 * 用NULL作为数组结束的标志 
 */



void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* 命令的字符数目 */
        i,      /* 循环变量 */
        start,  /* 命令的第一个字符位置 */
        ct;     /* 下一个参数存入args[]的位置 */
    
    ct = 0;
    
    /* 读入命令行字符，存入inputBuffer */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

    start = -1;
    if (length == 0) exit(0);            /* 输入ctrl+d，结束shell程序 */
    if (length < 0){ 
        perror("error reading the command");
	exit(-1);           /* 出错时用错误码-1结束shell */
    }    
/* 检查inputBuffer中的每一个字符 */
    for (i=0;i<length;i++) { 
        switch (inputBuffer[i]){
	    case ' ':
	    case '\t' :               /* 字符为分割参数的空格或制表符(tab)'\t'*/
		if(start != -1){
                    args[ct] = &inputBuffer[start];    
		    ct++;
		}
                inputBuffer[i] = '\0'; /* 设置 C string 的结束符 */
		start = -1;
		break;

            case '\n':                 /* 命令行结束 */
		if (start != -1){
                    args[ct] = &inputBuffer[start];     
		    ct++;
		}
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* 命令及参数结束 */
		break;

	    default :             /* 其他字符 */
		if (start == -1)
		    start = i;
                if (inputBuffer[i] == '&'){  
		    *background  = 1;          /*置命令在后台运行*/
                    inputBuffer[i] = '\0';
		}
	} 
     }    
     args[ct] = NULL; /* 命令字符数 > 80 */
} 


void handle_SIGINT(int signum)
{
    write(STDOUT_FILENO, buffer, strlen(buffer));
    printf("History Command is:\n");
    int i = pos;
    for (int count = 10; count > 0; count--){
      for (int j = 0; j < com_l[i]; j++){
        printf("%s ", history[i][j]);
      }
      printf("\n");
      i = (i + 1) % 10;
    }
    printf("\nCOMMAND->");
    fflush(stdout);
    return;
}




int main(void)
{
    char inputBuffer[MAX_LINE]; /* 这个缓存用来存放输入的命令*/
    int background;             /* ==1时，表示在后台运行命令，即在命令后加上'&' */
    char *args[MAX_LINE/2+1];/* 命令最多40个参数 */
    int i, j;
    for (int i = 0; i < 10; i++)
    {
      for (int j = 0; j < 10; j++)
      {
        history[i][j] = (char*)malloc(80 * sizeof(char));
      }
    }

    strcpy(buffer, "\nCaught Control C\n");
    signal(SIGINT, handle_SIGINT);

    while (1){            /* 程序在setup中正常结束*/
	     background = 0;
 	     printf("COMMAND->"); //输出提示符，没有换行，仅将字符串送入输出缓存
       fflush(stdout);                                            //若要输出输出缓存内容用fflush(stdout);头文件stdio.h
       setup(inputBuffer,args,&background);       /* 获取下一个输入的命令 */

       if (args[0]!=NULL && strcmp(args[0], "r")!=0)
       {
         if (args[0]!="\n")
         {
           for (i = 0;args[i]!=NULL;i++)
           {
             strcpy(history[pos][i], args[i]);
           }
           com_l[pos] = i;
           pos = (pos + 1) % 10;
         }
       }
       if (args[0]!=NULL && strcmp(args[0], "r")==0)
       {
         if (args[1]==NULL)
         {
           i = (pos + 9) % 10;
           for (int j = 0; j < com_l[i]; j++)
           {
             strcpy(history[pos][j], history[i][j]);
           }
           com_l[pos] = j;
           pos = (pos + 1) % 10;
         }
       }

       else{
        i = pos;
        for (int count = 0; count > 0; count--)
        {
          i = (i + 9) % 10;
          if (strncmp(args[1], history[i][0], 1)==0)
          {
            for (int j = 0; j < com_l[i]; j++)
            {
              strcpy(history[pos][j], history[i][j]);
            }
            com_l[pos] = j;
            pos = (pos + 1) % 10;
          }
        }
       }
	/* 这一步要做:
	 (1) 用fork()产生一个子进程
	 (2) 子进程将调用execvp()执行命令,即 execvp(args[0],args);
	 (3) 如果 background == 0, 父进程将等待子进程结束, 即if(background==0) wait(0);
	       否则，将回到函数setup()中等待新命令输入.
	*/
       int sub = fork();
       if(sub==0)
       {
          if (strcmp(args[0], "r")!=0)
          {
            execvp(args[0],args);
            int k = (pos - 1) % 10;
          }else{
            char *newargs[MAX_LINE / 2 + 1];
            for (int i = 0; i < MAX_LINE / 2 + 1; i++)
            {
              newargs[i] = (char*)malloc((MAX_LINE / 2 + 1) * sizeof(char));
            }
            pos = (pos + 9) % 10;
            history[pos][0] = '\0';
            if (args[1]==NULL)
            {
              i = (pos + 9) % 10;
              for (int j = 0; j < com_l[i]; j++)
              {
                strcpy(newargs[j], history[i][j]);
              }
              newargs[j] = NULL;
              execvp(newargs[0], newargs);
              exit(0);
            }else{
              i = pos;
              for (int count2 = 10 ; count2 > 0; count2--)
              {
                i = (i + 9) % 10;
                if (strncmp(args[i], history[i][0], 1)==0)
                {
                  for (int j = 0; j < com_l[i]; j++)
                  {
                    strcpy(newargs[j], history[i][j]);
                  }
                  newargs[j] = NULL;
                  execvp(newargs[0], newargs);
                }
              }
            }
            exit(0);
          }
          exit(0);
       }
       else
       {
       		if(background==0)
       			wait(0);
          else{
            setup(inputBuffer, args, &background);
          }
       }
    }
}
