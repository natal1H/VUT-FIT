<?php

$opcodes = array(
    "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN", # Work with frames, calling functions
    "PUSHS", "POPS", # Work with data stack
    "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "NOT", "INT2CHAR", "STRI2INT", # Arithmetic, relational, boolean and conversion instructions
    "READ", "WRITE", # I/O instructions
    "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", # Working with strings
    "TYPE", # Working with types
    "LABEL", "JUMP", "JUMPIFEQ", "JUMPIFNEQ", "EXIT", # Controlling program flow
    "DPRINT", "BREAK" # Debugging
);

function determine_token($word) {
    echo "\t\tI'm now going to determing the type of word $word...\n";

    global $opcodes;

    if (strtolower($word) == ".ippcode19") { # Case: Header
        echo "\t\tHEADER type\n";
    }
    elseif (preg_match("/^#/", $word)) { # Case: comment
        echo "\t\tComment type\n";
    }
    elseif (preg_match("/^GF@/", $word) or preg_match("/^LF@/", $word) or preg_match("/^TF@/", $word)) { # Case: variable
        // TODO - dokontrolovať aj názov premennej či neobsahuje nepovolené znaky
        echo "\t\tVariable type\n";
    }
    elseif (preg_match("/^bool@/", $word)) { # Case: bool constant
        // TODO - dokontrolať správnosť konštanty typu bool za @
        echo "\t\tBool constant\n";
    }
    elseif (preg_match("/^int@/", $word)) { # Case: bool constant
        // TODO - dokontrolať správnosť konštanty typu int za @
        echo "\t\tInt constant\n";
    }
    elseif (preg_match("/^string@/", $word)) { # Case: bool constant
        // TODO - dokontrolať správnosť konštanty typu string za @
        echo "\t\tString constant\n";
    }
    elseif ($word == "nil@nil") { # Case: bool constant
        echo "\t\tNil constant\n";
    }
    elseif (in_array(strtoupper($word), $opcodes)) { # Case: opcode
        echo "\t\tOpcode\n";
    }
    else {
        echo "\t\tUnknown type - LEX ERROR\n";
    }
}

echo "IPP Project 2018/2019\n";

while ($line = fgets(STDIN)) { # Split input into lines
    echo "line: " . $line;
    #$line = trim($line);
    foreach (preg_split("/[\s\t]+/", $line) as $word) { # Split line into words

        echo "\tword: " . $word . "\n";

        if (strlen($word) == 0) {
            # \n character
            echo "\t\tNewline character\n";
        }
        else {
            # Send each word to function to determine type of token
            determine_token($word);
        }
    }
}

?>

