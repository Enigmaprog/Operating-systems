#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define FTW_F 1 //файл, не являющийся каталогом
#define FTW_D 2 //каталог
#define FTW_DNR 3 //каталог, недоступный для чтения
#define FTW_NS 4 //файл, информацию о котором нельзя получить с помощью stat

// тип функции, которая будет вызываться для каждого встреченного файла
typedef int MyFunc(const char * ,const struct stat *, int);

static MyFunc counter;
static int myftw(char *, MyFunc * );
static int dopath(const char* filename, int depth, MyFunc * );

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char * argv[]) {
    int ret = -1;
    if (argc != 2) {
        fprintf(stderr, "Пример запуска: ./lab.out <каталог>\n");   
        return EXIT_FAILURE;
    }

    ret = myftw(argv[1], counter);

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;

    if (ntot == 0)
        ntot = 1; //во избежание деления на 0; вывести 0 для всех счетчиков

    printf("\nобычные файлы:\t%7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
    printf("каталоги:\t%7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
    printf("специальные файлы блочных устройств:\t%7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
    printf("специальные файлы символьных устройств:\t%7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
    printf("FIFO\t%7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
    printf("символьные ссылки:\t%7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
    printf("сокеты:\t%7ld, %5.2f %%\n\n", nsock, nsock*100.0/ntot);

    return ret;
}

// Обходит дерево каталогов, начиная с pathname и применяя к каждому файлу func
static int myftw(char * pathname, MyFunc * func) {
    return(dopath(pathname, 0, func));
}

static int dopath(const char * filename, int depth, MyFunc * func) {
    struct stat statbuf;
    struct dirent * dirp;
    DIR * dp;
    int ret = 0;

    if (lstat(filename, &statbuf) == -1) // Ошибка вызова функции lstat
        return(func(filename, &statbuf, FTW_NS));
    
    for (int i = 0; i < depth; ++i)
        printf("|\t");
    
    if (S_ISDIR(statbuf.st_mode) == 0) // Не каталог
        return(func(filename, &statbuf, FTW_F)); // Отобразить в дереве

    if ((ret = func(filename, &statbuf, FTW_D)) != 0)
        return(ret);

    if ((dp = opendir(filename)) == NULL) // Каталог недоступен
        return(func(filename, &statbuf, FTW_DNR));

    chdir(filename);
    while ((dirp = readdir(dp)) != NULL && ret == 0)
    {
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) //Пропустить каталоги . и ..
        {
            ret = dopath(dirp->d_name, depth + 1, func);
        }
    }

    chdir("..");
    
    if (closedir(dp) < 0)
        perror("Невозможно закрыть каталог");
    return(ret);
}
    
static int counter(const char* filename, const struct stat * statptr, int type)
{
    switch(type)
    {
        case FTW_F:
            printf( "|--- %s\n", filename);
            switch(statptr->st_mode & S_IFMT)
            {
                case S_IFREG: nreg++; break;
                case S_IFBLK: nblk++; break;
                case S_IFCHR: nchr++; break;
                case S_IFIFO: nfifo++; break;
                case S_IFLNK: nslink++; break;
                case S_IFSOCK: nsock++; break;
                case S_IFDIR:
                    fprintf(stderr, "Каталог имеет тип FTW_F");
                    return EXIT_FAILURE;
            }
            break;
        case FTW_D:
            printf( "|--- %s/\n", filename);
            ndir++; break;
        case FTW_DNR:
                fprintf(stderr, "Закрыт доступ к одному из каталогов!");
                return EXIT_FAILURE;
        case FTW_NS:
                fprintf(stderr, "Ошибка функции stat!");
                return EXIT_FAILURE;
        default:
                fprintf(stderr, "Неизвестый тип файла!");
                return EXIT_FAILURE;
    }
    return(0);
}