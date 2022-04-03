#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "apue.h"
#include <pthread.h>
#include <time.h>
#include <syslog.h>
// #include <sys/stat.h>
#include <sys/resource.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(void)
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    
    if (fd < 0)
    {
        syslog(LOG_ERR, "невозможно открыть %s: %s",LOCKFILE, strerror(errno));
        exit(1);
    }

    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "невозможно установить блокировку на Xs");
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return(0);
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    /*
    * 1. Сбросить маску режима создания файла.
    */
    umask(0);

    /*
    * Получить максимально возможный номер дескриптора файла.
    */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        perror("невозможно получить максимальный номер дескриптора ");
    /*
    * 2. Стать лидером нового сеанса, чтобы утратить управляющий
    терминал.
    */
    if ((pid = fork()) < 0)
        perror("ошибка вызова функции fork");
    else if (pid != 0) /* родительский процесс */
        exit(0);

    setsid();
    /*
    * Обеспечить невозможность обретения управляющего терминала
    в будущем.
    */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        perror("невозможно игнорировать сигнал SIGHUP");

    /*
    * Назначить корневой каталог текущим рабочим каталогом,
    * чтобы впоследствии можно было отмонтировать файловую
    систему.
    */
    if (chdir("/") < 0)
        perror("невозможно сделать текущим рабочим каталогом ");
    /*
    * Закрыть все открытые файловые дескрипторы.
    */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    /*
    * Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
    */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    /*
    * Инициализировать файл журнала.
    */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d",fd0, fd1, fd2);
        exit(1);
    }
}

int main()
{
    daemonize("Demon");
    /*
    * Убедиться, что ранее не была запущена другая копия демона.
    */
    if (already_running())
    {
        syslog(LOG_ERR, "Демон уже запущен");
        exit(1);
    }
    syslog(LOG_WARNING, "Проверка пройдена!");
    while(1)
    {
        long int t = time(NULL);
        syslog(LOG_INFO, "Демон %s", ctime (&t));
        sleep(3);
    }
}
