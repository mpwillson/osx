/*
 *  NAME
 *    osx - extra os functions
 *
 *  SYNOPSIS
 *    chroot (osx/chroot path)
 *    chown  (osx/chown path user &opt group)
 *    setuid (osc/setuid user)
 *    setgid (osx/setgid group)
 *    hostname (osx/hostname)
 *
 *  DESCRIPTION
 *    Provides additional os routines for daemons that need to change
 *    root directory, change file owner/group and set the effective uid
 *    or gid of the running process.
 *
 *  NOTES
 *    Defined for OpenBSD, but may be useful elsewhere.
 *
 */

#include <janet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

/* jpm build complains that chroot is not defined */
extern int chroot(const char*);
extern int gethostname(char *name, size_t len);

char*
errmsg(void) {
    return strerror(errno);
}

char* buf_or_str(Janet* argv, int n) {
    char* str;

    if (janet_checktype(argv[n], JANET_BUFFER)) {
        JanetBuffer* b = janet_getbuffer(argv, n);
        str = (char *) janet_smalloc(b->count+1);
        if (!str) janet_panic("unable to allocate memory");
        strcpy(str, b->data);
        str[b->count] = '\0';
    }
    else if (janet_checktype(argv[n], JANET_STRING)) {
        str = (char *) janet_getcstring(argv, n);
    }
    else {
        janet_panicf("arg %d: expected (buffer|string)", n);
    }
    return str;
}

static Janet
cfun_chroot(int32_t argc, Janet* argv) {
    const char* path;
    int result;

    janet_fixarity(argc, 1);
    path = buf_or_str(argv, 0);
    result = chroot(path);
    if (result != 0) {
        janet_panicf("chroot: %s", errmsg());
    }
    return janet_wrap_nil();
}

uid_t
get_uid(const char* user)
{
    struct passwd* pwd;

    pwd = getpwnam(user);
    if (pwd)
        return pwd->pw_uid;
    return -1;
}

gid_t
get_gid(const char* group)
{
    struct group* g;

    g = getgrnam(group);
    if (g)
        return g->gr_gid;
    return -1;
}

static Janet
cfun_chown(int32_t argc, Janet* argv)
{
    const char* obj = NULL;
    const char* user = NULL;
    const char* group = NULL;
    uid_t uid = -1;
    gid_t gid = -1;

    janet_arity(argc, 2, 3);
    obj = buf_or_str(argv, 0);
    user = buf_or_str(argv, 1);
    uid = get_uid(user);
    if (uid == -1) janet_panicf("chown: no such user: %s", user);
    if (argc == 3) {
        group = buf_or_str(argv, 2);
        if (gid == -1) janet_panicf("chown: no such group: %s", group);
    }
    if (chown(obj, uid, gid) != 0) janet_panicf("chown: %s", errmsg());
    return janet_wrap_nil();
}

static Janet
cfun_setuid(int32_t argc, Janet* argv)
{
    const char* user = NULL;
    uid_t uid = -1;

    janet_fixarity(argc, 1);
    user = janet_getcstring(argv, 0);
    uid = get_uid(user);
    if (uid == -1) janet_panicf("setuid: no such user: %s", user);
    if (setuid(uid) != 0) janet_panicf("setuid: %s", errmsg());
    return janet_wrap_nil();
}

static Janet
cfun_setgid(int32_t argc, Janet* argv)
{
    const char* group = NULL;
    gid_t gid = -1;

    janet_fixarity(argc, 1);
    group = janet_getcstring(argv, 0);
    gid = get_gid(group);
    if (gid == -1) janet_panicf("setgid: no such group: %s", group);
    if (setgid(gid) != 0) janet_panicf("setgid: %s", errmsg());
    return janet_wrap_nil();
}

enum {
    BUFSIZE = 64
};

static Janet
cfun_hostname(int32_t argc, Janet* argv)
{
    char hn[BUFSIZE+1];
    char *dot;
    size_t len = BUFSIZE;
    int status;

    janet_fixarity(argc, 0);
    status = gethostname(hn, len);
    if (status == -1) {
        janet_panicf("hostname: %s", errmsg());
    }
    hn[BUFSIZE] = '\0';
    if ((dot = strchr(hn, '.')) != NULL) *dot = '\0';
    len = strlen(hn);
    uint8_t *host = janet_string_begin(len);
    memcpy(host, (const uint8_t *) hn, len);
    return janet_wrap_string(host);
}

static JanetReg
cfuns[] = {
    {"chroot", cfun_chroot,
     "(osx/chroot path)\nSet root directory to _path_."},
    {"chown", cfun_chown,
     "(osx/chown path user &opt group)\nChange owner\\_id "
     "(and optionally group\\_id) of path to _user_ and _group_."},
    {"setuid", cfun_setuid,
     "(osx/setuid user)\nSet effective user\\_id of process to that of _user_."},
    {"setgid", cfun_setgid,
     "(osx/setgid group)\nSet effective group\\_id of process to that of _group_."},
    {"hostname", cfun_hostname, "(osx/hostname)\nReturn host name"},
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable* env) {
    janet_cfuns(env, "osx", cfuns);
}
