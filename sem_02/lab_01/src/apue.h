#ifndef _APUE_H
#define _APUE_H

#if defined(SOLARIS)
#define _XOPEN_SOURCE 500 /* Единая спецификация UNIX, версия 2 solaris 9  */
#define CMSG_LEN(x) _CMSG_DATA_ALIGN(sizeof(struct cmsghdr)+(x))
#elif !defined(BSD)
#define _XOPEN_SOURCE 600 /* Единая спецификация UNIX, Версия 3 */
#endif

#include <sys/types.h> /* Некоторые системы все еще требуют этого */
#include <sys/stat.h>
#include <sys/termios.h> /* для winsize */
#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif
#include <stdio.h> /* для convenience */
#include <stdlib.h> /* для convenience */
#include <stddef.h> /* для offsetof */
#include <string.h> /* для convenience */
#include <unistd.h> /* для convenience */
#include <signal.h> /* для SIG_ERR */
#define MAXLINE 4096 /* максимальная длина строки */

/*
* Разрешения на доступ к файлом по умолчанию для новых файлов.
*/
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
* Разрешения по умолчанию для новых каталогов.
*/
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

typedef void Sigfunc(int); /* для обработчиков сигналов */

#if defined(SIG_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((Sigfunc *)-1)
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

/*
* Прототипы для собственных функций.
*/
char *path_alloc(int *); /* {функция path_alloc} */
long open_max(void); /* {функция open_max} */
void clr_fl(int, int); /* {функция clr_fl} */
void set_fl(int, int); /* {функция set_fl} */
void pr_exit(int); /* {функция pr_exit} */
void pr_mask(const char *); /* {функция pr_mask} */
Sigfunc *signal_intr(int, Sigfunc *); /* {Progsignal_intr_function} */

int tty_cbreak(int); /* {функция tty_cbreak} */
int tty_raw(int); /* {функция tty_raw} */
int tty_reset(int); /* {функция tty_reset} */
void tty_atexit(void); /* {функция tty_atexit} */
#ifdef ECHO /* Только если <termios.h> был включен */
struct termios *tty_termios(void); /* {функция tty_termios} */
#endif

void sleep_us(unsigned int); /* {функция sleep_us} */
ssize_t readn(int, void *, size_t); /* {функция readn} */
ssize_t writen(int, const void *, size_t); /* {функция writen} */
void daemonize(const char *); /* {функция daemoninit} */

int s_pipe(int *); /* {функция s_pipe} */
/* {функция recv_fd} */
int recv_fd(int, ssize_t (*func)(int, const void *, size_t)); 
int send_fd(int, int); /* {функция send_fd} */

int send_err(int, int, const char *); /* {функция send_err} */
int serv_listen(const char *); /* {функция serv_listen} */
int serv_accept(int, uid_t *); /* {функция serv_accept} */
int cli_conn(const char *); /* {функция cli_conn} */
int buf_args(char *, int (*func)(int, char **)); /* {функция buf_args} */

int ptym_open(char *, int); /* {функция ptym_open} */
int ptys_open(char *); /* {функция ptys_open} */
#ifdef TIOCGWINSZ
pid_t pty_fork(int *, char *, int, const struct termios *, const struct winsize *); 
/* {функция pty_fork} */
#endif
int lock_reg(int, int, int, off_t, int, off_t); /* {функция lock_reg} */
#define read_lock(fd, offset, whence, len) \
lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

pid_t lock_test(int, int, off_t, int, off_t); /* {функция lock_test} */
#define is_read_lockable(fd, offset, whence, len) \
(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

void err_dump(const char *, ...); /* {misc_source} */
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_exit(int, const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

void log_msg(const char *, ...); /* {misc_source} */
void log_open(const char *, int, int);
void log_quit(const char *, ...);
void log_ret(const char *, ...);
void log_sys(const char *, ...);

void TELL_WAIT(void); /* предок/потомок из {Sec race_conditions}
*/
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);

#endif /* _APUE_H */