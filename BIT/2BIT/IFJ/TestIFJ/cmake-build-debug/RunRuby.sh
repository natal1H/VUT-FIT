  #/bin/bash

  RED='\033[0;31m'
  GREEN='\033[0;32m'
  NC='\033[0m'

    cp input.txt input.rb
    ruby input.rb > RUBY_result.txt
        A=$?
        if [  $A = 0 ]
        then
            echo ${GREEN}RunRuby.sh OK  ${NC}
            printf "\n"
        else
            echo ${RED}RunRuby.sh FAIL ${NC}
            printf "\n"
        fi

    #echo >> RUBY_result.txt
    rm input.rb