#include "log.h"
#include <pthread.h>

#define MAX_CALLBACKS 8

static pthread_mutex_t MUTEX_LOG;
static FILE* FP = NULL;

typedef struct
{
    log_fn_t fn;
    void *udata;
    int level;
} callback_t;

static struct
{
    void *mutex;
    lock_fn_t lock;
    int level;
    bool quiet;
    callback_t callbacks[MAX_CALLBACKS];
} L;

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};

static void stdout_callback(log_event_t *ev)
{
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
    fprintf(
        ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
        buf, level_colors[ev->level], level_strings[ev->level],
        ev->file, ev->line);
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

static void file_callback(log_event_t *ev)
{
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf(
        ev->udata, "%s %-5s %s:%d: ",
        buf, level_strings[ev->level], ev->file, ev->line);
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

static void lock(void)
{
    if (L.lock)
    {
        L.lock(true, L.mutex);
    }
}

static void unlock(void)
{
    if (L.lock)
    {
        L.lock(false, L.mutex);
    }
}

const char *log_level_string(int level)
{
    return level_strings[level];
}

void log_lock(bool lock, void* udata) {
    pthread_mutex_t *LOCK = (pthread_mutex_t*)(udata);
    if (lock)
        pthread_mutex_lock(LOCK);
    else
        pthread_mutex_unlock(LOCK);
}

void log_set_lock(lock_fn_t fn, void *mutex)
{
    L.lock = fn;
    L.mutex = mutex;
}

void log_set_level(int level)
{
    L.level = level;
}

void log_set_quiet(bool enable)
{
    L.quiet = enable;
}

int log_add_callback(log_fn_t fn, void *udata, int level)
{
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (!L.callbacks[i].fn)
        {
            L.callbacks[i] = (callback_t){fn, udata, level};
            return 0;
        }
    }
    return -1;
}

int log_add_fp(FILE *fp, int level)
{
    return log_add_callback(file_callback, fp, level);
}

static void init_event(log_event_t *ev, void *udata)
{
    if (!ev->time)
    {
        time_t t = time(NULL);
        ev->time = localtime(&t);
    }
    ev->udata = udata;
}

void log_init(const char* log_path)
{
    pthread_mutex_init(&MUTEX_LOG, NULL);
    log_set_lock(log_lock, &MUTEX_LOG);
    
    FP = fopen(log_path, "a+");
    log_set_quiet(true);
    log_set_level(LOG_ERROR);
    log_add_fp(FP, LOG_ERROR);
}

void log_destroy()
{
    fclose(FP);
    pthread_mutex_destroy(&MUTEX_LOG);
}

void log_log(int level, const char *file, int line, const char *fmt, ...)
{
    log_event_t ev = {
        .fmt = fmt,
        .file = file,
        .line = line,
        .level = level,
    };

    lock();

    if (!L.quiet && level >= L.level)
    {
        init_event(&ev, stderr);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++)
    {
        callback_t *cb = &L.callbacks[i];
        if (level >= cb->level)
        {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }

    unlock();
}