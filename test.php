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

    /**
     * Sets filename of script for analysis
     * @param $filename New filename
     */
    function setParseFile($filename) {
        $this->parseFile = $filename;
    }

    /**
     * Sets filename of script for interpret
     * @param $filename New filename
     */
    function setIntFile($filename) {
        $this->intFile = $filename;
    }

    /**
     * Sets directory where tests will be located
     * @param $dirname New directory
     */
    function setDirectory($dirname) {
        $this->directory = $dirname;
    }

    /**
     * Increase number of times option was input
     * @param $option Name of option
     */
    function incOption($option) {
        $this->options[$option] += 1;
    }

    /**
     * Function to check program arguments. Notes number of occurence of params and set values.
     * @return int Error code - ERR_OK or ERR_SCRIPT_PARAMS
     */
    function checkProgramArguments() {
        global $argv;
        global $argc;
        for ($i = 1; $i < $argc; $i++) {
            $arg = $argv[$i];
            switch ($arg) {
                case "--help":
                    $this->incOption("help");
                    break;
                case "--recursive":
                    $this->incOption("recursive");
                    break;
                case "--parse-only":
                    $this->incOption("parse-only");
                    break;
                case "--int-only":
                    $this->incOption("int-only");
                    break;
                default:
                    // Use regex to search for params: directory, parse-script, int-script

                    if (preg_match('/--directory=/', $arg)) { // --directory=path option
                        $this->incOption("directory");
                        $this->directory = substr($arg, strpos($arg, "=") + 1);
                    }
                    elseif (preg_match('/--parse-script=/', $arg)) { // --parse-script=file option
                        $this->incOption("parse-script");
                        $this->parseFile = substr($arg, strpos($arg, "=") + 1);
                    }
                    elseif (preg_match('/--int-script=/', $arg)) { // --int-script=file option
                        $this->incOption("int-script");
                        $this->intFile = substr($arg, strpos($arg, "=") + 1);
                    }
                    else { // Error, unknown
                        error_log("Error! Unknown parameter " . $arg);
                        return ERR_SCRIPT_PARAMS;
                    }
                    break;
            }
        }

        return ERR_OK;
    }

    /**
     * Check options for multiple occurences of option or conflicts between them
     * @return boolean true if no conflicts between parameters
     */
    function checkOptionsWithoutConflicts() {
        // Check if any option is > 1

        // Check if --help option is with anything else

        // Check if --parse-script with --int-only

        // Check if --int-script with --parse-only

        return true;
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

# Main body

$params = new Parameters();

// Check provided arguments
if ($params->checkProgramArguments() != ERR_OK)
    exit(ERR_SCRIPT_PARAMS);

var_dump($params);
?>