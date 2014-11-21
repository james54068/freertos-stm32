#include "shell.h"
#include <stddef.h>
//#include "clib.h"
#include <string.h>
//#include "fio.h"
//#include "filesystem.h"

#include "FreeRTOS.h"
#include "task.h"
//#include "host.h"
//#include "romfs.h"
#include "main.h"

typedef struct {
	const char *name;
	cmdfunc *fptr;
	const char *desc;
} cmdlist;

int  filedump_flag;

void ls_command(int, char **);
void lfs_command(int, char **);
void pwd_command(int, char **);
void man_command(int, char **);
void cat_command(int, char **);
void ps_command(int, char **);
void host_command(int, char **);
void help_command(int, char **);
void host_command(int, char **);
void test_command(int, char **);

#define MKCL(n, d) {.name=#n, .fptr=n ## _command, .desc=d}

cmdlist cl[]={
	MKCL(ls, "List directory"),
	MKCL(lfs, "List File system"),
	MKCL(pwd, "Show the true path in host"),
	MKCL(man, "Show the manual of the command"),
	MKCL(cat, "Concatenate files and print on the stdout"),
	MKCL(ps, "Report a snapshot of the current processes"),
	MKCL(host, "Run command on host"),
	MKCL(help, "help"),
	MKCL(test, "test new function")
};

int parse_command(char *str, char *argv[]){
	int b_quote=0, b_dbquote=0;
	int i;
	int count=0, p=0;
	for(i=0; str[i]; ++i){
		if(str[i]=='\'')
			++b_quote;
		if(str[i]=='"')
			++b_dbquote;
		if(str[i]==' '&&b_quote%2==0&&b_dbquote%2==0){
			str[i]='\0';
			argv[count++]=&str[p];
			p=i+1;
		}
	}
	/* last one */
	argv[count++]=&str[p];

	return count;
}

void ls_command(int n, char *argv[]){

	/*
	char buf[128];
	int k = 0;
	int fd;
	const char * path = argv[1];
	uint8_t buff; 
	
	switch(n)
	{
		case 1:
		
		fio_printf(2, "\r\nPlease enter file system name check lfs \r\n");
		break;
		
		case 2:
		fs_path_list(path,buff);
		for (k=0;k<10;k++)
		{
			
			fd=romfs_set_opaque(buff);
				
				if(fd==OPENFAIL)/*OPENFAIL=-1*/
	/*				fio_printf(2, "\r\nFile Error!!\r\n");

			fio_printf(1, "\r\n");

			int count;
			while((count=fio_read(fd, buf, sizeof(buf)))>0){
				fio_write(1, buf, count);
			}
		}

		fio_close(fd);
		break;

	}

	fio_printf(1, "\r\n");*/
}

/*
int filedump(const char *filename){
	char buf[128];
	int fd;

	fd = fs_open(filename, 0, O_RDONLY);

	if(fd==OPENFAIL)/*OPENFAIL=-1*/
/*		return 0;

	fio_printf(1, "\r\n");

	int count;
	while((count=fio_read(fd, buf, sizeof(buf)))>0){
		fio_write(1, buf, count);
	}

	fio_close(fd);
	return 1;
}*/

void lfs_command(int n, char *argv[]){
 	
/*	char buf[1024];
	fs_list(buf); 
	fio_printf(1, "%s\r\n", buf);*/
}

void pwd_command(int n, char *argv[]){

	
/*
	filedump_flag = 0;
	if(n==1){/*no assigned directory*/
/*		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

	if(!filedump(argv[1]))
		fio_printf(2, "\r\n%s no such file or directory.\r\n", argv[1]);/*assigned wrong directory*/

//	fio_printf(1, "\r\n");
}

void ps_command(int n, char *argv[]){
	
/*	signed char buf[1024];
	vTaskList(buf);
        fio_printf(1, "\n\rName          State   Priority  Stack  Num\n\r");
        fio_printf(1, "*******************************************\n\r");
	fio_printf(1, "%s\r\n", buf + 2);	*/
}

void cat_command(int n, char *argv[]){

/*
	filedump_flag = 1;
	if(n==1){/*no assigned directory*/
/*		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

	if(!filedump(argv[1]))
		fio_printf(2, "\r\n%s no such file or directory.\r\n", argv[1]);/*assigned wrong directory*/

//	fio_printf(1, "\r\n");	
}

void man_command(int n, char *argv[]){

/*	if(n==1){
		fio_printf(2, "\r\nUsage: man <command>\r\n");
		return;
	}

	char buf[128]="/romfs/manual/";
	strcat(buf, argv[1]);

	if(!filedump(buf))
		fio_printf(2, "\r\nManual not available.\r\n");*/
}

void host_command(int n, char *argv[]){

 /*   int i, len = 0, rnt;
    char command[128] = {0};

    if(n>1){
        for(i = 1; i < n; i++) {
            memcpy(&command[len], argv[i], strlen(argv[i]));
            len += (strlen(argv[i]) + 1);
            command[len - 1] = ' ';
        }
        command[len - 1] = '\0';
        rnt=host_action(SYS_SYSTEM, command);
        fio_printf(1, "\r\nfinish with exit code %d.\r\n", rnt);
    } 
    else {
        fio_printf(2, "\r\nUsage: host 'command'\r\n");
    }*/
}

void help_command(int n,char *argv[]){

	int i;
	printf("\r\n");
	for(i=0;i<sizeof(cl)/sizeof(cl[0]); ++i){
		printf("%s - %s\r\n", cl[i].name, cl[i].desc);
	}
}

void test_command(int n, char *argv[]) {
	printf("This is test command\r\n");
 /*   int handle;
    int error;
    int number = 0;
    char fib_buf[10];
    
    fio_printf(1, "\r\n");

    handle = host_action(SYS_OPEN, "output/fib", 8);
    if(handle == -1) {
        fio_printf(1, "Open file error!\n\r");
        return;
    }

    number = *argv[1] - 48;
    int fib = fibonacci(number); 
    sprintf(fib_buf,"%d",fib);  
  	
    char *buffer = fib_buf;
    //char *buffer = "Test host_write function which can write data to output/syslog\n";
    error = host_action(SYS_WRITE, handle, (void *)buffer, strlen(buffer));
    if(error != 0) {
        fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\n\r", error);
        host_action(SYS_CLOSE, handle);
        return;
    }

    host_action(SYS_CLOSE, handle);*/
}

cmdfunc *do_command(const char *cmd){

	int i;

	for(i=0; i<sizeof(cl)/sizeof(cl[0]); ++i){
		if(strcmp(cl[i].name, cmd)==0)
			return cl[i].fptr;
	}
	return NULL;	
}

int fibonacci(int number)
{
	int f0 = 0 ;
	int f1 = 1 ;
	int fn = 0 ;
	int i;

	if(number < 0 )
		return -1;
	else if (number == 0)
		return 0;
	else if (number == 1)
		return 1;

	for (i=0;i<(number-1);i++)
	{
		fn = f0 + f1;
		f0 = f1;
		f1 = fn;
	}
	return fn;
}

