# C Shell
## Installation:
To run the code, use the given makefile, which generates main as output file.
## Files and their uses:
   ### Headers
   - headerlist.h contains all the system header files to be included
   - listing.h is the header corresponding to listing.c
   - printuser.h is the header corresponding to printuser.c
   - tokenize.h is the header corresponding to tokenize.c
   
 ### C files:
   - listing.c contains the functions for ls incl its both flags 
   - printuser.c includes the functions for printing user and host along with current working dir.
   - tokenize.c contains the functions for tokenizing a string by ';', ' ' or newline. It also includes a function to trim leading and trailing spaces.
   - main.c is the main program 
### Basic Working
The Shell program runs on an infinite loop and takes input from stdin using getline(), which is further tokenized into semicolon-separated commands, which are run in order. Each command is tokenized separating the commands,flags(if any) and the arguments(if any). Switch-case is used in order to select from the executable commands like rm,mkdir etc and other self-defined commands pwd, ls, cd, echo, pinfo and history.

### Assumptions :
   - /proc filesystem is accessible as is to any standard linux user.
    - no of semicolon separated commands is maximum 50, with 2000 char as max length for each.
    - each command can have maximum value of x as 50, where argv[x].
    - username, hostname and cwd are 1000 characters at max.

### Other Functionalities added:
- input/output redirection
- piping
- User defined commands:
	- setenv 
	- unsetenv
	- jobs
	- kjob
	- fg 
	- bg
	- overkill
	- quit
- signal handling
	- ctrl+c
	- ctrl+d
	- ctrl+z
