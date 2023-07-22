# Janet C wrappers for extra UNIX os functions

cfun_chroot:
    (osx/chroot path) - Set root directory as path.

cfun_chown:
    (osx/chown path user &opt group) - Change owner_id (and optionally
    group_id) of path to user and group.

cfun_setuid:
     (osx/setuid user) - Set effective user_id of process to that of user.

 cfun_setgid:
     (osx/setgid group) - Set effective group_id of process to that of group.

cfun_hostname:
     (osx/hostname) - Returns name of host as string.

To install locally (for testing):

``` shell
jpm -l install
```

To install globally:

```shell
sudo jpm install
```

