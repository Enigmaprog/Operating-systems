#include <stdio.h>
#include <fcntl.h>

/*
    На моей машине размер буфера 20 байт.
   переведен в 12-символьный буфер.
   По-видимому, 8 байтов было израсходовано
   Библиотека stdio для бухгалтерского учета.
*/

int main()
{
    // иметь открытое соединение ядра с файлом алфавит.txt
    int fd = open("alphabet.txt", O_RDONLY);


    // создать два буферизованных потока ввода-вывода C, используя указанное выше соединение
    FILE *fs1 = fdopen(fd, "r");
    char buff1[20];
    setvbuf(fs1, buff1, _IOFBF, 20);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[20];
    setvbuf(fs2, buff2, _IOFBF, 20);

    
    // прочитать символ и записать его поочередно из fs1 и fs2
    int flag1 = 1, flag2 = 2;
    while (flag1 == 1 || flag2 == 1)
    {
        char c;
        flag1 = fscanf(fs1, "%c", &c);
        if (flag1 == 1) {
            fprintf(stdout, "%c", c);
        }
        flag2 = fscanf(fs2, "%c", &c);
        if (flag2 == 1) {
            fprintf(stdout, "%c", c);
        }
    }

    printf("\n");
    return 0; 
}

