#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *save1, *save2;
    char arg_buff[ARG_MAX] = {0}; // buffer to build the argument list with proper whitespaces
    int cmd_count = 0;

    // split the command line by "|" symbols
    char *command = strtok_r(cmd_line, "|", &save1);
    while( command != NULL ){

        if( cmd_count == 8 ) { return ERR_TOO_MANY_COMMANDS; } // if command count is 8, then we are processing the 9th, that's too many!
        if( strlen(command) > SH_CMD_MAX ) { return ERR_CMD_OR_ARGS_TOO_BIG; }

        // split the individual command by whitespace
        // the first token is the command, and the rest are the args
        char *arg = strtok_r(command, " \t", &save2);
        // make sure it is a valid size
        if( strlen(arg) > EXE_MAX ){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy( clist->commands[cmd_count].exe, arg); 

        arg = strtok_r( NULL, " \t", &save2 );
        while( arg != NULL ){
            strcat(arg_buff, arg);
            strcat(arg_buff, " ");
            arg = strtok_r(NULL, " \t", &save2);
        }
        // make sure agr list isn't too big
        if( strlen(arg_buff) > ARG_MAX ){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        arg_buff[strlen(arg_buff)-1] = '\0'; // remove the last space from the end of the args
        strcpy( clist->commands[cmd_count].args, arg_buff); // copy the arg list into the command list

        memset(arg_buff, '\0', ARG_MAX); // reset the buffer so that we can append it for the next command
        cmd_count++;
        command = strtok_r( NULL, "|", &save1);
    }
    clist->num = cmd_count; // record the number of commands processed into the list

    return OK; 
}