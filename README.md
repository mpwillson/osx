# Janet C wrappers for extra UNIX os functions

cfun_chroot:
    (osx/chroot path) - Set root directory to path.

cfun_chown:
    (osx/chown path user &opt group) - Change owner_id (and optionally
    group_id) of path to user and group.

cfun_setuid:
     (osx/setuid user) - Set effective user_id of process to that of user.

 cfun_setgid:
     (osx/setgid group) - Set effective group_id of process to that of group.,

To install locally:

``` shell
jpm -l install
```
