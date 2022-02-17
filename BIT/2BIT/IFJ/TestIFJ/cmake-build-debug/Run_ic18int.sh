 #/bin/bash

if [ -r "Clean_IFJ_Code.txt" ]
    then
  RED='\033[0;31m'
  GREEN='\033[0;32m'
  NC='\033[0m'

        ./ic18int Clean_IFJ_Code.txt > IFJ18_result.txt
                A=$?
        if [  $A = 0 ]
        then
            echo ${GREEN}Run_ic18int OK  ${NC}
        else
            echo ${RED}Run_ic18int FAIL ${NC}
        fi
fi