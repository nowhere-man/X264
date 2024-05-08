#include "log.h"
#include "cJSON.h"
#include <string.h>
#include <pthread.h>

static pthread_mutex_t MUTEX_LOG;
static FILE* FP = NULL;

typedef struct
{
    va_list ap;
    const char *fmt;
    const char *file;
    struct tm *time;
    void *udata;
    int line;
    int level;
} log_event_t;

typedef void (*log_fn_t)(log_event_t *ev);
typedef void (*lock_fn_t)(bool lock, void *udata);

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
    callback_t callback;
} L;

typedef struct {
    bool log2file;
    char* logfile_path;
    int logfile_level;
    bool log2stdout;
    int logstdout_level;
} log_config_t;


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
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
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

int get_loglevel_idx(const char* level_string) {
    for (int i = 0; i < sizeof(level_strings); i++) {
        if (strcmp(level_string, level_strings[i]) == 0) {
            return i;
        }
    }
    return LOG_ERROR;
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
    if (!L.callback.fn)
    {
        L.callback = (callback_t){fn, udata, level};
        return 0;
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

void load_json_config(log_config_t* config)
{
    const char* json_path = "./config.json";
    FILE* json_fp = fopen(json_path, "r+");
    if (json_fp == NULL) {
        printf("WARN:open config file failed.");
        return;
    }

    char json_str[256] = {0};
    size_t read_size = fread(json_str, sizeof(char), sizeof(json_str), json_fp);
    if (read_size == 0) {
        fclose(json_fp);
        printf("ERROR:read config file failed.");
        return;
    }
    fclose(json_fp);

    cJSON* x264_config = cJSON_Parse(json_str);
    if (x264_config == NULL) {
        printf("WARN:parse x264 config failed.");
        return;
    }
    cJSON* log_config = cJSON_GetObjectItem(x264_config, "log");
    if (log_config == NULL) {
        printf("WARN:parse log config failed.");
        return;
    }

    cJSON* log2file = cJSON_GetObjectItem(log_config, "log2file");
    cJSON* logfile_path = cJSON_GetObjectItem(log_config, "logfile_path");
    cJSON* logfile_level = cJSON_GetObjectItem(log_config, "logfile_level");
    cJSON* log2stdout = cJSON_GetObjectItem(log_config, "log2stdout");
    cJSON* logstdout_level = cJSON_GetObjectItem(log_config, "logstdout_level");

    if (log2file != NULL)
         config->log2file = log2file->valueint;
    if (logfile_path != NULL)
        config->logfile_path = logfile_path->valuestring;
    if (logfile_level != NULL)
        config->logfile_level = get_loglevel_idx(logfile_level->valuestring);
    if (log2stdout != NULL)
        config->log2stdout = log2stdout->valueint;
    if (logstdout_level != NULL)
         config->logstdout_level = get_loglevel_idx(logstdout_level->valuestring);
}

void log_init()
{
    log_config_t config = {
        .log2file = true,
        .logfile_path = "x264_log.log",
        .logfile_level = LOG_ERROR,
        .log2stdout = true,
        .logstdout_level = LOG_ERROR
    };

    load_json_config(&config);

    pthread_mutex_init(&MUTEX_LOG, NULL);
    log_set_lock(log_lock, &MUTEX_LOG);

    if (config.log2file) {
        FP = fopen(config.logfile_path, "w+");
        log_add_fp(FP, config.logfile_level);
    }

    if (config.log2stdout) {
        log_set_quiet(false);
        log_set_level(config.logstdout_level);
    } else {
        log_set_quiet(true);
    }
}

void log_destroy()
{
    if (FP != NULL)
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

    callback_t *cb = &L.callback;
    if ((L.quiet || level < L.level) && (cb == NULL || level < cb->level)) {
        return;
    }

    lock();

    if (!L.quiet && level >= L.level)
    {
        init_event(&ev, stderr);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    if (cb && level >= cb->level)
    {
        init_event(&ev, cb->udata);
        va_start(ev.ap, fmt);
        cb->fn(&ev);
        va_end(ev.ap);
    }

    unlock();
}