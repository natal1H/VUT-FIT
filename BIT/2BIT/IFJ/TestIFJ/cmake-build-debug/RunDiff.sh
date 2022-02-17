 #/bin/bash

    RED='\033[0;31m'
    GREEN='\033[0;32m'
    NC='\033[0m'

    echo "-----------------------------------TEST-START------------------------------"

    diff RUBY_result.txt IFJ18_result.txt

    #rm RUBY_result.txt IFJ18_result.txt

        A=$?
        if [  $A = 0 ]
        then
            echo ${GREEN}RunDiff.sh OK ${NC}
            printf "\n"
        else
            echo ${RED}RunDiff.sh FAIL ${NC}
            printf "\n"
        fi