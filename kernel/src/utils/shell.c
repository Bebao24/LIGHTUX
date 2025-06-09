#include "shell.h"
#include <stdbool.h>
#include <keyboard.h>
#include <string.h>
#include <console.h>
#include <ctype.h>
#include <memory.h>
#include <logging.h>
#include <fat32.h>
#include <heap.h>
#include <pci.h>

void launchShell()
{
    // Print a welcome message
    printf("Welcome to LightUX!\n");

    int inputLen = 0;
    char cmdString[256];
    char* cmdStringPtr = cmdString;

    int argc;
    char* argv[10];

    while (true)
    {
        // Print prompt
        printf("$ ");
        inputLen = 0;

        // Get the user input
        while (true)
        {
            char key = GetKey();

            if (key == '\r')
            {
                printf("\n");
                break;
            }
            if (key == '\b')
            {
                if (inputLen > 0)
                {
                    inputLen--;
                    cmdString[inputLen] = '\0';

                    printf("\b");
                }

                continue;
            }

            cmdString[inputLen++] = key;
            putc(key);
        }

        if (inputLen == 0)
        {
            // No input
            continue;
        }

        // NULL terminate
        cmdString[inputLen] = '\0';

        // Tokenize the input
        cmdStringPtr = cmdString;
        argc = 0;
        memset(argv, 0, sizeof(argv));

        while (*cmdStringPtr != '\0')
        {
            // Skip white spaces
            while (isspace(*cmdStringPtr))
            {
                *cmdStringPtr++ = '\0';
            }

            argv[argc++] = cmdStringPtr;

            // Go to the end of the string
            while (!isspace(*cmdStringPtr) && *cmdStringPtr != 0)
            {
                cmdStringPtr++;
            }
        }

        if (strcmp("echo", argv[0]) == 0)
        {
            for (int i = 1; i < argc; i++)
            {
                printf("%s ", argv[i]);
            }

            printf("\n");
        }
        else if (strcmp("clear", argv[0]) == 0)
        {
            console_clearScreen();
        }
        else if (strcmp("dir", argv[0]) == 0)
        {
            const char* path = "/";
            if (argc >= 2)
            {
                path = argv[1];
            }

            if (!FAT32_ListDirectory(path))
            {
                printf("Failed to list current directory!\n");
            }
        }
        else if (strcmp("read", argv[0]) == 0)
        {
            if (argc < 2)
            {
                printf("Please provide a file path!\n");
                continue;
            }

            const char* path = argv[1];
            FAT32_DirectoryEntry entry;
            if (!FAT32_TraversePath(path, &entry))
            {
                printf("%s not found!\n", path);
                continue;
            }

            char* buffer = malloc(entry.Size);
            if (!FAT32_ReadFile(entry, buffer))
            {
                printf("Failed to read file %s\n", path);
                free(buffer);
                continue;
            }

            // Display the file content
            for (uint32_t i = 0; i < entry.Size; i++)
            {
                putc(buffer[i]);
            }

            printf("\n");
        }
        else if (strcmp("lspci", argv[0]) == 0)
        {
            // Print all PCI devices' info
            PCI* browse = firstPCI;
            while (browse)
            {
                printf("Vendor ID: %x, Device ID: %x\n", browse->vendorID, browse->deviceID);
                browse = browse->next;
            }
        }
        else
        {
            printf("Command not found!\n");
        }
    }
}

