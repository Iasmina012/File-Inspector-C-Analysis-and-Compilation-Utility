#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


int verifFile(struct dirent *file) {
    char* a = file->d_name;  //se parcurge fiesierul
    if((file->d_name[strlen(file->d_name)-1] == 'c') && (file->d_name[strlen(file->d_name) - 2] == '.')) //pune pe ultima pozitie litera 'c' si penultima pozitie punctul care formeaza extensia .c
    return 1; 
    else return 0;
}

int main(int argc, char *argv[]) {

    DIR *dir;             
    struct stat st1;      
    struct dirent *file;  
    char aux[10];         //se declara datele
    int length = 0;       
    int cnt = 1;          
    char path[100];       
    char path2[100];      

     if (argc != 3) {
            printf("Usage: ./executable <director> <optiuni>\n"); 
            return 0;
        }

    dir = opendir(argv[1]);                                 // se deschide
    if (dir == NULL ) 
       printf("error open file!\n"); //   fisier

    while((file = readdir(dir)) && (file != NULL)) 
      {
       if(verifFile(file))        // daca e fisier .c se prelucreaza conform cerintei
        {
          int pid, status;
          length = strlen(argv[2])-1;
          cnt = 1;
          strcpy(aux,argv[2]);   // parcurgem sirul "-nu..."
          strcpy(path,argv[1]);
	        strcat(path,"/");
		      strcat(path,file->d_name);

          if (strchr(argv[2], 'p') != NULL ) {
            strcpy(path2, path);
            path2[strlen(path2) - 2] = '\0';
            char buff[4096];
            int nr_warnings=0,nr_errors=0;
            FILE *stream=fopen(path,"r");
            while((fgets(buff,4096,stream)) != NULL){
                if(strstr(buff,"error") != NULL)
                    nr_errors++;
                if(strstr(buff,"warning") != NULL)
                    nr_warnings++;
            }
            int nota = 10;
            if(nr_errors >= 1)    //avem erori
                nota = 1;
            else{
                //nu avem erori
                if(nr_warnings > 10)
                    nota = 2;
                else{
                    nota = 2 + 8*(10 - nr_warnings)/10;
                }
            }
            printf("%40s:    nota %d\n", file->d_name, nota);
            pid = wait(&status);
            printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid, WEXITSTATUS(status));
          }
          if (strchr(argv[2], 'g') != NULL) {
            if (pid = fork() == 0){
              strcpy(path2, path);
              path2[strlen(path2) - 2] = '\0';
              execlp("gcc", "gcc", "-o", path2, path, NULL);
              exit(0);
            }
            pid = wait(&status);
            printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid, WEXITSTATUS(status));
          }
          pid = fork();
      if (pid == 0)
      {
        length = strlen(argv[2]) - 1;
        cnt = 1;
          
        while(length>0) {
          //if (stat(path, &st1) == 0) {
                switch (aux[cnt]) {
                    case 'n': printf("name_file: %s\n",file->d_name);
                              break;

                    case 'u': if(stat(path,&st1) == 0)
                             {
                               printf("Utilizator: %d\n",st1.st_uid);
                             }
                            break;

                    case 'a': printf("user:\n Read - %s\n", st1.st_mode & S_IRUSR ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWUSR ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWUSR ? "Da" : "NU");
                              printf("Execute - %s\n", st1.st_mode & S_IXUSR ? "Da" : "NU");

                              printf("grup:\nRead - %s\n", st1.st_mode & S_IRUSR ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWGRP ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWGRP ? "Da" : "NU");
                              printf("Execute - %s\n", st1.st_mode & S_IXGRP ? "Da" : "NU");

                              printf("others:\nRead - %s\n", st1.st_mode & S_IRUSR ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWOTH ? "Da" : "NU");
                              printf("Write - %s\n", st1.st_mode & S_IWOTH ? "Da" : "NU");
                              printf("Execute - %s\n", st1.st_mode & S_IXOTH ? "Da" : "NU");
                            break;                 

                    case 'd': if(stat(path,&st1) == 0)
                             {
                               printf("Dimensiune in octeti: %ld\n",st1.st_size);
                             }
                             break;

                    case 'c': if(stat(path,&st1) == 0)
                             {
                               printf("Numarul de legaturi: %lu\n",st1.st_nlink);
                             }
                            break;
                    }
        
                length--;
                cnt++;

              }
              exit(0);
      }
      wait(&status);
      printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid, WEXITSTATUS(status));

      pid = fork();
      if (pid == 0) {

                  
                  if (st1.st_size < 1024 * 100) {
                    strcpy(path2, path);
                    path2[strlen(path2)-2] = '\0';
                    unlink(path2);
                    symlink(path, path2);
                  }
                exit(0);
      }
      wait(&status);
      printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid, WEXITSTATUS(status));
        }
        //file = readdir(dir);
        }  
    
  return 0;
}