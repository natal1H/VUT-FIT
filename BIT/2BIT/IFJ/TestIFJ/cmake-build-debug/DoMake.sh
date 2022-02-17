 #/bin/bash
if [ -r "../../Makefile" ]
   then
       RED='\033[0;31m'
       GREEN='\033[0;32m'
       NC='\033[0m'
       cd ../../ && make
                       A=$?
        if [  $A = 0 ]
        then
            echo ${GREEN}Makefile OK  ${NC}
        else
            echo ${RED}Makefile FAIL ${NC}
        fi
fi
