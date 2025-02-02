## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**:  Yes, I think it makes things much cleaner to make get student its own function, because the process of going to an id in the database and pulling out that data appears in almost every other functionality required for the database. Having the function return an error code is useful as well, because then get student can be called on in other functions much more seemlessly because you don't have to worry that an error occured inside of the call to get student instead you can just access the return value and act accordingly. 

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** The problem here is that you are returning a pointer to memory that was allocated on the stack only for the function call to get student, so once the function returned that memory block would be free to be overwritten by some other part of the program and the student you thought you got could actually be some random other data.

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** This version of get student would give the desired behavior, because since the memory is allocated on the heap, it will not be lost when the function returns and your student variable will hold the value you want. A problem that could occur here is that now that student you got exists in memory until you decide to free it, but since we are dealing with a seperate database file, that is not really where we want the data to be. At some point, you could get student 50 and have it stored in a variable and then delete student 50 from the database but it would still exist in your program. This could lead to a lot of confusion and misinterpretted data.  


4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** The file starts at byte 0, so when we seeked to byte 64 to add student 1, linux added a hole there of size 64 and when we wrote the student the total size became 64 * 2 or 128. Then, this same process happened for writing ID 3 and 64. The holes are created when the user seeks past the end of a file, and when they are read back they're represented as 0's but they are actually stored as metadata. The file size reported by ls is the actual number of bytes, either physically written or stored as holes from the start to the end of the file. 

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** Linux allocates space for files in byte blocks, which on tux come in sections of 4k bytes. So when we made the first write to the file for id =1, 4k of storage was allocated on the disk for our database, this was so the file could be written up to 4k bytes continously instead of ending up with situations where every byte of a file is at a different place on the disk. Then, when we wrote in the data for id=64 we we were writing to bytes that were outside of that original block allocated to us, so the os allocated another 4k block on the disk to extend the file further, totaling 8k. 

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:**  As we have seen in the last examples, linux fills space that is seeked past the EOF with a hole that is metadata for the os. Since before the id 99999 was added, the file was 2 blocks, the remaining data between id 64 and id 99999 gets filled with one of these holes. The difference here between adding id 64 and adding 99999 is that there is more than 4k space between the previous EOF and our new data, a lot more actually. And since we did not write any data in that space, linux will not allocate blocks where the entire 4k is a hole. So the new file size on disk is only 12k, because now we have three blocks allocated, and the extra space in our file bweteen that is all an empty hole that linux is keeping a record of. 