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

function strip_prefix($word) {
    return substr($word, strpos($word, '@') + 1);
}

function check_variable_name($name) {
    # TODO - try to implement using regex
    echo "\t\t -- name of variable: $name\n";
    if (empty($name))
        return false;

    # Check first character - cez regex
    #if (preg_match("'/^[a-zA-Z_\-]/'", $name)) {
    if (ctype_alpha($name[0]) or (in_array($name[0], array('_', '-', '$', '&', '%', '*', '!', '?') ) ) ) {
        print("\t\t- Povoleny zaciatok ID\n");
    }
    else {
        print("\t\t- Nepovoleny zaciatok ID\n");
        return false;
    }
    # Check if rest is only alpha/number
    #if(preg_match('/[a-zA-Z0-9]*/', $name)) {
    if (strlen($name) == 1 or ctype_alnum(substr($name, 1))) {
        print("\t\t- vhodny aj zvysok mena $name\n");
    }
    else {
        print("\t\t- NEvhodny zvysok mena $name\n");
        return false;
    }

    return true;
}

function check_int_constant($number) {
    echo "\t\t -- int constant: $number\n";

    if (preg_match('/^[\+\-]?[0-9]+$/', $number)) {
        print("\t\t- VALID number constant\n");
    }
    else {
        print("\t\t- INVALID number constant\n");
        return false;
    }

    return true;
}

function check_bool_constant($word) {
    echo "\t\t -- bool constant: $word\n";
    if (preg_match('/true|false/', $word)) {
        print("\t\t- VALID bool constant\n");
    }
    else {
        print("\t\t- INVALID bool constant\n");
        return false;
    }
    return true;
}

function check_string_constant($str) {
    echo "\t\t -- string constant: $str\n";

    # Check if contains #
    if (preg_match('/#/', $str)) {
        print("\t\t- INVALID string - contains #\n");
        return false;
    }
    # if contains \ - ckech if followed by xyz number in 000-999
    # preg_match all \xyz
    #preg_match_all('/\\.{,3}/', $str);

    $last_position = 0;
    while (($last_position = strpos($str, '\\', $last_position)) !== false) {
        # On index $last_position is \ -> check if it is in format \xyz
        print("\t\t -- \\ on position $last_position\n");

        # get substring starting from $last_position + 1, length = 3
        $esc_seqv = substr($str, $last_position + 1, 3);
        print("SUBSTR: " . $esc_seqv . "\n");

        if (preg_match('/[0-9][0-9][0-9]/', $esc_seqv)) {
            print("\t\t\t - esc_seqv OK\n");
        }
        else {
            print("\t\t\t - esc_seqv NOT OK\n");
            return false;
        }

        # Update last_position
        $last_position += 1;
    }

    return true;
}

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
        echo "\t\tVariable type\n";
        check_variable_name(strip_prefix($word));
    }
    elseif (preg_match("/^bool@/", $word)) { # Case: bool constant
        echo "\t\tBool constant\n";
        check_bool_constant(strip_prefix($word));
    }
    elseif (preg_match("/^int@/", $word)) { # Case: bool constant
        echo "\t\tInt constant\n";
        check_int_constant(strip_prefix($word));
    }
    elseif (preg_match("/^string@/", $word)) { # Case: bool constant
        // TODO - dokontrolať správnosť konštanty typu string za @
        echo "\t\tString constant\n";
        check_string_constant(strip_prefix($word));
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

