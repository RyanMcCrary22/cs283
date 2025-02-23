1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  For our shell, we need to use fork and then exec to be able to handle more than one command until the user chooses to exit. If instead we just took in the first command, and if it wasn't built in we called execvp(), the syscall would replace our shells process with the new process in memory, and once that processed terminated, so would our shell. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  fork() will return -1 if it fails to fork the process. My shell handles this by printing an error message using perror(), and then returning with error code -6, because I think if the shell is having trouble forking, then you should not continue to run it. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() finds the command to execute by matching the filename of the argument provided to the names of files in the directories specified by the $PATH environment variable. Or, if you supply a filename with '/' characters, execvp() will use the relative path to the filename given and ignore the environment path. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling wait() ensures that our parent shell and child will execute in the sequence we want. The parents next move after forking is to restart the loop and print the shell prompt, but we want this to happen after the child terminates, so we tell the parent to wait() which will stall it from running until its child proccess "has a change in state", i.e. returns. If we didn't call wait, then the prompt could print from the parent in the middle of or before the childs functionality. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() is used with the wait() syscall to extract the return status from the child process when it finsishes. This is important information, because in many cases including our shell, the programmer will want to handle error returns from the child process differently than returns of 0, so having this exit code is obviously necessary to do so. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My build_cmd_buff() handles quoted args as 1 arg from the start of the quote to the end, regardless of whitespaces. This is necessary because our shell is operating on top of unix commands that expect arguments to be passed to them this way. Also, if we ever want to implement special characters like "$" or "*" into our shell, we will need to implement quote handling into our shell for them to work propely so bunching args is a good way to start and be able to implement the next step pretty easily. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  My parsing logic is much different in this assignment than the last. In the last assignment I split up commands using nested loops with strtok(), looking for spaces and pipe characters. Now in this assignment I built the command by walking along each character of the command and splitting on spaces while also watching for quotes, and setting each argument pointer to the start of the next word. I dealt with a really dumb typo bug while refactoring the code across from the last assignment that made this much harder than it ever needed to be, but overall the proccess was not very difficult. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are used in a linux system to send messages to proccesses about an external event, examples could be a child process terminating, the user pressing ctrl-c, the cpu overheating, etc. Signals differ from other forms of IPC because they do not actually pass any data between processes, they only notify of an event. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL, SIGTERM and SIGINT are all signals that deal with terminating proccesses. SIGINT and SIGTERM are more "friendly interrupt" versions, and both can be handled by the programmer to either be ignored or make some specified clean up before the process terminates. SIGINT is sent to an interactive process by the user by pressing ctrl-c. SIGTERM is sent between processes without the user. SIGKILL also terminates programs, but unlike the other two options it is unignorable. SIGKILL is usually used when a proccess is frozen or unresponsive and therefore would likely never terminate wihtout the signal. 

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process recieves SIGSTOP, it is immediately paused but not terminated. This means that the process is left in memory in its active state, but it will not run until it recieves the SIGCONT or continue signal. SIGSTOP cannot be ignored. 
