<?php

class Instruction {
    var $opcode;
    var $arg1;
    var $arg2;
    var $arg3;
    var $order;
}

abstract class TokenType {
    const T_HEADER = 0;
    const T_COMMENT = 1;
    const T_VARIABLE = 2;
    const T_BOOL = 3;
    const T_INT = 4;
    const T_NIL = 5;
    const T_STRING = 6;
    const T_OPCODE = 7;
    const T_UNKNOWN = 8;
}

# Class token:
class Token {
    var $type;
    var $attribute;
    var $valid;

    function setType($new_type) {
        $this->type = $new_type;
    }

    function getType() {
        return $this->type;
    }

    function getAttribute() {
        return $this->attribute;
    }

    function setValidity($new_validity) {
        $this->valid = $new_validity;
    }

    function getValidity() {
        return $this->valid;
    }

    function __construct($str) {
        # Set attribute and default type
        $this->attribute = $str;
        $this->setType(TokenType::T_UNKNOWN);
        $this->valid = true; # true until proven false
    }


    function determine_type() {
        # Will set actual type based on attribute

        global $opcodes; # TODO - make their own class for them

        # Case: Header
        if (strtolower($this->attribute) == ".ippcode19") {
            $this->setType(TokenType::T_HEADER);
        }
        # Case: comment
        elseif (preg_match("/^#/", $this->attribute)) {
            $this->setType(TokenType::T_COMMENT);
        }
        # Case: variable
        elseif (preg_match("/^GF@/", $this->attribute) or preg_match("/^LF@/", $this->attribute) or preg_match("/^TF@/", $this->attribute)) {
            if ($this->checkVariableName($this->stripPrefix($this->attribute))) # Valid var format -> set type to T_VARIABLE
                $this->setType(TokenType::T_VARIABLE);
            else # Invalid var format -> set validity to false
                $this->setValidity(false);
        }
        # Case: bool constant
        elseif (preg_match("/^bool@/", $this->attribute)) {
            if ($this->checkBoolConstant($this->stripPrefix($this->attribute))) # valid bool format -> set type to T_BOOL
                $this->setType(TokenType::T_BOOL);
            else # invalid bool type -> set token validity to false
                $this->setValidity(false);
        }
        # Case: int constant
        elseif (preg_match("/^int@/", $this->attribute)) {
            if ($this->checkIntConstant($this->stripPrefix($this->attribute))) # valid int format -> set type to T_INT
                $this->setType(Tokentype::T_INT);
            else # invalid int format -> set validity to false
                $this->setValidity(false);
        }
        # Case: string constant
        elseif (preg_match("/^string@/", $this->attribute)) {
            if ($this->checkStringConstant($this->stripPrefix($this->attribute))) # valid string format -> set type to T_STRING
                $this->setType(TokenType::T_STRING);
            else # invalid string format -> set validity to false
                $this->setValidity(false);
        }
        # Case: nil constant
        elseif ($this->attribute == "nil@nil") { # set type to T_NIL
            $this->setType(TokenType::T_NIL);
        }
        # Case: opcode
        elseif (in_array(strtoupper($this->attributed), $opcodes)) { # Set type to T_OPCODE
            $this->setType(tokenType::T_OPCODE);
        }
        # Case: lex error
        else { # Set validity to false
            $this->setValidity(false);
        }

    }

    function stripPrefix($word) {
        return substr($word, strpos($word, '@') + 1);
    }

    function checkVariableName($name) {
        # TODO - try to implement using regex
        if (empty($name))
            return false;

        # Check first character
        if (!ctype_alpha($name[0]) and !(in_array($name[0], array('_', '-', '$', '&', '%', '*', '!', '?') ) ) ) {
            return false;
        }

        # Check if rest is only alpha/number
        if (strlen($name) != 1 and !ctype_alnum(substr($name, 1))) {
            return false;
        }

        return true;
    }

    function checkIntConstant($number) {
        return preg_match('/^[\+\-]?[0-9]+$/', $number);
    }

    function checkBoolConstant($word) {
        return preg_match('/true|false/', $word);
    }

    function checkStringConstant($str) {
        # Check if contains #
        if (preg_match('/#/', $str)) {
            return false;
        }

        # if contains \ - check if followed by xyz number in 000-999
        $last_position = 0;
        while (($last_position = strpos($str, '\\', $last_position)) !== false) {
            # On index $last_position is \ -> check if it is in format \xyz

            # get substring starting from $last_position + 1, length = 3
            $esc_seqv = substr($str, $last_position + 1, 3);

            if (!preg_match('/[0-9][0-9][0-9]/', $esc_seqv)) {
                return false;
            }

            # Update last_position
            $last_position += 1;
        }

        return true;
    }

}
# end class token


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


$error_occurred = false;
$line_number = 0;
### Main loop
while ($line = fgets(STDIN)) { # Split input into lines
    # 1 line = 1 instruction

    # Create new intruction
    $instruction = new Instruction;

    echo "line: " . $line;

    # Array for tokens
    $token_array = [];

    foreach (preg_split("/[\s\t]+/", $line) as $word) { # Split line into words

        echo "\tword: " . $word . "\n";

        if (strlen($word) == 0) {
            # \n character
            echo "\t\tNewline character\n";
            break; # Break foreach - begin syntax analysis with acquired tokens
        }
        else {
            print("-- creating new token\n");
            # Create token and determine its type
            $token = new Token($word);
            $token->determine_type();
            print("Attribute: " . $token->getAttribute() . "\n");
            print("Type: " . $token->getType() . "\n");

            if ($token->getValidity()) {
                print("Valid token\n");

                $token_array[] = $token;
            }
            else {
                print("Invalid token, LEX ERROR\n");
                $error_occurred = true;
                break; # Break foreach
            }
        }
    }

    if ($error_occurred) {
        # TODO - error handling
        print("Error occured - stopping analysis.\n");
        break; # break while
    }

    # No lexical error, move onto syntax analysis
    print("Beginning syntax analysis of line $line_number.\n");

    # print tokens just to check
    foreach ($token_array as $token) {
        print(" " . $token->attribute);
    }
    print("\n");

    $line_number += 1;
}

if (!$error_occurred) {
    print("Program analysis without error.\n");
}
else {
    print("Program analysis with error.\n");
}
?>

