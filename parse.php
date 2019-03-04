<?php
# VUT FIT - IPP - Project
# Author: Natália Holková (xholko02)



# Define error constants
define("ERR_SCRIPT_PARAMS", 10); # Error - missing script param or usage of forbidden combination of params
define("ERR_INPUT_FILES", 11); # Error - opening input files
define("ERR_OUTPUT_FILES", 12); # Error - opening output files
define("ERR_HEADER", 21); # Error - missing or wrong header
define("ERR_OPCODE", 22); # Error - unknown or wrong opcode
define("ERR_LEX_OR_SYNTAX", 23); # Error - other lexical or syntax error
define("ERR_INTERNAL", 99); # Internal error

$params = array(
  "help" => false,
  "stats" => false,
  "loc" => false,
  "comments" => false,
  "labels" => false,
  "jumps" => false
);

/**
 * Class for keeping track of statistics such as lines of code, lines with comments, jumps or labels
 */
class Statistic {
    private $stats_order;
    private $stats_num;
    private $file;
    private $labels;

    /**
     * Statistic constructor - sets all stats to 0
     */
    function  __construct() {
        $this->stats_order = array();
        $this->stats_num = $stats_num = array(
            "loc" => 0,
            "comments" => 0,
            "labels" => 0,
            "jumps" => 0
        );
        $this->file = "";
        $this->labels = array();
        $this->jumps = array();
    }

    /**
     * Sets name of file where output will be stored
     * @param $file Name of file
     */
    function setFile($file) {
        $this->file = $file;
    }

    /**
     * Adds another statistics, which will be printed to output file
     * @param $str name of statistic
     */
    function appendStatOrder($str) {
        $this->stats_order[] = $str;
    }

    /**
     * Increase number of lines of code
     */
    function incLoc() {
        $this->stats_num["loc"]++;
    }

    /**
     * Increase number of lines with comments
     */
    function incComments() {
        $this->stats_num["comments"]++;
    }

    /**
     * Try to increase number of unique labels
     * @param $label Label name
     */
    function tryIncLabels($label) {
        if (!in_array($label, $this->labels)) {
            $this->labels[] = $label;
            $this->stats_num["labels"]++;
        }
    }

    /**
     * Increase number of jumps
     */
    function incJumps() {
        $this->stats_num["jumps"]++;
    }

    /**
     * Ouput desired stats to file
     */
    function outputStats() {
        # Try to open file for output
        $handle = fopen($this->file, "w");
        if (!$handle) # Error - opening file failed
            exit(ERR_OUTPUT_FILES);

        foreach ($this->stats_order as $stat)
            fwrite($handle, $this->stats_num[$stat] . "\n");

        fclose($handle);
    }
}

/**
 * Class for working with instructions
 */
class Instruction {

    /**
     * Determine type of argument
     * @param $token_type token type
     * @return string Type of argument in string format
     */
    static function getArgType($token_type) {
        if ($token_type == TokenType::T_NIL)
            return "nil";
        elseif ($token_type == TokenType::T_VARIABLE)
            return "var";
        elseif ($token_type == TokenType::T_INT)
            return "int";
        elseif ($token_type == TokenType::T_BOOL)
            return "bool";
        elseif ($token_type == TokenType::T_STRING)
            return "string";
        elseif ($token_type == TokenType::T_LABEL)
            return "label";
        elseif ($token_type == TokenType::T_TYPE)
            return "type";
        else
            return "unknown";
    }

    /**
     * Get argument value, strip of prefix if necessary
     * @param $token_atr Token attribute
     * @param $token_type Type of token
     * @return bool|string New argument attribute
     */
    static function getArgValue($token_atr, $token_type) {
        if ($token_type == TokenType::T_NIL || $token_type == TokenType::T_STRING || $token_type == TokenType::T_BOOL || $token_type == TokenType::T_INT)
            return Token::stripPrefix($token_atr);
        else
            return $token_atr;
    }

    /**
     * Check if it is valid opcode for instruction
     * @param $str String
     * @return bool Is or isn't opcode
     */
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

    /**
     * Determine number of arguments each instruction should have
     * @param $opcode Instruction code
     * @return int Number of arguments
     */
    static function getNumberOfArgs($opcode) {
        $no_args = array(
          "CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"
        );
        $one_arg = array(
          "DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"
        );
        $two_args = array(
            "MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"
        );
        $three_args = array(
            "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"
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

/**
 * Various token type constants
 */
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
    const T_TYPE = 9;
    const T_UNKNOWN = 10;
}

/**
 * Class representing single token
 */
class Token {
    private $type;
    private $attribute;
    private $valid;

    /**
     * Get token type
     * @return token type
     */
    function getType() {
        return $this->type;
    }

    /**
     * Get token attribute
     * @return token attribute
     */
    function getAttribute() {
        return $this->attribute;
    }

    /**
     * Get token validity
     * @return bool Is token valid?
     */
    function getValidity() {
        return $this->valid;
    }

    /**
     * Token constructor.
     * @param $str Token attribute
     */
    function __construct($str) {
        # Set attribute and default type
        $this->attribute = $str;
        $this->type = TokenType::T_UNKNOWN;
        $this->valid = true; # true until proven false
    }

    /**
     * Determine and set type of token based on attribute
     * @param $opcode_before Was token before opcode?
     */
    function determine_type($opcode_before) {
        # Will set actual type based on attribute

        # Case: Header
        if (strtolower($this->attribute) == ".ippcode19") {
            $this->type = TokenType::T_HEADER;
        }
        # Case: comment
        elseif (preg_match("/^#/", $this->attribute)) {
            $this->type = TokenType::T_COMMENT;
        }
        # Case: variable
        elseif (preg_match("/^GF@/", $this->attribute) or preg_match("/^LF@/", $this->attribute) or preg_match("/^TF@/", $this->attribute)) {
            if ($this->checkVariableName(Token::stripPrefix($this->attribute))) # Valid var format -> set type to T_VARIABLE
                $this->type = TokenType::T_VARIABLE;
            else # Invalid var format -> set validity to false
                $this->valid = false;
        }
        # Case: bool constant
        elseif (preg_match("/^bool@/", $this->attribute)) {
            if ($this->checkBoolConstant(Token::stripPrefix($this->attribute))) # valid bool format -> set type to T_BOOL
                $this->type =TokenType::T_BOOL;
            else # invalid bool type -> set token validity to false
                $this->valid = false;
        }
        # Case: int constant
        elseif (preg_match("/^int@/", $this->attribute)) {
            if ($this->checkIntConstant(Token::stripPrefix($this->attribute))) # valid int format -> set type to T_INT
                $this->type =  Tokentype::T_INT;
            else # invalid int format -> set validity to false
                $this->valid = false;
        }
        # Case: string constant
        elseif (preg_match("/^string@/", $this->attribute)) {
            if ($this->checkStringConstant(Token::stripPrefix($this->attribute))) # valid string format -> set type to T_STRING
                $this->type = TokenType::T_STRING;
            else # invalid string format -> set validity to false
                $this->valid = false;
        }
        # Case: nil constant
        elseif ($this->attribute == "nil@nil") { # set type to T_NIL
            $this->type = TokenType::T_NIL;
        }
        # Case: opcode
        #elseif (in_array(strtoupper($this->getAttribute()), $opcodes)) { # Set type to T_OPCODE
        elseif (Instruction::isOpcode($this->attribute)) { # Set type to T_OPCODE
            if ($opcode_before) { # If opcode was before, this could by label (e.g LABEL label
                $this->type = TokenType::T_LABEL;
            }
            else {
                $this->type= TokenType::T_OPCODE;
            }
        }
        # Case: type
        elseif (in_array($this->attribute, array("int", "string", "bool"))) {
            $this->type = TokenType::T_TYPE;
        }
        # Case: label
        elseif ($this->checkVariableName($this->attribute)) {
            $this->type = TokenType::T_LABEL;
        }
        # Case: lex error
        else { # Set validity to false
            $this->valid = false;
        }

    }

    /**
     * Strip string of prefix
     * @return stripped string
     */
    static function stripPrefix($word) {
        return substr($word, strpos($word, '@') + 1);
    }

    /**
     * Check if string could be variable name
     * @param $name Name
     * @return bool Can be variable name?
     */
    function checkVariableName($name) {
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

    /**
     * Check if string could be number
     * @param $number String representing number
     * @return bool Could be number?
     */
    function checkIntConstant($number) {
        return preg_match('/^[\+\-]?[0-9]+$/', $number);
    }

    /**
     * Check if string could be boolean
     * @param $word String representing boolean
     * @return bool Could be boolean?
     */
    function checkBoolConstant($word) {
        return preg_match('/true|false/', $word);
    }

    /**
     * Check if could be string constant
     * @param $str String
     * @return bool Could be string constant
     */
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

            $last_position += 1; # Update last_position
        }

        return true;
    }

}

/**
 * Escape string for xml generation
 * @param $str String
 * @return Escaped string
 */
function escape_string_for_xml($str) {
    $str = str_replace("&", "&amp;", $str); # Replace &
    $str = str_replace("<", "&lt;", $str); # Replace <
    $str = str_replace(">", "&gt;", $str); # Replace >

    return $str;
}

/**
 * Performs lexical analysis on single line
 * @param $line Line with instruction
 * @param $line_number Line number
 * @param $stat Reference to object with statistics
 * @return array Array of Tokens
 */
function line_lexical_analysis($line, $line_number, &$stat) {
    # Array for tokens
    $token_array = [];

    $opcode_before = false;

    foreach (preg_split("/[\s\t]+/", $line) as $word) { # Split line into words
        #echo "Word: " . $word . "\n";
        if (strlen($word) == 0) { # \n character
            break; # Break foreach - begin syntax analysis with acquired tokens
        }
        else {

            if (preg_match("/#/", $word) && strlen($word) > 1) {
                # Contains # - is beginning of line comment
                $word = substr($word, 0, strpos($word, "#"));

                # Create token and determine its type
                $token = new Token($word);
                $token->determine_type($opcode_before);

                if ($token->getValidity()) {
                    $token_array[] = $token;
                    $opcode_before = ($token->getType() == TokenType::T_OPCODE);
                }
                elseif ($line_number != 0) {
                    exit(ERR_LEX_OR_SYNTAX);
                }

                break; # Comment would follow, so break
            }
            else {
                # Create token and determine its type
                $token = new Token($word);
                $token->determine_type($opcode_before);

                # Check for comment
                if ($token->getType() == TokenType::T_COMMENT) {
                    $stat->incComments();
                    break; # Ignore rest of line - break foreach loop
                }

                if ($token->getValidity()) {
                    $token_array[] = $token;
                    $opcode_before = ($token->getType() == TokenType::T_OPCODE);
                }
                elseif ($line_number != 0) {
                    exit(ERR_LEX_OR_SYNTAX);
                }
            }
        }
    }
    return $token_array;
}

/**
 * Performs syntax analysis on line
 * @param $token_array Array with tokens
 * @param $line_number Line number
 */
function line_syntax_analysis($token_array, $line_number) {
    if ($line_number == 0) {
        # First line has to be header
        if (!(count($token_array) == 1 && $token_array[0]->getType() == TokenType::T_HEADER)) {
            exit(ERR_HEADER);
        }
    }
    else {
        # Check if first token is valid opcode
        if (count($token_array) > 0) {
            if ($token_array[0]->getType() != TokenType::T_OPCODE) {
                exit(ERR_OPCODE);
            }
            elseif (count($token_array) - 1 != Instruction::getNumberOfArgs(strtoupper($token_array[0]->getAttribute()))) {
                exit(ERR_LEX_OR_SYNTAX);
            }
        }

    }
}

/**
 * Generate xml for line with instruction
 * @param $xml Base xml
 * @param $root Root
 * @param $token_array Token array
 * @param $order Order of instruction
 * @param $stat Object with statistics
 */
function line_generate_xml($xml, $root, $token_array, $order, &$stat) {
    # Time to generate xml for instruction
    $instr = $root->appendChild($xml->createElement("instruction"));
    $attr_order = new DOMAttr('order', $order);
    $instr->setAttributeNode($attr_order);
    $opcode = strtoupper($token_array[0]->getAttribute());
    $attr_opcode = new DOMAttr('opcode', $opcode);
    $instr->setAttributeNode($attr_opcode);

    # Generate xml for args
    for ($i = 1; $i < count($token_array); $i++) {
        if ($opcode == "LABEL" && $i == 1)
            $stat->tryIncLabels($token_array[$i]);

        $type = Instruction::getArgType($token_array[$i]->getType());
        if ($type == "unknown")
            exit(ERR_LEX_OR_SYNTAX); # Error - wrong type of argument

        $value = escape_string_for_xml(Instruction::getArgValue($token_array[$i]->getAttribute(), $token_array[$i]->getType()));
        $arg = $instr->appendChild($xml->createElement("arg" . $i, $value));
        $attr_arg_type = new DOMAttr('type', $type);
        $arg->setAttributeNode($attr_arg_type);
    }

    # Statistic - increase number of lines of code
    $stat->incLoc();
    # Statistic - increasing number of jumps
    if (in_array($opcode, array("JUMP", "JUMPIFEQ", "JUMPIFNEQ")))
        $stat->incJumps();
}

/**
 * Parse command line args
 * @param $stat Object where statistics will be stored
 */
function parse(&$stat) {

    # prepare xml
    $xml = new DOMDocument();
    $xml->encoding = 'utf-8';
    $xml->xmlVersion = '1.0';
    $xml->formatOutput = true;

    $root = $xml->appendChild($xml->createElement('program'));
    $attr_language = new DOMAttr('language', 'IPPcode19');
    $root->setAttributeNode($attr_language);
    $order = 1;

    $line_number = 0;

    # main parsing loop
    while ($line = fgets(STDIN)) { # Split input into lines

        # Lexical analysis of line
        $token_array = line_lexical_analysis($line, $line_number,$stat);

        # Syntax analysis of line
        line_syntax_analysis($token_array, $line_number);

        # XML generating

        if (count($token_array) > 0 && $line_number != 0) {
            line_generate_xml($xml, $root, $token_array, $order,$stat);

            $order += 1;
        }

        $line_number += 1;
    }
    if ($line_number == 0)
        exit(ERR_HEADER);


    # output xml
    echo $xml->saveXML();
}

/**
 * Display help
 */
function display_help() {
    echo <<<EOL
parse.php is script of type filter (written in PHP 7.3), which reads from stdin source code in IPPcode19 language.
It then performs lexical and syntax analysis to check code validity and afterwards outputs XML representation of program according to specification.

Usage: php7.3 parse.php [options]
           - user manually has to type input (not recommended)
    or php7.3 parse.php [options] < input_file
           - program reads input from specified file
    or php7.3 parse.php [options] [< input_file] > output_file
           - program can store output to specified file
           
Arguments (write in place of [options]):
    --help              display help, no additional argument can be called
    --stats=filename    collect statistics about source code and store them to specified file
    --loc               count lines of code (--stat=... has to be called before)
    --comments          count lines of code with comments (--stat=... has to be called before)
    --jumps             count number of jumps (--stat=... has to be called before)
    --labels            count number of unique labels (--stat=... has to be called before)
    
Return codes:
    0    OK
    10   Problem with script parameters
    11   Problem with opening input files
    12   Problem with opening output files
    21   Missing or wrong header
    22   Unknown or wrong opcode
    23   Other lexical or syntax error
    99   Internal error\n
EOL;

}

function check_program_arguments(&$params, &$stat) {
    global $argv;
    global $argc;

    for ($i = 1; $i < $argc; $i++) {
        $arg = $argv[$i];
        switch ($arg) {
            case "--help":
                if ($argc == 2) # Only --help param, no problem
                    $params["help"] = true;
                else # Error - --help param cannot be with other params
                    exit(ERR_SCRIPT_PARAMS);
                break;
            case "--loc":
                if ($params["stats"] == true && $params["loc"] == false) {
                    $params["loc"] = true;
                    $stat->appendStatOrder("loc");
                }
                else
                    exit(ERR_SCRIPT_PARAMS);
                break;
            case "--comments":
                if ($params["stats"] == true && $params["comments"] == false) {
                    $params["comments"] = true;
                    $stat->appendStatOrder("comments");
                }
                else
                    exit(ERR_SCRIPT_PARAMS);
                break;
            case "--labels":
                if ($params["stats"] == true && $params["labels"] == false) {
                    $params["labels"] = true;
                    $stat->appendStatOrder("labels");
                }
                else
                    exit(ERR_SCRIPT_PARAMS);
                break;
            case "--jumps":
                if ($params["stats"] == true && $params["jumps"] == false) {
                    $params["jumps"] = true;
                    $stat->appendStatOrder("jumps");
                }
                else
                    exit(ERR_SCRIPT_PARAMS);
                break;
            default:
                if (preg_match('/--stats=[a-zA-Z0-9_\.]+/', $arg) && $params["stats"] == false) {
                    $params["stats"] = true;
                    $stat->setFile(substr($arg,strpos($arg, "=") + 1));
                }
                else { # Error - unknown param
                    exit(ERR_SCRIPT_PARAMS);
                }
                break;
        }
    }
}

# Main body
$stat = new Statistic();

# Check script parameters
check_program_arguments($params, $stat);

# Decide what to do - display help or parse code
if ($params["help"]) {
    display_help();
}
else {
    parse($stat);

    if ($params["stats"])
        $stat->outputStats();
}
?>