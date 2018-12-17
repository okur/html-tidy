#define FUSE_USE_VERSION 26

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/xattr.h>
#include <dirent.h>
#include <unistd.h>
#include <fuse.h>
#include <tidy.h>
#include <buffio.h>
#include <stdio.h>
#include <errno.h>


// Global to store our read-write path
char *rw_path;

// Global to size of buffer 
int size_buffer = 0;

void tidy(char * buf){
  TidyBuffer output = {0};
  TidyDoc Doc = tidyCreate();
  tidyOptSetBool(Doc, TidyXhtmlOut, yes);
  tidyOptSetInt(Doc, TidyIndentContent, yes);
  tidyOptSetInt(Doc, TidyIndentSpaces, 4);
  tidyParseString(Doc, buf); 
  tidyOptSetBool(Doc, TidyForceOutput, yes);  
  tidySaveBuffer(Doc, &output );
  strcpy(buf, output.bp);
  //printf("%s", buf);
  //printf("%d\n", output.size);
  tidyBufFree(&output);
  tidyRelease(Doc);
}
int get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot) return 0;
    if(!strcmp(dot, ".html")) return 1;
    else return 0;
}


static char* translate_path(const char* path)
{
	printf("\n\n\n %s\n", path);
    char *rPath= malloc(sizeof(char)*(strlen(path)+strlen(rw_path)+1));

    strcpy(rPath,rw_path);
    if (rPath[strlen(rPath)-1]=='/') {
        rPath[strlen(rPath)-1]='\0';
    }
    strcat(rPath,path);
    printf("rPath: %s\n\n\n\n ", rPath);
    return rPath;
}

//File Attributes
static int website_getattr(const char *path, struct stat *st_data)
{
    int res;
    char *upath=translate_path(path);
	
    res = lstat(upath, st_data);
    st_data->st_size += size_buffer;
    free(upath);
    if(res == -1) {
        return -errno;
    }
    return 0;
}

static int website_readlink(const char *path, char *buf, size_t size)
{
    int res;
    char *upath=translate_path(path);

    res = readlink(upath, buf, size - 1);
    free(upath);
    if(res == -1) {
        return -errno;
    }
    buf[res] = '\0';
    return 0;
}

static int website_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;
    int res;

    (void) offset;
	(void) fi;
    //Directory Listing
    char *upath=translate_path(path);

    dp = opendir(upath);
    free(upath);
    if(dp == NULL) {
        res = -errno;
        return res;
    }
    //Directory Info
    while((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int website_mknod(const char *path, mode_t mode, dev_t rdev)
{
    (void)path;
    (void)mode;
    (void)rdev;
    return -EROFS;
}

static int website_mkdir(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EROFS;
}

static int website_unlink(const char *path)
{
    (void)path;
    return -EROFS;
}

static int website_rmdir(const char *path)
{
    (void)path;
    return -EROFS;
}

static int website_symlink(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}

static int website_rename(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}

static int website_link(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}

static int website_chmod(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EROFS;

}

static int website_chown(const char *path, uid_t uid, gid_t gid)
{
    (void)path;
    (void)uid;
    (void)gid;
    return -EROFS;
}

static int website_truncate(const char *path, off_t size)
{
    (void)path;
    (void)size;
    return -EROFS;
}

static int website_utime(const char *path, struct utimbuf *buf)
{
    (void)path;
    (void)buf;
    return -EROFS;
}

static int website_open(const char *path, struct fuse_file_info *finfo)
{
    int res;
    int flags = finfo->flags;

    if ((flags & O_WRONLY) || (flags & O_RDWR) || (flags & O_CREAT) || (flags & O_EXCL) || (flags & O_TRUNC) || (flags & O_APPEND)) {
        return -EROFS;
    }

    char *upath=translate_path(path);

    res = open(upath, flags);

    free(upath);
    if(res == -1) {
        return -errno;
    }
    close(res);
    return 0;
}
//Reading from a file
static int website_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
    int fd;
    int res;
    (void)finfo;

    char *upath=translate_path(path);
    fd = open(upath, O_RDONLY);
    free(upath);
    if(fd == -1) {
        res = -errno;
        return res;
    }
	memset(buf, '\0', size_buffer);
    res = pread(fd, buf, size, offset);
    close(fd);

    if(get_extension(path))
    {
		char * temp = buf;
		tidy(temp);
		buf = temp;
		size_buffer  = strlen(buf);
		res = strlen(buf);	
	}
    if(res == -1) {
        res = -errno;
    }
    return res;
}

static int website_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
    (void)path;
    (void)buf;
    (void)size;
    (void)offset;
    (void)finfo;
    return -EROFS;
}

static int website_statfs(const char *path, struct statvfs *st_buf)
{
    int res;
    char *upath=translate_path(path);

    res = statvfs(upath, st_buf);
    free(upath);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int website_release(const char *path, struct fuse_file_info *finfo)
{
    (void) path;
    (void) finfo;
    return 0;
}

static int website_access(const char *path, int mode)
{
    int res;
    char *upath=translate_path(path);

    if (mode & W_OK)
        return -EROFS;

    res = access(upath, mode);
    free(upath);
    if (res == -1) {
        return -errno;
    }
    return res;
}

struct fuse_operations website_oper = {
    .getattr     = website_getattr,
    .readlink    = website_readlink,
    .readdir     = website_readdir,
    .mknod       = website_mknod,
    .mkdir       = website_mkdir,
    .symlink     = website_symlink,
    .unlink      = website_unlink,
    .rmdir       = website_rmdir,
    .rename      = website_rename,
    .link        = website_link,
    .chmod       = website_chmod,
    .chown       = website_chown,
    .truncate    = website_truncate,
    .utime       = website_utime,
    .open        = website_open,
    .read        = website_read,
    .write       = website_write,
    .statfs      = website_statfs,
    .release     = website_release,
    .access      = website_access,
};

static int website_parse_opt(void *data, const char *arg, int key,
                          struct fuse_args *outargs)
{
    (void)data;
    (void)outargs;
    if(key == FUSE_OPT_KEY_NONOPT)
    {
    	if (rw_path == 0)
        {
            rw_path = strdup(arg);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if(key == FUSE_OPT_KEY_OPT)
    	return 1;
    else
    	exit(1);
}

static struct fuse_opt website_opts[] = {
    FUSE_OPT_END
};

int main(int argc, char *argv[])
{
	
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    
    int res;

    res = fuse_opt_parse(&args, &rw_path, website_opts, website_parse_opt);
    if (res != 0)
    {
        printf("Invalid arguments\n");
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }
    if (rw_path == 0)
    {
        printf("missing arguments\n");
        fprintf(stderr, "Missing readwritepath\n");
        exit(1);
    }
    
    fuse_main(args.argc, args.argv, &website_oper, NULL);
    return 0;
}
