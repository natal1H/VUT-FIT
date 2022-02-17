#/bin/bash

if [ -r "../../main" ]
    then
      RED='\033[0;31m'
  GREEN='\033[0;32m'
  NC='\033[0m'
        cd ../../
        ./main <TestIFJ/cmake-build-debug/input.txt > TestIFJ/cmake-build-debug/IFJ18_output.txt
                        A=$?
        if [  $A = 0 ]
        then
            echo ${GREEN}Run_IFJ.sh OK  ${NC}
        else
            echo ${RED}Run_IFJ.sh FAIL ${NC}
        fi
fi        

