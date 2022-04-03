#include <fcntl.h>
#include <unistd.h>
int main()
{
    int flag1 = 1;
    int flag2 = 1;
    char c;
    // Ядро открывает два подключения к файлу алфавит.txt
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);
    // читать символ и записывать его поочередно из соединений fs1 и fd2
    while(flag1 == 1 && flag2 == 1)
    {
        flag1 = read(fd1, &c, 1);
        if (flag1 == 1)
        {
            write(1, &c, 1);
            flag2 = read(fd2, &c, 1);
            if (flag2 == 1)
                write(1, &c, 1);
        }
    }
    return 0;
}