<?php

# TODO - argumenty programu

class Instruction {
    var $opcode;
    var $arg1;
    var $arg2;
    var $arg3;
    var $order;

    function getOpcode() {
        return $this->opcode;
    }

    function setOpcode($str) {
        $this->opcode = strtoupper($str);
    }

    static function isOpcode($str) {
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

        if (in_array(strtoupper($str), $opcodes))
            return true;
        else
            return false;
    }

    static function getNumberOfArgs($opcode) {
        $no_args = array(
          "CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"
        );
        $one_arg = array(
          "DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"
        );
        $two_args = array(
            "MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE",
        );
        $three_args = array(
            "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "NOT", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"
        );



        if (in_array($opcode, $no_args))
            return 0;
        elseif (in_array($opcode, $one_arg))
            return 1;
        elseif (in_array($opcode, $two_args))
            return 2;
        elseif (in_array($opcode, $three_args))
            return 3;
        else
            return -1;
    }
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
    const T_LABEL = 8;
    const T_UNKNOWN = 9;
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
        elseif (preg_match("/^#/", $this->getAttribute())) {
            $this->setType(TokenType::T_COMMENT);
        }
        # Case: variable
        elseif (preg_match("/^GF@/", $this->getAttribute()) or preg_match("/^LF@/", $this->attribute) or preg_match("/^TF@/", $this->attribute)) {
            if ($this->checkVariableName($this->stripPrefix($this->getAttribute()))) # Valid var format -> set type to T_VARIABLE
                $this->setType(TokenType::T_VARIABLE);
            else # Invalid var format -> set validity to false
                $this->setValidity(false);
        }
        # Case: bool constant
        elseif (preg_match("/^bool@/", $this->getAttribute())) {
            if ($this->checkBoolConstant($this->stripPrefix($this->getAttribute()))) # valid bool format -> set type to T_BOOL
                $this->setType(TokenType::T_BOOL);
            else # invalid bool type -> set token validity to false
                $this->setValidity(false);
        }
        # Case: int constant
        elseif (preg_match("/^int@/", $this->getAttribute())) {
            if ($this->checkIntConstant($this->stripPrefix($this->getAttribute()))) # valid int format -> set type to T_INT
                $this->setType(Tokentype::T_INT);
            else # invalid int format -> set validity to false
                $this->setValidity(false);
        }
        # Case: string constant
        elseif (preg_match("/^string@/", $this->getAttribute())) {
            if ($this->checkStringConstant($this->stripPrefix($this->getAttribute()))) # valid string format -> set type to T_STRING
                $this->setType(TokenType::T_STRING);
            else # invalid string format -> set validity to false
                $this->setValidity(false);
        }
        # Case: nil constant
        elseif ($this->getAttribute() == "nil@nil") { # set type to T_NIL
            $this->setType(TokenType::T_NIL);
        }
        # Case: opcode
        #elseif (in_array(strtoupper($this->getAttribute()), $opcodes)) { # Set type to T_OPCODE
        elseif (Instruction::isOpcode($this->getAttribute())) { # Set type to T_OPCODE
            $this->setType(TokenType::T_OPCODE);
        }
        # Case: label
        elseif ($this->checkVariableName($this->getAttribute())) {
            $this->setType(TokenType::T_LABEL);
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

function lexical_analysis() {
    # TODO - put whole lex. analysis into function
}

# prepare xml
$xml = new DOMDocument();
$xml->encoding = 'utf-8';
$xml->xmlVersion = '1.0';
$xml->formatOutput = true;

$root = $xml->appendChild($xml->createElement('program'));
$attr_language = new DOMAttr('language', 'IPPCode19');
$root->setAttributeNode($attr_language);
$order = 0;

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

            # Check for comment
            if ($token->getType() == TokenType::T_COMMENT) {
                # Ignore rest of line - break foreach loop
                break;
            }

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
        print(" " . $token->getAttribute());
    }
    print("\n");
    print("Num of tokens: " . count($token_array) . "\n");

    # TODO: check if token_array is not empty

    if ($line_number == 0) {
        # First line has to be header
        if (count($token_array) == 1 && $token_array[0]->getType() == TokenType::T_HEADER) {
            print("HEADER OK\n");

        }
        else {
            print("ERROR: HEADER NOT OK\n");
            $error_occurred = true;
            break;
        }
    }
    else {
        # Check if first token is valid opcode
        if (count($token_array) > 0) {
            if ($token_array[0]->getType() != TokenType::T_OPCODE) {
                print("ERROR: no opcode as first token\n");
                $error_occurred = true;
                break;
            }
            # Opcode ok, check for length of array
            #elseif (count($token_array) > 4) {
            #    print("Error: too many args\n");
            #    $error_occurred = true;
            #    break;
            #}
            # Check if correct number of params for opcode
            elseif (count($token_array) - 1 != Instruction::getNumberOfArgs($token_array[0]->getAttribute())) {
                print("Error: wrong number of args\n");
                $error_occurred = true;
                break;
            }
        }

    }


    # XML generating
    if (count($token_array) > 0 && $line_number != 0) {
        # Time to generate xml for instruction
        $instr = $root->appendChild($xml->createElement("instruction"));
        $attr_order = new DOMAttr('order', $order);
        $instr->setAttributeNode($attr_order);
        $attr_opcode = new DOMAttr('opcode', strtoupper($token_array[0]->getAttribute()));
        $instr->setAttributeNode($attr_opcode);

        $order += 1;
    }

    $line_number += 1;
}

if (!$error_occurred) {
    print("Program analysis without error.\n");
}
else {
    print("Program analysis with error.\n");
}

# output xml
print("XML output:\n");
echo $xml->saveXML();
print("End of output\n");
?>

