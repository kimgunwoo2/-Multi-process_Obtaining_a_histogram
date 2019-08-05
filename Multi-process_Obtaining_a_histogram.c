#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
void history_init();
void file_rw();
void file_count(int *file_counter,int num_start,int num_end,int process_num);
int main(int argc, char * argv[]){
    
    int num_start,num_end,process_num;
    int status;
    int tid=0;
    int *file_counter;
    struct timeval time_start,time_end;
    num_start=atoi(argv[1]);
    num_end=atoi(argv[2]);
    if(argc==4)
     process_num=atoi(argv[3]);
    else if(argc==3)
    process_num=1;
    pid_t pid[process_num];
    file_counter=(int *)malloc(sizeof(int)*(process_num+1));
    file_count(file_counter,num_start,num_end,process_num);
    if(argc != 3 && argc != 4){
        printf("usage:hw1 A B C((1<=(A and B)<=128) && (A < B) 만약 C가 없으면 1 , 1<=C<=128 \n ");
        return -1;
    }
if((1>num_start)||(num_end>128)||(num_start>num_end)){
     printf("usage:hw1 A B C((1<=(A and B)<=128) && (A < B) 만약 C가 없으면 1 , 1<=C<=128 \n ");
        return -1;
    }
    
    history_init(); //histogram.bin 0으로 초기
   
        
    
    for(int i=0; i<process_num; i++){
            tid++;
            pid[i]=fork();
            if(i==0){ gettimeofday(&time_start,NULL);}
            if(pid[i]<0){ //실패 했을 경우
            printf("error");
            return 0;                    
            }else if(pid[i]==0){  //child
                struct timeval ptime_start,ptime_end;
                gettimeofday(&ptime_start,NULL);
                printf("tid번호 %d 프로세스 담당 파일 %d ~ %d\n",tid,file_counter[tid-1]+1,file_counter[tid]);
                file_rw(file_counter[tid-1]+1,file_counter[tid]);
                gettimeofday(&ptime_end,NULL);
                float ptimevalue=ptime_end.tv_sec-ptime_start.tv_sec+(ptime_end.tv_usec-ptime_start.tv_usec)/1000000.0;
                printf("tid 번호 %d 프로세스 수행시간 %fmses \n",tid,ptimevalue*1000);
                exit(0);
            }
             
    }
    for(int i=0; i<process_num; i++){
    if(pid[i]>0){
            
            wait(&status);
             if(i==process_num-1){gettimeofday(&time_end,NULL);}
    }
}
        
        float tvalue=time_end.tv_sec-time_start.tv_sec+(time_end.tv_usec-time_start.tv_usec)/1000000.0;
        printf("총시간 %f msec\n",tvalue*1000);
}

void file_rw(int num_start,int num_end){
    char name[20] = "data";
       int file_his;
    FILE *fp;
        int histogram[256] = {0,};
        int hisSum[256] = {0,};
    int ch;

    int rock_res;    
    for(int a=num_start; a<=num_end; a++){
        sprintf(name,"data%d.bin",a);
        for(int j= 0; j <256; j++){
                histogram[j] = 0;
        }
        fp=fopen(name,"rb");
        
        
        while((ch=fgetc(fp))!=EOF){
            
                        histogram[ch]++;    
             }    
        fclose(fp);        
        file_his= open("histogram.bin",O_RDWR);
        lockf(file_his,F_LOCK,0);
        read(file_his,hisSum,1024);
        for(int i=0; i<256; i++)
            hisSum[i]=hisSum[i]+histogram[i];
        lseek(file_his,(off_t)0,SEEK_SET);
        write(file_his,hisSum,1024);
        lockf(file_his,F_ULOCK,0);
               close(file_his);
        
}
}
void history_init(){
        int file;
    int hisSum[256] = {0,};
         file = open("histogram.bin",O_RDWR);
            lseek(file,(off_t)0,SEEK_SET);
            write(file,hisSum,1024);
            close(file);
}
void file_count(int *file_counter,int num_start,int num_end,int process_num){

    int file_snum=num_end-num_start+1;
    int rem=file_snum%process_num;
    int count=0;
    file_counter[0]=num_start-1;
    for(int i=0; i<process_num; i++){
    if(rem!=0){
        file_counter[i+1]=(file_snum/process_num)*(i+1)+1+count+(num_start-1) ;
        rem--;
        count++;
    }else{
    file_counter[i+1]=(file_snum/process_num)*(i+1)+count+(num_start-1);
}
    
}
} 