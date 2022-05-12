#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <ctype.h>


// This structure contains information about each directory entry and helps keep tracking the file/directory information
struct __attribute__((__packed__)) DirectoryEntry
{
  char DIR_Name[11];
  uint8_t DIR_Attr;
  uint8_t Unused1[8];
  uint16_t DIR_FirstClusterHigh;
  uint8_t Unused2[4];
  uint16_t DIR_FirstClusterLow;
  uint32_t DIR_FileSize;
};

uint8_t Attr;
char dir_holder[11];

uint16_t BPB_BytesPerSec; // Offset 11
uint8_t BPB_SecPerClus;   // Offset 13
uint16_t BPB_RsvdSecCnt;  // Offset 14
uint8_t BPB_NumFATs;      // Offset 16
uint32_t BPB_FATSz32;     // Offset 36

// struct used to save all the file information.
struct DirectoryEntry dir[16];

FILE *fp;
FILE *ofp;
#define MAX_NUM_ARGUMENTS 11

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

int LBAToOffset(int32_t sector)
{
  return ((sector - 2) * BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt) + (BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec);
}

int16_t NextLB(uint32_t sector)
{
  uint32_t FATAdress = (BPB_BytesPerSec * BPB_RsvdSecCnt) + (sector * 4);
  int16_t val;
  fseek(fp, FATAdress, SEEK_SET);
  fread(&val, 2, 1, fp);

  return val;
}

// This function ignores and fixes the weird spacing between the desired directory and the saved one
int compare(char *_input, char *_DIR_name)
{
  char IMG_Name[11];

  char input[11];
  char expanded_name[12];

  strncpy(IMG_Name, _DIR_name, 11);
  strncpy(input, _input, 11);

  if (strncmp(_input, "..",2) != 0)
  {
   
    memset(expanded_name, ' ', 12);

    char *token = strtok(input, ".");

    strncpy(expanded_name, token, strlen(token));

    token = strtok(NULL, ".");

    if (token)
    {
      strncpy((char *)(expanded_name + 8), token, strlen(token));
    }

    expanded_name[11] = '\0';

    // make the name of the directory not case sensitive
    int i;
    for (i = 0; i < 11; i++)
    {
      expanded_name[i] = toupper(expanded_name[i]);
    }
  }

    // if we find a match
  else
  {
    strncpy(expanded_name, "..", 2);
    expanded_name[3] = '\0';

    if (strncmp(expanded_name, IMG_Name, 2) == 0)
    {
      return 0;
    }
    return 1;
  }
  
  if (strncmp(expanded_name, IMG_Name, 11) == 0)
  {
    return 0;
  }

  return 1;
}

int main()
{
  uint8_t BUFFER[512];

  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  char name[12]; // Null Terminate the file name

  int i; // Iterable variable
  int file_status;

  while (1)
  {
    // Print out the mfs prompt
    printf("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    //\TODO Remove this code and replace with your FAT32 functionality
    if (token[0] == '\0')
    {
      continue;
    }

    // prints the info regarding the files
    if (!strcmp(token[0], "info"))
    {
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else
      {
        printf("BPB_BytesPerSec:\t%d\t%x\n", BPB_BytesPerSec, BPB_BytesPerSec);
        printf("BPB_SecPerClus:\t\t%d\t%x\n", BPB_SecPerClus, BPB_SecPerClus);
        printf("BPB_RsvdSecCnt:\t\t%d\t%x\n", BPB_RsvdSecCnt, BPB_RsvdSecCnt);
        printf("BPB_RsvdSecCnt:\t\t%d\t%x\n", BPB_RsvdSecCnt, BPB_RsvdSecCnt);
        printf("BPB_NumFATs:\t\t%d\t%x\n", BPB_NumFATs, BPB_NumFATs);
        printf("BPB_FATSz32:\t\t%d\t%x\n", BPB_FATSz32, BPB_FATSz32);
      }
    }

    // opens the file
    else if (!strcmp(token[0], "open"))
    {
      if (fp == NULL)
      {
        file_status = 1; 
        fp = fopen(token[1], "r");

        if(fp == NULL)
        {
          printf("Error: File system image not found.\n");
        }

        else
        {
          // read the BPB varaibles

          fseek(fp, 11, SEEK_SET);
          fread(&BPB_BytesPerSec, 2, 1, fp);

          fseek(fp, 13, SEEK_SET);
          fread(&BPB_SecPerClus, 1, 1, fp);

          fseek(fp, 14, SEEK_SET);
          fread(&BPB_RsvdSecCnt, 2, 1, fp);

          fseek(fp, 16, SEEK_SET);
          fread(&BPB_NumFATs, 1, 1, fp);

          fseek(fp, 36, SEEK_SET);
          fread(&BPB_FATSz32, 4, 1, fp);

          // Placing the file pointer at the root
          fseek(fp, BPB_NumFATs * (BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec), SEEK_SET);
          fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
        }
      }

      else
      {
        printf("Error: File system image already open\n");
      }
    }


    // close the file 
    else if (!strcmp(token[0], "close"))
    {
      if (fp == NULL && !file_status)
      {
        printf("Error: File system not open.\n");
      }
      else
      {
        file_status = fclose(fp);
        fp = NULL;
      }
    }

    // stat prints the information regarding the files
    else if (!strcmp(token[0], "stat"))
    {
      // if the directory doesn't exist
      int dirExist = 0;
      
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else if(token[1] == NULL)
      {
        printf("Error: No filename or directory name specified.\n");
      }

      else
      {
        // check for all directoryies from the image
        for (i = 0; i < 16; i++)
        {
          if (!compare(token[1],dir[i].DIR_Name))
          {
            dirExist = 1;
            break;
          }
        }
        // if there exists, print the stat of the file   
        if(dirExist)
        {
          printf("File Size: %d\n", dir[i].DIR_FileSize);
          printf("First Cluster Low: %d\n", dir[i].DIR_FirstClusterLow);
          printf("DIR_ATTR: %d\n", dir[i].DIR_Attr);
          printf("First Cluster High: %d\n",dir[i].DIR_FirstClusterHigh);
        }
        else
        {
          printf("Error: No filename or directory name specified.\n");
        }
      }
    }

    // lists all the directories
    else if (!strcmp(token[0], "ls"))
    {

      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else 
      {
        for (i = 0; i < 16; i++)
        {
          // Checks if the file is read only OR it's a sub-directory OR the archive flag.
          // AND The filename should not be used, or deleted.
          if (dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20 )
          {
            memcpy(name, dir[i].DIR_Name, 11);
            name[11] = '\0';

            printf("Filename: %s\n", name); // Reading Directory Name wrong.
          }
        }
      }
    }

    // not sure if we are going to put
    else if (!strcmp(token[0], "put"))
    {
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }  
    }

    // will copy the file to the local directory 
    else if (!strcmp(token[0], "get"))
    {
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else
      {
        for (i = 0; i < 16; i++)
        {
          if (!compare(token[1], dir[i].DIR_Name))
          {
            //get the offset to the first low cluster to fseek to the directory
            int offset = LBAToOffset(dir[i].DIR_FirstClusterLow);
            int cluster =dir[i].DIR_FirstClusterLow; 
            fseek(fp, offset, SEEK_SET);

            // opening a new file to copy into it
            FILE *output_file = fopen(token[1], "w");

            int size = dir[i].DIR_FileSize;
            while (size >= BPB_BytesPerSec)
            {
              fread(BUFFER, 512, 1, fp);
              fwrite(BUFFER, 512, 1, output_file);

              size = size - BPB_BytesPerSec;

              // keep updating the cluster
              cluster = NextLB(cluster);

              // if we reach out of bounds, reset the offset and fseek to it
              if (cluster > -1)
              {
                offset = LBAToOffset(cluster);
                fseek(fp, offset, SEEK_SET);
              }
            }
            // read and output to the new file the copied version
            if (size > 0)
            {
              fread(BUFFER, size, 1, fp);
              fwrite(BUFFER, size, 1, output_file);
            }

            fclose(output_file);
          }
        }
      } 
    }

    // reads the file
    else if (!strcmp(token[0], "read"))
    {
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else
      {
        int file_exist = 0;
        int offset;
        // Going through all directories to check for a match
        for (i = 0; i < 16; i++)
        {
          if (!compare(token[1], dir[i].DIR_Name))
          {
            file_exist = 1;
            offset = LBAToOffset(dir[i].DIR_FirstClusterLow) + atoi(token[2]);
          }
        }

        if (file_exist)
        {
          fseek(fp, offset, SEEK_SET);

          // if match found read the file and go to the offset given
          fread(BUFFER, 1, atoi(token[3]), fp);
          
          // printing the requested bytes
          for (i = 0; i < atoi(token[3]); i++)
          {
            printf("%d ", BUFFER[i]);
          }

          if (i == atoi(token[3]))
          {
            printf("\n");
          }
    
        }
        else
        {
          // if we don't find a match
          printf("Error: File not found\n");
        }
      }
    }

    // deletes the file from the directory
    else if (!strcmp(token[0], "del"))
    {

      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }
      else
      {
        int dirExist = 0;
        for (i = 0; i < 16; i++)
        {
          // checks if the file or directory exists
          if (!compare(token[1], dir[i].DIR_Name))
          {
            dirExist = 1;
            break;
          }
        }

        if (dirExist)
        {
          // if it does, switch the attribute to make it invisible and change its name
          Attr = dir[i].DIR_Attr;
          dir[i].DIR_Attr = 0xe5;
          strncpy(dir_holder, dir[i].DIR_Name, 11);
          strcpy(dir[i].DIR_Name, "?");
        }
        else
        {
          printf("Error: File not found.\n");
        }
      }
    }

    // undeletes the file which was deleted earlier
    else if (!strcmp(token[0], "undel"))
    {
      if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else
      {
        int check = 0;
        for (i = 0; i < 16; i++)
        {
          if (!strcmp(dir[i].DIR_Name, "?"))
          {
            check = 1;
            break;
          }
        }
        if (check)
        {
          // replace the name of the deleted directory with the previous one
          strncpy(dir[i].DIR_Name, dir_holder, 11);
          dir[i].DIR_Attr = Attr;
        }
        else
        {
          printf("Error: File not found.\n");
        }
      }
    }

    // changes directory
    else if (!strcmp(token[0], "cd"))
    {
       if (fp == NULL)
      {
        printf("Error: File system image must be opened first.\n");
      }

      else
      {
        for (i = 0; i < 16; i++)
        {
          if (!compare(token[1], dir[i].DIR_Name))
          {
            // get the cluster number to reach to the directory
            int cluster = dir[i].DIR_FirstClusterLow;

            if (cluster == 0) // checking for root
            {
              cluster = 2;
            }
            int offset = LBAToOffset(cluster);

            // using the offset seek to the starting of the directory
            fseek(fp, offset, SEEK_SET);
            // reading the directory
            fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);

            break;
          }
        }

        char *directoy;

        directoy = strtok(token[1], "/");
        for (i = 0; i < 16; i++)
        {
          // we check if the directory exists in the directory structure
          if (!compare(token[1], dir[i].DIR_Name))
          {
            // get the cluster number to get the next offset to the directory
            int cluster = dir[i].DIR_FirstClusterLow;
            int offset = LBAToOffset(cluster);

            if (cluster == 0) // checking for root
            {
              cluster = 2;
            }
            // go to the new offset and start reading
            fseek(fp, offset, SEEK_SET);
            fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);

            break;
          }
        }


        // A case to deal with paths
        while ((directoy = strtok(NULL, "/")))
        {
          int cluster = dir[i].DIR_FirstClusterHigh;

          if (cluster == 0) // checking for root
          {
            cluster = 2;
          }
            
          // get the cluster number to get the next offset to the directory
          int offset = LBAToOffset(cluster);
          
          // go to the new offset and start reading
          fseek(fp, offset, SEEK_SET);
          fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
        }
      }
    }

    // quits or exits the program
    else if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit"))
    {
      exit(0);
    }
    else
    {
      printf("mfs> command not found: %s\n", token[0]);
    }

    free(working_root);
  }
  return 0;
}
