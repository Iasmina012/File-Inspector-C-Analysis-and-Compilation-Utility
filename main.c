#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int verificareFisier(struct dirent * fisier){
   
	//verificam daca ultimele 2 caractere sunt .c reprezentand extensia si returnam 1 daca e adv
    if((fisier->d_name[strlen(fisier->d_name)-1] == 'c') && (fisier->d_name[strlen(fisier->d_name) - 2] == '.')) 
        return 1;
    else return 0;
}

int main(int argc, char *argv[])
{
    DIR *dir;             
    struct stat st1;    
    struct dirent *fisier;  
    char aux[10];        
    int length = 0;       
    int contor = 1;          
    char path1[100];
    char path2[100];
    int p_op;
    int pfd1[2],pfd2[2]; 
    int pid, status;
    int pid1,pid2,pid3;

    if (argc != 3)  //verificam numarul argumentelor
        {
            printf("Usage: ./executabil <director> <optiuni>\n"); 
            return 0;
        }

    dir = opendir(argv[1]); //deschidem directorul, adica al doilea argument                           
    if (dir == NULL )  //daca este gol atunci afisam un mesaj de eroare
      printf("Eroare deschidere.\n");

    /*
    vechea citire din fisier care lua un singur fisier
    fisier = readdir(dir);                                     
    if(fisier == NULL)
       printf("Erore parcurgere fisier!\n");
    */
 
    //noua citire din fisier
	//citim in while si cat timp nu este gol il verificam daca este fisier .c
    while((fisier = readdir(dir)) && (fisier != NULL))
     {
       if(verificareFisier(fisier)) //daca este fisier .c putem parcurge optiunile    
        {
          length = strlen(argv[2]) - 1; //lungimea sirului
          contor = 1; //contorul, numarul de optiuni

          //parcurgem sirul de optiuni, al treilea argument 
          strcpy(aux,argv[2]);  //copiem sirul intr-un vector
          strcpy(path1,argv[1]); //copiem arg cu directorul in path1
          strcat(path1,"/"); //concatenam "/"
          strcat(path1,fisier->d_name); //adaugam numele fisierului


          
          if (strchr (argv[2], 'p') != NULL) //daca optiunea este p se creaza pipe
          {
            p_op = 1;
            if (pipe(pfd1) < 0)
            {
              perror( "Eroare la crearea pipe-ului\n");
              exit(1);
            }
            if (pipe(pfd2) < 0)
            {
              perror( "Eroare la crearea pipe-ului\n");
              exit(1);
            }
            
          }

        //fiul 1 cu gcc;compilare
          if (strchr(argv[2], 'g') != NULL) //pentru varianta -g folosim gcc pentru a compila fisierul .c
          {
            
            pid=fork();

            if (pid == 0) //procesul pentru optiunea -g
            {
               if(p_op)
              {
                dup2(pfd1[1],2); //duplicarea descriptorului 2, returnand noul descriptor
                close(pfd1[0]); //inchid capatul de citire
                close(pfd1[1]); //inchid capatul de scriere
              }
              path2[strlen(path2) - 2] = '\0';
              execlp("gcc", "gcc", "-o", path2, path1, NULL); //executam gcc
              exit(0);
            }
            wait(&status); //asteapta statusul
            printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid, WEXITSTATUS(status));
          }

        //fiul 2 cu filtrarea erorilor
          if(p_op)
          {
            if ((pid3 = fork()) < 0)
            {
              perror( "Eroare la fork");
              exit(1);
            }

            if(pid3 == 0)
            {
              dup2(pfd1[0],0); //duplicarea descriptorului 0, returnand noul descriptor
              dup2(pfd2[1],1); //duplicarea descriptorului 1, returnand noul descriptor
              close(pfd1[1]); //inchid capatul de scriere
              close(pfd2[0]); //inchid capatul de citire
              execlp("egrep","egrep","(error)|(warning)",NULL); //execut grep
              exit(0);
            }
          } 

          if ((pid1 = fork()) < 0)
            {
              perror( "Eroare la fork");
              exit(1);
            }
          if (pid1 == 0) //procesul pentru switch, celelalte optiuni din sir
          {
            length = strlen(argv[2]) - 1;
            contor = 1;

          while(length>0) //cat timp exista optiuni in sir
           {
            if(stat(path1,&st1) == 0)
            {
             switch (aux[contor]) 
                {

                  //numele fisierului
                  case 'n': printf("Nume: %s\n",fisier->d_name);
                            break;

                  //utilizatorul fisierului
                  case 'u':
                             if(stat(path1,&st1) == 0)
                             {
                               printf("Utilizator: %d\n",st1.st_uid);
                             }
                            break;

                  //drepturile de permisiune
                  case 'a': printf("Utilizator:\nRead - %s\n", st1.st_mode & S_IRUSR ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWUSR ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWUSR ? "DA" : "NU");
                            printf("Execute - %s\n", st1.st_mode & S_IXUSR ? "DA" : "NU");

                            printf("Grup:\nRead - %s\n", st1.st_mode & S_IRUSR ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWGRP ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWGRP ? "DA" : "NU");
                            printf("Execute - %s\n", st1.st_mode & S_IXGRP ? "DA" : "NU");

                            printf("Altii:\nRead - %s\n", st1.st_mode & S_IRUSR ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWOTH ? "DA" : "NU");
                            printf("Write - %s\n", st1.st_mode & S_IWOTH ? "DA" : "NU");
                            printf("Execute - %s\n", st1.st_mode & S_IXOTH ? "DA" : "NU");
                            break;

                  //dimensiunea in octeti a fisierului folosind stat
                  case 'd': if(stat(path1,&st1) == 0)
                             {
                               printf("Dimensiune in octeti: %ld\n",st1.st_size);
                             }
                             break;

                  //numarul de legaturi ale fisierului folosind stat
                  case 'c': if(stat(path1,&st1) == 0)
                             {
                               printf("Numarul de legaturi: %lu\n",st1.st_nlink);
                             }
                            break;
                  
                }
            }else printf("eroare");
            length--; //lungimea sirului scade
            contor++; //creste contorul practic pozitia din vectorul de optiuni
          }
          exit(0);
          }
          wait(&status); //asteapta statusul
          printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid1, WEXITSTATUS(status));

      if ((pid2 = fork()) < 0) //crearea procesului pentru legaturi
            {
              perror( "Eroare la fork");
              exit(1);
            } 
      if(pid2 == 0) //procesul pentru legaturi
      {
        if (st1.st_size < 1024 * 100) //daca dimensiunea e mai mica decat 1024 KB atunci cream o legatura noua
        {
           strcpy(path2, path1);
           path2[strlen(path2) - 2] = '\0';
           unlink(path2);
           symlink(path1,path2);
        }
        exit(0);
      }
      wait(&status); //asteapta statusul
      printf("Procesul cu PID <%d> s-a terminat cu codul <%d>.\n", pid2, WEXITSTATUS(status));

    }

    }
    return 0;
}
