#include <fcntl.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <memory>
#include <sys/stat.h>
using namespace std;
int main(int argc, char *argv[]){
        char buff[4096];
        string tmpStr, tmpStrPath,outStrPath,inputDir_str;
        int source, dest;
        DIR *src_dir;
        DIR *dest_dir;
        struct dirent *pDirent;
        int error;
        size_t info=1;
        if(argc < 3){
        	write(STDOUT_FILENO,"program <sourcefile> <destinationfile>\n",40);
                exit(0);
        }else{
                int res = strcmp(argv[1],"-r");
                if(res == 0){
                        src_dir = opendir(argv[2]);
                        if(src_dir == NULL){
                                return -1;
                        }
                }else{
                        source = open(argv[1],O_RDONLY);

                        if(source < 0){
                                write(STDOUT_FILENO,"error opening source file\n", 27);
                                return -1;
                        }
                }
                if(res == 0){
                        dest_dir = opendir(argv[3]);
                        if(dest_dir != NULL){
                                src_dir = opendir(argv[2]);
                                while((pDirent = readdir(src_dir)) != NULL){
                                        tmpStr = pDirent->d_name;
                                        if(strcmp(pDirent->d_name,".") != 0 && strcmp(pDirent->d_name,"..")!=0){
                                                tmpStrPath = argv[2];
                                                tmpStrPath.append("/");
                                                tmpStrPath.append(tmpStr);
                                                struct stat buf;
                                                stat(tmpStrPath.c_str(),&buf);
                                                S_ISREG(buf.st_mode);
                                                source = open(tmpStrPath.c_str(),O_RDONLY);
                                                outStrPath = argv[3];
                                                outStrPath.append("/");
                                                outStrPath.append(tmpStr);
                                                dest = open(outStrPath.c_str(),O_WRONLY|O_CREAT|O_TRUNC|O_SYNC,0666);
                                                if(dest < 0){
                                                        error = errno;
                                                        close(source);
                                                        if(dest >= 0){
                                                                close(dest);
                                                         }
                                                        errno = error;
                                                        return -1;
                                                }
                                                goto reading;

                                        }
                                }
                        }else{
                                return -1;
                        }


                }else{
                        dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_SYNC,0666);

                        if(dest < 0){
                                error = errno;
                                close(source);
                                if(dest >= 0){
                                        close(dest);
                                }
                                            errno = error;
                                return -1;
                        }
                        goto reading;
                }
                reading:
                        while(info > 0){
                                char* output = buff;
                                size_t input;
                                while(info > 0){
                                        input = write(dest,output,info);
                                        if(input >=0){
                                                info -= input;
                                                output += input;
                                        }else if(errno != EINTR){
                                                error = errno;
                                                close(source);
                                                if(dest >= 0){
                                                        close(dest);
                                                }
                                                errno = error;
                                                return -1;
                                                                         }
                                }
                                info = read(source, buff, sizeof buff);
                        }
                        if(info == 0){
                                if(close(dest) < 0){
                                        dest = -1;
                                        error = errno;
                                        close(source);
                                        if(dest >=0){
                                                close(dest);
                                        }
                                        errno = error;
                                        return -1;
                                }
                                close(source);
                                return 0;
                        }

        }
}
                
