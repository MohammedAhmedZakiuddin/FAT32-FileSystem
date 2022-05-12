#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
  char IMG_Name[11] = "FOO     TXT";

  char input[7] = "foo.txt";


  char expanded_name[12];

  // filling the block of memory with data.
  memset( expanded_name, ' ', 12 )  ;

  char *token = strtok( input, "." );
  // copying it to expanded name
  strncpy( expanded_name, token, strlen( token ));
  //breaks the string series into tokens 
  token = strtok( NULL, "." );

  if(token)
  {
    strncpy( (char*)(expanded_name + 8), token, strlen(token ) );
  }

  expanded_name[11] = '\0';

  int i;
  for( i = 0; i < 11; i++ )
  {
    expanded_name[i] = toupper( expanded_name[i] );
  }

  if( strncmp( expanded_name, IMG_Name, 11 ) == 0 )
  {
    printf("They matched\n");
  }

  return 0;
}



// void compare(char *DIR_name, char *_input)
// {
//     char expanded_name[12];
//     memset(expanded_name, ' ', 12);

//     char input[12];
//     memcpy( input, _input, 11);

//     char user_name[12];
//     strcpy(user_name, input)

//     char *token = strtok(input, "." );

//     if(token)
//     {
//         strncpy(expanded_name, token, strlen(token));

//         token = strtok(NULL, "." );

//         if(token)
//         {
//             strncpy((char*)(expanded_name + 8), token, strlen(token));
//         }

//         expanded_name[11] = '\0';

//         int i;
//         for( i = 0; i < 11; i++ )
//         {
//             expanded_name[i] = toupper( expanded_name[i] );
//         }
//     }

//     else
//     {
//         strcmp(expanded_name, DIR_name, strlen(DIR_name));
//         expanded_name[11] = '\0';
//     }

//     return 0;
// }