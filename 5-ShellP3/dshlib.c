#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

// allocate the memory for our command structure
int alloc_cmd_buff( cmd_buff_t *cmd ){
    cmd->_cmd_buffer = malloc( SH_CMD_MAX ); // allocates room for the whole command to be stored in the struct
    if( cmd->_cmd_buffer == NULL ) { return ERR_MEMORY; }

    return OK;
}

int alloc_cmd_list( command_list_t *cmds ){
    if ( !cmds ){
        printf("yep this happened\n");
        return ERR_MEMORY;
    }
    int rc;
    for( int i = 0; i < CMD_MAX; i++ )
    {
        rc = alloc_cmd_buff( &cmds->commands[i] ); // allocate the buffer for every command
        if( rc == ERR_MEMORY ) {return ERR_MEMORY; }
    }
    return OK;
}

 // free the command struct
 int free_cmd_buff( cmd_buff_t *cmd ){
    free( cmd->_cmd_buffer ); 
    return OK;
}

int free_cmd_list( command_list_t *cmds ){
    //int rc;
    for( int i = 0; i < CMD_MAX; i++ )
    {
        free_cmd_buff( &cmds->commands[i] ); 
    }
    return OK;
}

 // parse the command - split args by spaces and set all pointers
 int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff){
    // local handling vars
    bool quote_mode = false;
    bool new_word = true; // tracks when a new token is starting in the command, so a pointer should be set
    int char_ctr = 0; // counter to walk across the buffer

    cmd_buff->argc = 0; // reset argc and increment it while parsing


    memcpy( cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX );

    while( cmd_buff->_cmd_buffer[char_ctr] != '\0' )
    {
        if( cmd_buff->_cmd_buffer[char_ctr] == ' ' )
        {
            if ( !quote_mode )
            {
                new_word = true;  // next non-space will be the start of a new arg
                cmd_buff->_cmd_buffer[char_ctr] = '\0'; // split the args so each pointer ends at the spaces   
            }
        }
        else if (cmd_buff->_cmd_buffer[char_ctr] == '"')
        {
            if( !quote_mode )
            {
                // if quote mode is off, we want to start our next arg here, so mimic the start of a new word normally
                cmd_buff->argv[cmd_buff->argc] = cmd_buff->_cmd_buffer+char_ctr+1; // +1 bc the current character is the " so we want the next one
                cmd_buff->argc++; // increment argc
                new_word = false; // reset to false until the next space is hit
            }
            // handle quote mode on/off
            quote_mode = !quote_mode;
            cmd_buff->_cmd_buffer[char_ctr] = '\0';
        }
        else
        {
            if( new_word )
            {
                // super ugly line of code
                // it is saying the next open pointer in argv now points to the start of this new word
                //printf("%s\n", cmd_buff->_cmd_buffer+char_ctr);
                cmd_buff->argv[cmd_buff->argc] = cmd_buff->_cmd_buffer+char_ctr;
                cmd_buff->argc++; // increment argc
                new_word = false; // reset to false until the next space is hit
            }
        }
        char_ctr++;
    }

    // point all of the unused args to null
    // this is required for the expected input to exec()
    for( int i = cmd_buff->argc; i < CMD_ARGV_MAX; i++ )
    {
        cmd_buff->argv[i] = NULL;
    }
    
    return OK;
}

 // reset the command buffer before the next command
 int clear_cmd_buff( cmd_buff_t *cmd ){
    memset( cmd->_cmd_buffer, '\0', SH_CMD_MAX );

    return OK;
 }

int build_cmd_list( char *cmd_line, command_list_t *cmds ){
    int cmd_count = 0;
    char *tok_buff;

    tok_buff = strtok( cmd_line, PIPE_STRING );  // split the command line by pipe characters
    while( tok_buff != NULL )
    {
        build_cmd_buff( tok_buff, &cmds->commands[cmd_count] );
        cmd_count++;
        tok_buff = strtok( NULL, PIPE_STRING );
    }
    cmds->num = cmd_count; // assign the number of commands
    return OK;
}

int clear_cmd_list( command_list_t *cmds ){
    for( int i = 0; i < CMD_MAX; i++ )
    {
        clear_cmd_buff( &cmds->commands[i] );
    }
    return OK;
}

 Built_In_Cmds match_command(char *input){
    // check for all built in commands, return not built in if not found
    if ( strcmp(input, EXIT_CMD) == 0 ) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0 ) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0 ) {
        return BI_CMD_CD;
    } else {
        return BI_NOT_BI;
    }
 }

 int exec_cd( cmd_buff_t *cmd ){
    int rc = OK;

    switch( cmd->argc ){
        case 1:
            // when argc is 1, cd was called with no args
            // do nothing
            break;
        case 2:
            rc = chdir( cmd->argv[1] );
            //check for errors
            if (rc < 0 )
            {
                //chdir() returns -1 on any error
                perror( "cd error" );
            }
            break;
        default:
            // any other case, there are too many args
            printf( "ERR: too many args for cd command\n" );
            rc = ERR_CMD_ARGS_BAD;
            break;
    }
    return rc;
 }

 Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd){
    Built_In_Cmds match = match_command( cmd->argv[0] ); //check which command is called

    switch (match) {
        case BI_CMD_EXIT:
            if( cmd->argc > 1 ) {
                printf( "%s", EXIT_ARG_ERR_MSG );
            } else {
                exit(OK);
            }
            break; // don't need this, but just so it looks better haha

        case BI_CMD_CD:
            exec_cd( cmd );
            break;
        
        case BI_CMD_DRAGON:
            print_dragon();
            break;
        
        case BI_NOT_BI: // this is effectively our default case
            return BI_NOT_BI;
            break; // also dont need this one
        
        default:
            // only reached here if something went wrong, so return the error option
            return BI_ERR;
            break;
    }
    return BI_EXECUTED;
 }

 int execute_pipeline(command_list_t *clist) {
    int pipes[clist->num - 1][2];  // Array of pipes
    pid_t pids[clist->num];        // Array to store process IDs

    // Create all necessary pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Create processes for each command
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], NULL, 0);
        return OK;
    }
return OK;
}


int exec_local_cmd_loop()
{
    char *cmd_buff = malloc( SH_CMD_MAX );
    int rc = 0;  
    // cmd_buff_t cmd;  // memory for storing each command as it's proccessed
    command_list_t cmds;
    //alloc_cmd_buff( &cmd );
    alloc_cmd_list( &cmds );

    Built_In_Cmds bi_tracker; // handle the returns of functions that deal with built in commands
    int f_result, c_result; // return code handling for fork/exec

    while(1){
        printf("%s", SH_PROMPT);
        if( fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL ){
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0'; 

        //rc = build_cmd_buff( cmd_buff, &cmd );
        rc = build_cmd_list( cmd_buff, &cmds );

        // check for empty command
        if( cmds.num < 1 ){
            // if < 1, no command was typed
            printf( "%s", CMD_WARN_NO_CMD );
            continue;
        } else if (cmds.num == 1) {

            bi_tracker = exec_built_in_cmd( &cmds.commands[0] );  
            // check status of built in return 
            if( bi_tracker == BI_NOT_BI )  // if command not built in, fork and exec
            {
                f_result = fork();
                if (f_result < 0 )
                {
                    perror( "fork error" );
                    return ERR_EXEC_CMD;
                }

                //parent child logic
                if (f_result == 0){
                    //child logic
                    //printf( "FROM CHILD: %s\n", cmd.argv[0] );
                    rc = execvp( cmds.commands[0].argv[0], cmds.commands[0].argv );
                    if( rc < 0 ) // exec returns -1 on error
                    {
                        perror( "exec error" );
                        //printf( "errno: %d\n", errno );
                        exit(ERR_EXEC_CMD);
                    }
                } else {
                    //parent logic
                    wait( &c_result ); // wait for the child process to exit
                    // extra cred error handling would go here
                }
            }
        } else {
            rc = execute_pipeline( &cmds );
        }
    }


    free(cmd_buff); // gotta free it :)
    //free_cmd_buff( &cmd );
    free_cmd_list( &cmds );
    
    return OK;
}
