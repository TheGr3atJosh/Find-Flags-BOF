#include <windows.h>
#include "bofdefs.h"
#include "base.c"
#include "queue.c"

int is_target_file(const char *filename) {
    if (MSVCRT$_stricmp(filename, "user.txt") == 0) return 1;
    if (MSVCRT$_stricmp(filename, "root.txt") == 0) return 1;
    if (MSVCRT$_stricmp(filename, "local.txt") == 0) return 1;
    if (MSVCRT$_stricmp(filename, "proof.txt") == 0) return 1;
    if (MSVCRT$_stricmp(filename, "secret.txt") == 0) return 1;
    if (MSVCRT$_stricmp(filename, "flag.txt") == 0) return 1;
    return 0;
}

int is_excluded_dir(const char *path, const char *dirname) {
    if (MSVCRT$strcmp(path, "C:\\*") == 0 || MSVCRT$strcmp(path, "c:\\*") == 0) {
        if (MSVCRT$_stricmp(dirname, "Windows") == 0) return 1;
        if (MSVCRT$_stricmp(dirname, "Program Files") == 0) return 1;
        if (MSVCRT$_stricmp(dirname, "Program Files (x86)") == 0) return 1;
    }
    return 0;
}

void findFlags() {
    WIN32_FIND_DATA fd = {0};
    HANDLE hand = NULL;
    int found = 0;
    Pqueue dirQueue = queueInit();
    char * curitem;
    char * nextPath;
    
    char * initialPath = intAlloc(6);
    MSVCRT$strcpy(initialPath, "C:\\*");
    dirQueue->push(dirQueue, initialPath);

    while((curitem = dirQueue->pop(dirQueue)) != NULL) {
        hand = KERNEL32$FindFirstFileA(curitem, &fd);
        if (hand == INVALID_HANDLE_VALUE) {
            intFree(curitem);
            continue;
        }

        do {
            if (MSVCRT$strcmp(fd.cFileName, ".") == 0 || MSVCRT$strcmp(fd.cFileName, "..") == 0) {
                continue;
            }

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    if (!is_excluded_dir(curitem, fd.cFileName)) {
                        int pathlen = MSVCRT$strlen(curitem); 
                        int dirlen = MSVCRT$strlen(fd.cFileName);
                        nextPath = intAlloc(pathlen + dirlen + 3);
                        MSVCRT$memcpy(nextPath, curitem, pathlen - 1);
                        nextPath[pathlen-1] = '\0';
                        MSVCRT$strcat(nextPath, fd.cFileName);
                        MSVCRT$strcat(nextPath, "\\*");
                        dirQueue->push(dirQueue, nextPath);
                    }
                }
            } else {
                if (is_target_file(fd.cFileName)) {
                    int pathlen = MSVCRT$strlen(curitem); 
                    char * fullFilePath = intAlloc(pathlen + MSVCRT$strlen(fd.cFileName) + 1);
                    MSVCRT$memcpy(fullFilePath, curitem, pathlen - 1);
                    fullFilePath[pathlen-1] = '\0';
                    MSVCRT$strcat(fullFilePath, fd.cFileName);
                    internal_printf("[+] Found flag: %s\n", fullFilePath);

                    HANDLE hFile = KERNEL32$CreateFileA(fullFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile != INVALID_HANDLE_VALUE) {
                        char buf[1024];
                        DWORD bytesRead = 0;
                        if (KERNEL32$ReadFile(hFile, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0) {
                            buf[bytesRead] = '\0';
                            internal_printf("%s\n", buf);
                        }
                        KERNEL32$CloseHandle(hFile);
                    }

                    found++;
                    intFree(fullFilePath);
                }
            }
        } while(KERNEL32$FindNextFileA(hand, &fd));
        
        KERNEL32$FindClose(hand);
        intFree(curitem);
    }
    dirQueue->free(dirQueue);

    if (found == 0) {
        internal_printf("[-] No flags found.\n");
    }
}

#ifdef BOF
VOID go(
    IN PCHAR Buffer,
    IN ULONG Length
)
{
    if(!bofstart())
    {
        return;
    }


    findFlags();
    printoutput(TRUE);
};
#endif
