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

    expected_output="dsh3>warning:nocommandsprovideddsh3>cmdloopreturned0"

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

    expected_output="/tmpdsh3>dsh3>dsh3>cmdloopreturned0"

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

    run "./dsh" <<EOF 
cd assn6_tmp
pwd
EOF

    # strip all whitespaces form output
    stripped_output=$(echo "$output" | tr -d '[:space:]')    

    expected_output="$current/assn6_tmpdsh3>dsh3>dsh3>cmdloopreturned0"

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

    expected_output="dsh3>ERR:toomanyargsforcdcommanddsh3>cmdloopreturned0"

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

    expected_output="cderror:Nosuchfileordirectorydsh3>dsh3>cmdloopreturned0"

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

    expected_output="dsh3>exiterror:commandtakesnoargsdsh3>cmdloopreturned0"     

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions
    [ "$stripped_output" = "$expected_output" ]
}