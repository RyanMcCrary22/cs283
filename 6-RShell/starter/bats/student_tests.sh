#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file


@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

### Tests for remote shell



### Make sure old tests pass in local mode

@test "can our script run" {
    run ./dsh <<EOF

EOF

    # Assertions
    [ "$?" -eq 0 ]
}

@test "command with no args prints error" {

    run ./dsh <<EOF

EOF
    # strip all whitespace from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="localmodedsh4>warning:nocommandsprovideddsh4>cmdloopreturned0"

    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    #Check exact match 
    [ "$stripped_output" = "$expected_output" ]
}

##### TESTS FOR BUILT INS #######
# CD

@test "cd doesnt change directory with no args" {
    current=$(pwd)

    cd /tmp
    run "${current}/dsh" <<EOF 
cd
pwd
EOF

    # strip all whitespace from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/tmplocalmodedsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    #Check exact match 
    [ "$stripped_output" = "$expected_output" ]
}

@test "cd enters correct directory" {
    current=$(pwd)

    mkdir assn7_tmp

    run "./dsh" <<EOF 
cd assn7_tmp
pwd
EOF
    rm -r assn7_tmp

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')    

    expected_output="$current/assn7_tmplocalmodedsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    #Check exact match 
    [ "$stripped_output" = "$expected_output" ]
}

@test "2 or more args for cd" {

    run "./dsh" <<EOF
cd bla bla bla
EOF

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="localmodedsh4>ERR:toomanyargsforcdcommanddsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    #Check exact match 
    [ "$stripped_output" = "$expected_output" ]
}

@test "cd to invalid dir" {
    run ./dsh <<EOF
cd bruh_tmp
EOF
    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="cderror:Nosuchfileordirectorylocalmodedsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    #Check exact match 
    [ "$stripped_output" = "$expected_output" ]
}

# EXIT

@test "exit returns code 0" {

    run ./dsh <<EOF
exit
EOF

    # Assertions
    [ $status -eq 0 ]

}

@test "exit with args prints error" {

    run ./dsh <<EOF
exit blah blah balh
EOF

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="localmodedsh4>exiterror:commandtakesnoargsdsh4>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}

#### PIPED Tests 

@test "piping ls after chdir" {

    mkdir assn7_tmp

    run ./dsh <<EOF
cd assn7_tmp
ls -l | wc -w
EOF

    rm -r assn7_tmp
    
    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="2localmodedsh4>dsh4>dsh4>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}

@test "piped command with invalid args" {
    run ./dsh << EOF
ls -l | wc -5
EOF

     # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="wc:invalidoption--'5'Try'wc--help'formoreinformation.localmodedsh4>dsh4>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}

@test "too many piped commands" {
    run ./dsh <<EOF
cmd 1 | cmd 2 | mcd 3 | 4 | 5 | 6 | 7 | 8 | 9
EOF

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="localmodedsh4>error:pipinglimitedto8commandsdsh4>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}

@test "piping an invalid command" {
    run ./dsh <<EOF
ls -a | bruh this not a command 
EOF

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="execvp:Nosuchfileordirectorylocalmodedsh4>localmodedsh4>dsh4>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}


