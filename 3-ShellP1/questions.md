1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  Shells operate on commands that are sent in line by line, and fgets() reads input 1 line at a time, therefore it kind of naturally pulls 1 command from stdin at a time. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:   In the way the code was supplied, cmd_buff was initialized as a char pointer by "char *cmd_buff;". In this case, the buffer is just a pointer pointing to nothing when the program starts, so if you tried to copy a string into it, you would be trying to copy a string into null space of memory and would get an error. This is why you need to first allocate that space using malloc and then fill it with strcpy() or something similar. ( I don't fully understand this question, because I originally changed the code to "char cmd_buff[SH_CMD_MAX]" which worked the same as using malloc?)


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  When we start executing commands, the shell will rely on comparing the string given in the command to program file names stored locally on disk, so if we were to leave trailing spaces at the end of the command strings, we could run into bugs where correctly spelled commands couldn't be found because the two strings wouldn't exactly match. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Our shell should eventually be able to use "<" in a command to redirect the filename that follows into the stdin of the program executed by the command before the "<". Then, our shell should also be able to recognize ">" as the user asking for the stdout of the command before the ">" to be redirected and written into the file that follows. There are two forms of this redirection, the one I just described ">" should overwrite any existing data in the given file, and our shell should also be able to recognize ">>" and instead append the stdout of the command to the end of the given file. These two redirections also have parallels "2>" and "2>>" that instead redirect the stderr from the first command into the file given after. 

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  As said above, redirection in the shell is used to redirect a single commands stdin stdout or stderr into or from a given filename. Pipes are slightly different, because instead of a filename on the rigth side, pipes have a second command, and they work by redirecting the stdout of the command on the left into the stdin of the command on the right, allowing the two processes to communicate with each other. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  When we start creating more complex programs, we are going to want to act accordingly when some process throws an error, and if we have to randomly search through the entire output stream to find the place where the error message printed, it is much more complicated than directly accessing a seperate stream used only for error messages. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  I think errors from failing commands should definitley be printed to the screen, because a lot of commands exist that would don't produce any output when ran correctly, so allowing failed commands to also print nothing out would be very confusing. With that said, I still think the shell needs a way to combine the two so that they can be passed together between pipes when the program recieving the data needs both the output and error messages. Without some way to combine the two streams only 1 could be redirected at a time. We could do this the same way bash does by looking for the "2>&1" symbol at the end of a command.