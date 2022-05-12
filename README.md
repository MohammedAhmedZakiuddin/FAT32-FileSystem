# FAT-32 (File System)

This code file allocates table, endieness as well as file access and implements a user space shell application that is capable of interpreting a FAT-32 file system image. <br />

Commands (To run the code):

gcc mfs.c <br />
./a.out <br />

Once you are into the file system:

open <filename>: This command shall open a fat32 image. Filenames of fat32 images shall not contain spaces and shall be limited to 100 characters.<br />
close: This command closes the fat32 image. If the file system is not currently open your program shall output: “Error: File system not open.” <br /> 
info: This command prints out the information about the file system in both hexadecimal and base 10 format. <br />
  
• BPB_BytesPerSec
• BPB_SecPerClus
• BPB_RsvdSecCnt
• BPB_NumFATS
• BPB_FATSz32
  
stat <filename> or <directory name>: 
