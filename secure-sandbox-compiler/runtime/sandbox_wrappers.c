#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int sandbox_system_wrapper()
{
    printf("[Sandbox] system() call blocked by policy.\n");
    return -1;
}

int sandbox_exec_wrapper()
{
    printf("[Sandbox] exec() call blocked by policy.\n");
    return -1;
}

int sandbox_fork_wrapper()
{
    printf("[Sandbox] fork() call blocked by policy.\n");
    return -1;
}

// sanitize file path - block ../ traversal and sensitive paths
const char *sandbox_sanitize_path(const char *path)
{
    if (!path) return NULL;

    // block parent directory traversal
    if (strstr(path, "../") || strstr(path, "..\\"))
    {
        printf("[Sandbox] Path blocked: directory traversal detected: %s\n", path);
        return NULL;
    }

    // block sensitive system paths
    const char *blocked[] = {
        "/etc/", "/root/", "/proc/",
        "/sys/", "/boot/", "/dev/", NULL
    };

    for (int i = 0; blocked[i]; i++)
    {
        if (strncmp(path, blocked[i], strlen(blocked[i])) == 0)
        {
            printf("[Sandbox] Path blocked: sensitive path: %s\n", path);
            return NULL;
        }
    }

    return path;
}