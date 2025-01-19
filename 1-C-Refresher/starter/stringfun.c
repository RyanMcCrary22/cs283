/**************************************************************
* stringfun.c - simple string proccessing program to parse a string given by the command line user
*					 use -h for help 
* author - Ryan McCrary w/ starter code adapted from the CS283 proffessors
*
* written on - Linux tux4 5.15.0-121-generic
*
**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string( char *, int, int);
void swap_char( char *, int, int);
void print_buff_no_dots( char *, int );
void print_words( char *, int, int );


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions

	 // need 2 seperate counters to handle consecutive whitespace
	 int buff_len = 0; // counter for the nuber of chars added to buff
	 int counter = 0; // counter for the number of chars in the input string

	 // need two predicates to control the state of whitespace
	 int first_char = 0;
	 int consecutive_space = 0;

	 // loop through the string and copy each character to buff
	 while( *(user_str + counter) != '\0' )
	 {
		// check for when the user input is larger than our buffer
		if( counter >= len )
		{
			printf( "Error: provided string is too long\n" );
			return -1; 
		}
		// check for tabs and other whitespace
		if( *(user_str + counter) == ' ' || *(user_str + counter) == '\t' )
		{
			// we will only add a space to buff if the first non-whitespace character has appeared, and it is not
			// following another whitespace
			if( !(first_char == 0 || consecutive_space == 1) )
			{
				*(buff+buff_len) = ' '; 
				buff_len++;
				consecutive_space = 1; // set the state to say we just saw a space
			}
		} else // anything else we want to add to buff
		{
			first_char = 1; // we will accept whitespace after the first non-space char
			consecutive_space = 0; // seeing a non-whitespace char resets the state
			*(buff+buff_len) = *(user_str + counter);
			buff_len++;
		}
		counter ++;
	 }

	 memset( (buff+buff_len) , '.', len - buff_len ); // fill the rest of buff with '.' 

    return buff_len; // return the length of the input string 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
	 // len is the length of the buffer and str_len is the length of the user string inside the buffer before the '.'
	 // start
	 int word_count = 0;
	 int char_counter = 0; // counter to iterate through the buffer
	 int new_word = 1; // state tracker: 1 if a space is seen and 1 to start

	 while( char_counter < str_len )
	 {
	 	if( *(buff+char_counter) == ' ' )
		{
			new_word = 1; // set new word to 1 on every space
		} else {
			if( new_word == 1 )
			{
				// if new word is 1 and char isnt a space, this is the start of a new word
				word_count++;
				new_word = 0;
			}
		}
		char_counter++; // move to the next character
	 }
    return word_count;
}

void swap_char( char *buff, int ind1, int ind2 ){
	// swap the characters at positions ind1 and ind2 inside of the buffer
	char temp = *(buff + ind1);
	*(buff+ind1) = *(buff+ind2);
	*(buff+ind2) = temp;
}

void reverse_string( char *buff, int len, int str_len){
	// front and end counters to move through the string
	int front = 0;
	int end = str_len-1;

	while( !( front >= end ) )
	{
		swap_char( buff, front, end );
		front++;
		end--;
	}
}

void print_buff_no_dots( char *buff, int str_len ){
	for( int i = 0; i<str_len; i++ )
	{
		putchar( *(buff+i) );
	}
}

void print_words( char *buff, int len, int str_len ){
	// print the header
	printf( "Word Print\n" );
	printf( "__________\n");
	// set up local variables 
	int word_len = 0; // counter to track the length of individual words
	int word_count = 0; // counter for the number of words processed in the buff
	int buff_ctr = 0; // counter to track the total number of characters processed
	int at_start = 1; // boolean int, 1 if we are at the start of a new word i.e after a space, 0 otherwise
	// loop over the buffer
	while( buff_ctr < str_len )
	{
		if( at_start )
		{
			// before each new word, increment the word count and print it
			word_count++;
			printf( "%d. ", word_count );
			at_start = 0; // set to 0 because we have enetered the word
		}
		// proccess the characters
		if( *(buff+buff_ctr) == ' ' )
		{
			printf( "(%d)\n", word_len );
			word_len = 0; // reset the word length before a new word
			at_start = 1; // set to 1 because we have processed the last word
		} else {
			word_len++;
			putchar( *(buff+buff_ctr) );
		}
		buff_ctr++; // move to the next character
	}
	printf( "(%d)\n", word_count );
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
	 //
	 /* This is safe because if argv[1] does not exist then (argc < 2) will evaluate to true and therefore so will the
	 * or statement as a whole, meaning the conditional will execute and the program will exit.
	 */

    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
	 /*
	 * The if statement below catches a case where the user input an argument flag for the mode of the program, but
	 * forgot to include the string to be acted on. 
	 */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string


    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
	 buff = malloc( BUFFER_SZ );
	 if( !buff )
	 {
	 	printf( "Error allocating memory"  );
		exit(99);
	 }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
		  
		  case 'r':
		 		reverse_string( buff, BUFFER_SZ, user_str_len );
				printf( "Reversed String: " );
				print_buff_no_dots( buff, user_str_len );
				putchar( '\n' );
				break;
		 
		  case 'w':
				print_words( buff, BUFFER_SZ, user_str_len );
				
				break;

		  case 'x':
		  		if( argc < 5 )
				{
					printf( "Error: not enough arguments presented for replacment" );
					exit( 1 );
				} 
				printf( "Not implemented\n" );
				break;

        default:
            usage(argv[0]);
            exit(1);
    }

    // print_buff(buff,BUFFER_SZ);
	 free( buff );
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Providing both the length and the buffer is helpful when checking for error cases, like when you somehow
//          overrun the buffer. Passing the length into the function directly is a good idea for two reasons, first it
//          can be more efficient because this way the value then can be passed around
//          as a variable rather than running strlen() or something similar every time your function iterates over the
//          buffer. It also helps the code to be cleaner with less dependencies, because even though we know that the
//          buffer length is 50, if we had hard coded that into each function and then someday for some reason we
//          decided to change the value to 100, we would have to go find every instance that we hard coded 50 and
//          change it, instead in the format we have it we only have to change the global value and the whole program
//          would follow. 
