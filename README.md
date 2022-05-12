# FAT-32 (File System)

This code file allocates table, endieness as well as file access and implements a user space shell application that is capable of interpreting a FAT-32 file system image. <br />

Commands (To run the code):

gcc mfs.c <br />
./a.out <br />

Once you are into the file system: <br />

open <filename>: <br />
This command shall open a fat32 image. Filenames of fat32 images shall not contain spaces and shall be limited to 100 characters.<br />
close: This command closes the fat32 image. If the file system is not currently open your program shall output: “Error: File system not open.” <br /> 
info: This command prints out the information about the file system in both hexadecimal and base 10 format. <br />
  
• BPB_BytesPerSec <br />
• BPB_SecPerClus <br />
• BPB_RsvdSecCnt <br />
• BPB_NumFATS <br />
• BPB_FATSz32 <br />
  
stat <filename> or <directory name>: <br />
This command shall print the attributes and starting cluster number of the file or directory name. <br />
If the parameter is a directory name then the size shall be 0. <br />

get <filename>: <br />
This command shall retrieve the file from the FAT 32 image and place it in your current working directory. <br />

error: <br />
Prints if the file or directory does not exist then your program shall output “Error: File not found”. <br />

cd <directory<>> <br />
This command changes the current working directory to the given directory. <br />
Your program shall support relative paths, e.g cd ../name and absolute paths. <br />

ls <br />
Lists the directory contents. Supports listing "." and ".." <br />
  
read <filename> <position> <number of bytes> <br />
Reads from the given file at the position, in bytes, specified by the position parameter and output the number of bytes specified. <br />

del <filename> <br />
Deletes the file from the file system. <br />
  
undel <filename> <br />
Un-deletes the file from the file system. <br />
