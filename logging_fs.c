#define FUSE_USE_VERSION 26
#define _GNU_SOURCE

#include <limits.h>     
#include <time.h>       
#include <sys/time.h>   
#include <sys/types.h>  
#include <sys/stat.h>   
#include <unistd.h>     
#include <signal.h>     
#include <string.h>     
#include <stdlib.h>     
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


static char *target_dir = NULL;
static char *log_file = NULL;
static FILE *log_fp = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void get_full_path(char fpath[PATH_MAX], const char *path) {
    strcpy(fpath, target_dir);
    strncat(fpath, path, PATH_MAX - strlen(target_dir));
}


static void log_operation(const char *operation, const char *path, const char *details) {
    pthread_mutex_lock(&log_mutex);
    
    if (log_fp) {
        time_t now;
        struct tm *timeinfo;
        char timestamp[80];
        
        time(&now);
        timeinfo = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        fprintf(log_fp, "[%s] %s: %s", timestamp, operation, path);
        if (details) {
            fprintf(log_fp, " - %s", details);
        }
        fprintf(log_fp, "\n");
        fflush(log_fp);
    }
    
    pthread_mutex_unlock(&log_mutex);
}


static int logging_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    res = lstat(fpath, stbuf);
    
    if (res == -1) {
        log_operation("GETATTR_ERROR", path, strerror(errno));
        return -errno;
    }
    
    log_operation("GETATTR", path, "OK");
    return 0;
}


static int logging_access(const char *path, int mask) {
    int res;
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    res = access(fpath, mask);
    
    if (res == -1) {
        log_operation("ACCESS_ERROR", path, strerror(errno));
        return -errno;
    }
    
    log_operation("ACCESS", path, "OK");
    return 0;
}

static int logging_readlink(const char *path, char *buf, size_t size) {
    int res;
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    res = readlink(fpath, buf, size - 1);
    
    if (res == -1) {
        log_operation("READLINK_ERROR", path, strerror(errno));
        return -errno;
    }
    
    buf[res] = '\0';
    log_operation("READLINK", path, buf);
    return 0;
}


static int logging_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char fpath[PATH_MAX];
    
    (void) offset;
    (void) fi;
    
    get_full_path(fpath, path);
    dp = opendir(fpath);
    
    if (dp == NULL) {
        log_operation("READDIR_ERROR", path, strerror(errno));
        return -errno;
    }
    
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }
    
    closedir(dp);
    log_operation("READDIR", path, "OK");
    return 0;
}


static int logging_mkdir(const char *path, mode_t mode) {
    int res;
    char fpath[PATH_MAX];
    char mode_str[32];
    
    get_full_path(fpath, path);
    res = mkdir(fpath, mode);
    
    if (res == -1) {
        log_operation("MKDIR_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(mode_str, "mode=%o", mode);
    log_operation("MKDIR", path, mode_str);
    return 0;
}


static int logging_unlink(const char *path) {
    int res;
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    res = unlink(fpath);
    
    if (res == -1) {
        log_operation("UNLINK_ERROR", path, strerror(errno));
        return -errno;
    }
    
    log_operation("UNLINK", path, "OK");
    return 0;
}


static int logging_rmdir(const char *path) {
    int res;
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    res = rmdir(fpath);
    
    if (res == -1) {
        log_operation("RMDIR_ERROR", path, strerror(errno));
        return -errno;
    }
    
    log_operation("RMDIR", path, "OK");
    return 0;
}

static int logging_rename(const char *from, const char *to) {
    int res;
    char fpath_from[PATH_MAX];
    char fpath_to[PATH_MAX];
    char rename_info[PATH_MAX * 2 + 10];
    
    get_full_path(fpath_from, from);
    get_full_path(fpath_to, to);
    res = rename(fpath_from, fpath_to);
    
    if (res == -1) {
        log_operation("RENAME_ERROR", from, strerror(errno));
        return -errno;
    }
    
    sprintf(rename_info, "to: %s", to);
    log_operation("RENAME", from, rename_info);
    return 0;
}

static int logging_chmod(const char *path, mode_t mode) {
    int res;
    char fpath[PATH_MAX];
    char mode_str[32];
    
    get_full_path(fpath, path);
    res = chmod(fpath, mode);
    
    if (res == -1) {
        log_operation("CHMOD_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(mode_str, "mode=%o", mode);
    log_operation("CHMOD", path, mode_str);
    return 0;
}

static int logging_chown(const char *path, uid_t uid, gid_t gid) {
    int res;
    char fpath[PATH_MAX];
    char owner_info[64];
    
    get_full_path(fpath, path);
    res = lchown(fpath, uid, gid);
    
    if (res == -1) {
        log_operation("CHOWN_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(owner_info, "uid=%d gid=%d", uid, gid);
    log_operation("CHOWN", path, owner_info);
    return 0;
}

static int logging_truncate(const char *path, off_t size) {
    int res;
    char fpath[PATH_MAX];
    char size_str[32];
    
    get_full_path(fpath, path);
    res = truncate(fpath, size);
    
    if (res == -1) {
        log_operation("TRUNCATE_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(size_str, "size=%ld", size);
    log_operation("TRUNCATE", path, size_str);
    return 0;
}

static int logging_utimens(const char *path, const struct timespec ts[2]) {
    int res;
    struct timeval tv[2];
    char fpath[PATH_MAX];
    
    get_full_path(fpath, path);
    
    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;
    
    res = utimes(fpath, tv);
    
    if (res == -1) {
        log_operation("UTIMENS_ERROR", path, strerror(errno));
        return -errno;
    }
    
    log_operation("UTIMENS", path, "OK");
    return 0;
}

static int logging_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char fpath[PATH_MAX];
    char flags_str[64];
    
    get_full_path(fpath, path);
    res = open(fpath, fi->flags);
    
    if (res == -1) {
        log_operation("OPEN_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(flags_str, "flags=%d fd=%d", fi->flags, res);
    log_operation("OPEN", path, flags_str);
    
    close(res);
    return 0;
}

static int logging_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi) {
    FILE *fp;
    int res;
    char fpath[PATH_MAX];
    char read_info[64];
    
    (void) fi;
    get_full_path(fpath, path);
    fp = fopen(fpath, "r");
    
    if (fp == NULL) {
        log_operation("READ_ERROR", path, strerror(errno));
        return -errno;
    }
    
    fseek(fp, offset, SEEK_SET);
    res = fread(buf, 1, size, fp);
    
    if (res == -1) {
        res = -errno;
        log_operation("READ_ERROR", path, strerror(errno));
    } else {
        sprintf(read_info, "bytes=%d offset=%ld", res, offset);
        log_operation("READ", path, read_info);
    }
    
    fclose(fp);
    return res;
}

static int logging_write(const char *path, const char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi) {
    FILE *fp;
    int res;
    char fpath[PATH_MAX];
    char write_info[64];
    
    (void) fi;
    get_full_path(fpath, path);
    fp = fopen(fpath, "r+");
    
    if (fp == NULL) {
        log_operation("WRITE_ERROR", path, strerror(errno));
        return -errno;
    }
    
    fseek(fp, offset, SEEK_SET);
    res = fwrite(buf, 1, size, fp);
    
    if (res == -1) {
        res = -errno;
        log_operation("WRITE_ERROR", path, strerror(errno));
    } else {
        sprintf(write_info, "bytes=%d offset=%ld", res, offset);
        log_operation("WRITE", path, write_info);
    }
    
    fclose(fp);
    return res;
}

static int logging_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    int res;
    char fpath[PATH_MAX];
    char create_info[64];
    
    get_full_path(fpath, path);
    res = creat(fpath, mode);
    
    if (res == -1) {
        log_operation("CREATE_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(create_info, "mode=%o fd=%d", mode, res);
    log_operation("CREATE", path, create_info);
    
    fi->fh = res;
    return 0;
}


static struct fuse_operations logging_oper = {
    .getattr    = logging_getattr,
    .access     = logging_access,
    .readlink   = logging_readlink,
    .readdir    = logging_readdir,
    .mkdir      = logging_mkdir,
    .unlink     = logging_unlink,
    .rmdir      = logging_rmdir,
    .rename     = logging_rename,
    .chmod      = logging_chmod,
    .chown      = logging_chown,
    .truncate   = logging_truncate,
    .utimens    = logging_utimens,
    .open       = logging_open,
    .read       = logging_read,
    .write      = logging_write,
    .create     = logging_create,
};

static void cleanup() {
    if (log_fp) {
        log_operation("UNMOUNT", "/", "File system unmounted");
        fclose(log_fp);
        log_fp = NULL;
    }
    
    if (target_dir) {
        free(target_dir);
        target_dir = NULL;
    }
    
    if (log_file) {
        free(log_file);
        log_file = NULL;
    }
}

static void signal_handler(int sig) {
    printf("\nReceived signal %d, cleaning up...\n", sig);
    cleanup();
    exit(0);
}

int main(int argc, char *argv[]) {
    int ret;
    
    
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <target_dir> <log_file> <mount_point> [fuse_options...]\n", argv[0]);
        fprintf(stderr, "Example: %s /home/user/data /tmp/fs.log /mnt/logging_fs -f\n", argv[0]);
        return 1;
    }
    
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    
    target_dir = realpath(argv[1], NULL);
    if (!target_dir) {
        perror("realpath");
        return 1;
    }
    

    struct stat st;
    if (stat(target_dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: Target directory '%s' tidak ada atau bukan direktori\n", target_dir);
        free(target_dir);
        return 1;
    }
    
  
    log_file = strdup(argv[2]);
    if (!log_file) {
        perror("strdup");
        free(target_dir);
        return 1;
    }
    
  
    log_fp = fopen(log_file, "a");
    if (!log_fp) {
        perror("fopen log file");
        free(target_dir);
        free(log_file);
        return 1;
    }
    
    printf("Logging FUSE File System\n");
    printf("Target directory: %s\n", target_dir);
    printf("Log file: %s\n", log_file);
    printf("Mount point: %s\n", argv[3]);
    

    log_operation("MOUNT", "/", target_dir);
    

    char **fuse_argv = malloc((argc - 1) * sizeof(char*));
    fuse_argv[0] = argv[0];  // program name
    fuse_argv[1] = argv[3];  // mount point
    

    for (int i = 4; i < argc; i++) {
        fuse_argv[i - 2] = argv[i];
    }
    

    ret = fuse_main(argc - 2, fuse_argv, &logging_oper, NULL);
    

    free(fuse_argv);
    cleanup();
    
    return ret;
}