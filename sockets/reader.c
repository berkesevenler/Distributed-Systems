#include<stdlib.h>  
#include<stdio.h>  
#include<string.h>  
#include<unistd.h>  
#include<sys/types.h>  
#include<sys/ipc.h>  
#include<sys/msg.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];

struct my_msg
{  
    long int msg_type;  
    char some_text[MAX_STRING_SIZE];  
};

void output()
{
    printf("Entered Text: %s\n", buffer);
}

int main()
{
    printf("Communication test (exit with CTRL+c)\n");
    
    int running=1;  
    int msgid;  
    struct my_msg some_data;  
    long int msg_to_rec=0;  
    msgid=msgget((key_t)12345,0666|IPC_CREAT);
    
    while(running)  
    {  
        msgrcv(msgid,(void *)&some_data,MAX_STRING_SIZE,msg_to_rec,0);
        strcpy(buffer, some_data.some_text);
        output();
        
        if(strncmp(buffer,"end",3)==0)  
        {  
            running=0;  
        }  
    }
    
    msgctl(msgid,IPC_RMID,0); 

    return 0;
}
