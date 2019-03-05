<?php
# VUT FIT - IPP - Project
# Author: Natália Holková (xholko02)

define("ERR_SCRIPT_PARAMS", 10); # Error - missing script param or usage of forbidden combination of params
define("ERR_INPUT_FILES", 11); # Error - opening input files
define("ERR_OUTPUT_FILES", 12); # Error - opening output files
define("ERR_INTERNAL", 99); # Internal error
define("ERR_OK", 0); # No error

/**
 * Class Parameters - stores number of times parameter was selected and value after them
 */
class Parameters {
    private $options; /** @var dictionary with number of time parameter was in cmd line args */
    private $parseFile;
    private $intFile;
    private $directory;

    /**
     * Parameters constructor. Sets implicit values
     */
    function __construct() {
        $this->parseFile = "parse.php";
        $this->intFile = "interpret.py";
        $this->directory = "./";

        $this->options = array(
            "help" => 0,
            "directory" => 0,
            "recursive" => 0,
            "parse-script" => 0,
            "int-script" => 0,
            "parse-only" => 0,
            "int-only" => 0
        );
    }

    function setParseFile($filename) {
        $this->parseFile = $filename;
    }

    function setIntFile($filename) {
        $this->intFile = $filename;
    }

    function setDirectory($dirname) {
        $this->directory = $dirname;
    }

    function incOption($option) {
        $this->options[$option] += 1;
    }
}

/**
 * Display help
 */
function display_help() {
    echo <<<EOL
test.php is script for automatic testing of programs parse.php and interpret.py.

Usage: php7.3 test.php [options]
           
Arguments:
    --help                 display help
    --directory=path       search for tests in specified directory (if missing this parameter, uses current directory)
    --recursive            will search recursively for tests
    --parse-script=file    file with script in PHP 7.3 for analysis of source code in IPPcode19,
                           if missing, implicit value is parse.php in current directory
    --int-script=file      file with script in Python 3.6 for interpret of XML representation of code in IPPcode19,
                           if missing, implicit value is interpet.py in current directory
    --parse-only           only script for analysis will be tested (do not combine with --int-script)
    --int-only             only script for interpret will be teste (do not combine with --parse-scritp)

EOL;
}

/**
 * Function to check program arguments. Notes number of occurence of params and set values.
 * @param $params Object of class Parameters
 * @return int Error code
 */
function check_program_arguments($params) {
    global $argv;
    global $argc;
    for ($i = 1; $i < $argc; $i++) {
        $arg = $argv[$i];
        switch ($arg) {
            case "--help":
                $params->incOption("help");
                break;
            case "--recursive":
                $params->incOption("recursive");
                break;
            case "--parse-only":
                $params->incOption("parse-only");
                break;
            case "--int-only":
                $params->incOption("int-only");
                break;
            default:
                // Use regex to search for params: directory, parse-script, int-script
                break;
        }
    }

    return ERR_OK;
}

# Main body

$params = new Parameters();
check_program_arguments($params);

?>