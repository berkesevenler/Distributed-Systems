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

void input() 
{
    printf("Enter Text: ");
    memset(buffer, 0, MAX_STRING_SIZE);
    fgets(buffer, MAX_STRING_SIZE, stdin);
}

int main()
{
    printf("Communication test (exit with CTRL+c)\n");
    
    int running=1;  
    int msgid;  
    struct my_msg some_data;  
    //char buffer[50]; //array to store user input (NOT VALID) 
    msgid=msgget((key_t)12345,0666|IPC_CREAT);  
    
    if (msgid == -1) // -1 means the message queue is not created  
    {  
        printf("Error in creating queue\n");  
        exit(0);  
    }
    
    while(running) 
    {
        input();
        some_data.msg_type=1;  
        strcpy(some_data.some_text,buffer);
        
        if(msgsnd(msgid,(void *)&some_data, MAX_STRING_SIZE,0)==-1) // msgsnd returns -1 if the message is not sent  
        {  
            printf("Msg not sent\n");  
        }  
                 
        if(strncmp(buffer,"end",3)==0)  
        {  
            running=0;  
        } 
    }

    return 0;
}