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
        foreach ($this->options as $key => $value) {
            if ($value > 1) {
                error_log("Error! Some of parameters were input multiple times.");
                return false;
            }
        }

        // Check if --help option is with anything else
        if ($this->options["help"] == 1) {
            foreach ($this->options as $key => $value)
                if ($key != "help" && $value > 0) {
                    error_log("Error! Cannot have --help with additional parameters.");
                    return false;
                }
        }

        // Check if --parse-script with --int-only
        if ($this->options["parse-script"] == 1 && $this->options["int-only"] == 1) {
            error_log("Error! Cannot have --parse-script and --int-only parameters at the same time.");
            return false;
        }

        // Check if --int-script with --parse-only
        if ($this->options["int-script"] == 1 && $this->options["parse-only"] == 1) {
            error_log("Error! Cannot have --int-script and --parse-only parameters at the same time.");
            return false;
        }

        return true; // No error - OK
    }

    /**
     * Decide what actions to take based on selected options.
     * Expects that method checkProgramArguments and checkOptionsWithoutConflicts were called prior.
     */
    function decideCourseOfAction() {
        if ($this->options["help"] == 1) {
            display_help();
        }
        elseif ($this->options["parse-only"] == 1) {

        }
        elseif ($this->options["int-only"] == 1) {

        }
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
 * @param $path Full path to file
 * @param $content Content to put in file
 * @return int Error ok - ERR_OK or ERR_OUTPUT_FILES
 */
function generate_file($path, $content) {
    $handle = fopen($path, "w");
    if ($handle == false) {
        error_log("Error! Could not create " . $path . ".\n");
        return ERR_OUTPUT_FILES;
    }
    print("I am going to write in file " . $path . " now.\n");

    fwrite($handle, $content);

    fclose($handle);
    return ERR_OK;
}

/**
 * Automatic test only for parse, non-recursive option.
 * @param $directory Directory with test
 * @param $parseFile Filename of parse script
 * @return int Error code
 */
function test_parse_non_recursive($directory, $parseFile) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_file($directory . "/" . $file) && (preg_match("/.src/", $file))) {

            $name = substr($file, 0, strlen($file) - 4);
            print("-- filename without .src: " . $name . "\n");

            // Check if it has .rc file
            if (!is_file($directory . "/" . $name . ".rc")) {
                print("--- also does not have .rc file Generate it!\n");
                generate_file($directory . "/" . $name . ".rc", "0\n");
            }
            // Check if it has .out file
            if (!is_file($directory . "/" . $name . ".out")) {
                print("--- also does not have .out file Generate it!\n");
                generate_file($directory . "/" . $name . ".out", "");
            }
            // Check if it has .in file
            if (!is_file($directory . "/" . $name . ".in")) {
                print("--- also does not have .in file Generate it!\n");
                generate_file($directory . "/" . $name . ".in", "");
            }

            // We now have all necessary file - do the test!
            // Run parse.php (or whatever it is called) with current source file and save output to .my_out and return code to .my_rc
            exec("php7.3 " . $parseFile . " < " . ($directory . "/" . $file) . " > " . ($directory . "/" . $name . ".my_out") );
            exec("echo $? > " . $directory . "/" . $name . ".my_rc");

            // Compare return codes
            // TODO - diff nech ignoruje prázdne riadky
            $rc_diff = shell_exec("diff -w " . $directory . "/" . $name . ".my_rc " . $directory . "/" . $name . ".rc");
            //print("diff --strip-trailing-cr " . $directory . "/" . $name . ".my_rc " . $directory . "/" . $name . ".rc\n");
            print("rc diff: " . $rc_diff . "\n");
            // Compare outputs
            //
        }
    }

    return ERR_OK;
}

# Main body

/*
$params = new Parameters();

// Check provided arguments
if ($params->checkProgramArguments() != ERR_OK)
    exit(ERR_SCRIPT_PARAMS);
// Check for conflicts between parameters
if(!$params->checkOptionsWithoutConflicts())
    exit(ERR_SCRIPT_PARAMS);

$params->decideCourseOfAction();
*/

$parseFile = "parse.php";
$directory = "mytests";

// TODO - return error code if directory does not exist

if (is_file($parseFile))
    print("file exists\n");
else
    print("file does not exit\n");

if (is_dir($directory ))
    print("is directory\n");
else
    print("isn't directory\n");

/*
$current_path = $directory;

$files = scandir($directory);
var_dump($files);
foreach ($files as $file) {
    if (is_file($current_path . "/" . $file) && (preg_match("/.src/", $file))) {
        print("- is src file: " . $current_path . "/" . $file . "\n");

        $name = substr($file, 0, strlen($file) - 4);
        print("-- filename without .src: " . $name . "\n");
        // Check if it has .rc file
        if (is_file($current_path . "/" . $name . ".out"))
            print("--- also has .out file.\n");
    }
}*/

test_parse_non_recursive($directory, $parseFile);

?>