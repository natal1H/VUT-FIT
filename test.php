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
    private $jexamxml;
    private $jexamxmlOptions;

    /**
     * Parameters constructor. Sets implicit values
     */
    function __construct() {
        $this->parseFile = "parse.php";
        $this->intFile = "interpret.py";
        $this->directory = "./";
        $this->jexamxml = realpath("/pub/courses/ipp/jexamxml/jexamxml.jar");
        $this->jexamxmlOptions = realpath("/pub/courses/ipp/jexamxml/options");

        $this->options = array(
            "help" => 0,
            "directory" => 0,
            "recursive" => 0,
            "parse-script" => 0,
            "int-script" => 0,
            "parse-only" => 0,
            "int-only" => 0,
            "xml" => 0
        );
    }

    public function getDirectory() {
        return $this->directory;
    }

    public function getParseFile() {
        return $this->parseFile;
    }

    public function getIntFile() {
        return $this->intFile;
    }

    /**
     * @return string
     */
    public function getJexamxml() {
        return $this->jexamxml;
    }

    /**
     * @return string
     */
    public function getJexamxmlOptions() {
        return $this->jexamxmlOptions;
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
                        // add / at the end of directory if not there already
                        if ($this->directory[strlen($this->directory) - 1] != "/") $this->directory .= "/";

                        // Test input directory
                        if (!$this->testDirValidity($this->directory)) {
                            error_log("Error! Directory does not exist.");
                            return ERR_INPUT_FILES;
                        }  
                    }
                    elseif (preg_match('/--parse-script=/', $arg)) { // --parse-script=file option
                        $this->incOption("parse-script");
                        $this->parseFile = substr($arg, strpos($arg, "=") + 1);
                        // Test input parse file
                        if (!$this->testFileValidity($this->parseFile)) {
                            error_log("Error! " . $this->parseFile . " does not exist.");
                            return ERR_INPUT_FILES;
                        }
                    }
                    elseif (preg_match('/--int-script=/', $arg)) { // --int-script=file option
                        $this->incOption("int-script");
                        $this->intFile = substr($arg, strpos($arg, "=") + 1);
                        // Test input interpret file
                        if (!$this->testFileValidity($this->intFile)) {
                            error_log("Error! " . $this->intFile . " does not exist.");
                            return ERR_INPUT_FILES;
                        }
                    }
                    elseif (preg_match('/--xml=/', $arg)) { // --xml= directory where jexamxml.jar is stored
                        $this->incOption("xml");
                        $this->jexamxml = substr($arg, strpos($arg, "=") + 1);
                        if ($this->jexamxml[strlen($this->jexamxml) - 1] != "/") $this->jexamxml .= "/";

                        // Test input directory
                        if (!$this->testDirValidity($this->jexamxml)) {
                            error_log("Error! Directory for JExamXML does not exist.");
                            return ERR_INPUT_FILES;
                        }

                        $this->jexamxmlOptions = realpath($this->jexamxml . "options");
                        $this->jexamxml .= "jexamxml.jar";
			$this->jexamxml = realpath($this->jexamxml);
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
     * Test if filename is really file
     * @param $filename Path to file
     * @return bool Is valid file?
     */
    function testFileValidity($filename) {
        return (is_file($filename));
    }

    /**
     * Test if directory name is really directory
     * @param $dirname Path to directory
     * @return bool Is valid directory?
     */
    function testDirValidity($dirname) {
        return (is_dir($dirname));
    }

    /**
     * Decide what actions to take based on selected options.
     * Expects that method checkProgramArguments and checkOptionsWithoutConflicts were called prior.
     * @return int Error code
     */
    function decideCourseOfAction($stat) {
        if ($this->options["help"] == 1) {
            display_help();
            exit(ERR_OK);
        }
        elseif ($this->options["parse-only"] == 1) {
            $stat->generateParseBeginning();
            if ($this->options["recursive"] == 0)
                // Non-recursive test only for parse
                test_parse_non_recursive($this->directory, $this->parseFile, $this->jexamxml, $this->jexamxmlOptions, $stat);
            else
                // Recursive test only for parse
                test_parse_recursive($this->directory, $this->parseFile, $this->jexamxml, $this->jexamxmlOptions, $stat);
            $stat->generateTestEnd();
            $stat->totalAndPassed();
        }
        elseif ($this->options["int-only"] == 1) {
            $stat->generateIntBeginning();
            if ($this->options["recursive"] == 0)
                // Non-recursive test only for parse
                test_interpret_non_recursive($this->directory, $this->intFile, $stat);
            else
                // Recursive test only for parse
                test_interpret_recursive($this->directory, $this->intFile, $stat);
            $stat->generateTestEnd();
            $stat->totalAndPassed();
        }
        else {
            $stat->generateParseBeginning();
            if ($this->options["recursive"] == 0)
                // Non-recursive test only for parse
                test_both_non_recursive($this->directory, $this->parseFile, $this->intFile, $stat);
            else
                // Recursive test only for parse
                test_both_recursive($this->directory, $this->parseFile, $this->intFile, $stat);
            $stat->generateTestEnd();
            $stat->totalAndPassed();
        }

        return ERR_OK;
    }
}

/**
 * Class HTMLStats - class for storing html output
 */
class HTMLStats {
    private $html;
    private $passed;
    private $total;

    /**
     * HTMLStats constructor
     */
    function __construct() {
        $this->html = "";
        $this->passed = 0;
        $this->total = 0;
    }

    /**
     * Generate header of html file
     */
    function generateHead($params) {
        $dir = $params->getDirectory();
        $parse = $params->getParseFile();
        $int = $params->getIntFile();
        $this->html .= <<<EOD
<html>
<head>
    <title>test.php</title>
    <style>
        body {font-family: arial; }
        table {background-color:  #f0f3f4 ; width: 100%}
        th {text-align: left; padding-top: 10px;}
        td {padding: 5px;}
        .okTest {background-color: #58d68d; }
        .errorTest {background-color:  #ec7063 ; }
        .errorDetail {background-color:  #f0f3f4; }
        .pane { width: 100%; display: inline-block; overflow-y: scroll; max-height: 500px; }
    </style>
</head>
<body>
<h1>test.php - results</h1>
<p>
Selected settings:
<ul>
    <li>Directory with tests: <b>$dir</b></li>
    <li>Path to script for analysis: <b>$parse</b></li>
    <li>Path to interpret script: <b>$int</b></li>
</ul>
</p>

EOD;
    }

    /**
     * Generate end of html file
     */
    function generateEnd() {
        $this->html .= <<<EOD
</body>
</html>

EOD;
    }

    function generateParseBeginning() {
        $this->html .= <<<EOD
<h2>Testing parse.php</h2>

<div class="pane">
<table>
    <tr>
        <th>#</th>
        <th>Test</th>
        <th>Result</th>
    </tr>

EOD;
    }

    function generateIntBeginning() {
        $this->html .= <<<EOD
<h2>Testing interpret.py</h2>

<div class="pane">
<table>
    <tr>
        <th>#</th>
        <th>Test</th>
        <th>Result</th>
    </tr>

EOD;
    }

    function generateErrorDetail($detail) {
        $this->html .= <<<EOD
<tr>
    <td></td>
    <td colspan="2" class="errorDetail">
        $detail
    </td>
</tr>

EOD;

    }

    function generateTestEnd() {
        $this->html .= "</table>\n</div>\n";
    }

    function testResult($path, $passed) {
        $result = ($passed == true) ? "OK" : "ERROR";
        $testClass = ($passed == true) ? "okTest" : "errorTest";
        $this->total += 1;
        if ($passed) $this->passed += 1;

        $this->html .= <<<EOD
<tr class="$testClass">
    <td>$this->total</td>
    <td>$path</td>
    <td>$result</td>
</tr>

EOD;

    }

    function totalAndPassed() {
        $this->html .= <<<EOD
<hr>
<p>
Total number of tests: <b>$this->total</b><br>
Passed tests: <b>$this->passed</b><br>
</p>

EOD;

    }

    /**
     * Output HTML to stdout
     */
    function outputHTML() {
        echo $this->html;
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
    --xml                  directory, in which can be jexamxml.jar and option be found.
                           You need to have permission to use it.

EOL;
}

/**
 * Generate file with content
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
    fwrite($handle, $content);

    fclose($handle);
    return ERR_OK;
}

function remove_tmp_file($path) {
    return unlink($path);
}

/**
 * Automatic test only for parse, non-recursive option.
 * @param $directory
 * @param $parseFile
 * @param $jexamxml
 * @param $jexamxmlOptions
 * @param @param $stat Object from class HTMLStats
 */
function test_parse_non_recursive($directory, $parseFile, $jexamxml, $jexamxmlOptions, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_file($directory . $file) && (preg_match("/.src/", $file))) {

            $name = substr($file, 0, strlen($file) - 4);
            $path = $directory . $name;

            // Check if it has .rc file
            if (!is_file($path . ".rc")) {
                generate_file($path . ".rc", "0\n");
            }
            // Check if it has .out file
            if (!is_file($path . ".out")) {
                generate_file($path . ".out", "");
            }
            // Check if it has .in file
            //if (!is_file($path . ".in")) {
            //    generate_file($path . ".in", "");
            //}

            // We now have all necessary file - do the test!
            // Run parse.php (or whatever it is called) with current source file and save output to .my_out
            exec("php7.3 " . $parseFile . " < " . ($directory . $file) . " > " . $path . ".my_out 2>/dev/null", $output, $my_rc);

            // Compare return codes
            $ref_rc = intval(file_get_contents($path . ".rc"));

            if ($ref_rc != $my_rc) {
                $stat->testResult($directory . $name, false);
                $detail = "Expected exit code " . $ref_rc . ", received " . $my_rc;
                $stat->generateErrorDetail($detail);
            }
            elseif ($my_rc == 0) {
                // Compare outputs
                exec("java -jar " . $jexamxml . " " . $path . ".out " . $path . ".my_out " . $directory . "diffs.xml /D " . $jexamxmlOptions, $out, $status);
		        //echo "java -jar " . $jexamxml . " " . $path . ".out " . $path . ".my_out " . $directory . "diffs.xml /D " . $jexamxmlOptions . "\n";
	
                if ($status != 0) {
                    $stat->testResult($directory . $name, false);
                    $detail = "XML output differs";
                    $stat->generateErrorDetail($detail);
                }
                else {
                    $stat->testResult($directory . $name, true);
                }
            }
            else {
                $stat->testResult($directory . $name, true);
            }

            // Remove tmp files
            if (file_exists($path . ".my_out"))
                remove_tmp_file($path . ".my_out"); // remove .my_out
            if (file_exists($directory . "diffs.xml"))
                remove_tmp_file($directory . "diffs.xml"); // remove diffs.xml
            if (file_exists($path . ".out.log"))
                remove_tmp_file($path . ".out.log"); // remove .log
        }
    }
}

function test_parse_recursive($directory, $parseFile, $jexamxml, $jexamxmlOptions, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_dir($directory . $file) && $file != "." && $file != "..") {
            // Is directory
            $currentDir = $directory . $file . "/";
            test_parse_recursive($currentDir, $parseFile, $jexamxml, $jexamxmlOptions, $stat);
        }
    }

    // Non-recursive test in main directory
    test_parse_non_recursive($directory, $parseFile, $jexamxml, $jexamxmlOptions, $stat);
}

function test_interpret_non_recursive($directory, $intFile, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_file($directory . $file) && (preg_match("/.src/", $file))) {

            $name = substr($file, 0, strlen($file) - 4);
            $path = $directory . $name;

            // Check if it has .rc file
            if (!is_file($path . ".rc")) {
                generate_file($path . ".rc", "0\n");
            }
            // Check if it has .out file
            if (!is_file($path . ".out")) {
                generate_file($path . ".out", "");
            }
            // Check if it has .in file
            if (!is_file($path . ".in")) {
                generate_file($path . ".in", "");
            }

            // We now have all necessary file - do the test!
            // Run interpret.py (or whatever it is called) with current source file and save output to .my_out
            exec("python3.6 " . $intFile . " --source=" . ($directory . $file) . " --input=" . $path .".in >" . $path . ".my_out 2>/dev/null", $output, $my_rc);

            // Compare return codes
            $ref_rc = intval(file_get_contents($path . ".rc"));

            if ($ref_rc != $my_rc) {
                $stat->testResult($directory . $name, false);
                $detail = "Expected exit code " . $ref_rc . ", received " . $my_rc;
                $stat->generateErrorDetail($detail);
            }
            elseif ($my_rc == 0) {
                // Compare outputs
                exec("diff " . $path . ".out " . $path . ".my_out", $out, $status);

                if ($status != 0) {
                    $stat->testResult($directory . $name, false);
                    $detail = "Output differs";
                    $stat->generateErrorDetail($detail);
                }
                else {
                    $stat->testResult($directory . $name, true);
                }
            }
            else {
                $stat->testResult($directory . $name, true);
            }

            // Remove tmp files
            if (file_exists($path . ".my_out"))
                remove_tmp_file($path . ".my_out"); // remove .my_out
            if (file_exists($path . ".out.log"))
                remove_tmp_file($path . ".out.log"); // remove .log
        }
    }
}

function test_interpret_recursive($directory, $intFile, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_dir($directory . $file) && $file != "." && $file != "..") {
            // Is directory
            $currentDir = $directory . $file . "/";
            test_interpret_recursive($currentDir, $intFile, $stat);
        }
    }

    // Non-recursive test in main directory
    test_interpret_non_recursive($directory, $intFile, $stat);
}

function test_both_non_recursive($directory, $parseFile, $intFile, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_file($directory . $file) && (preg_match("/.src/", $file))) {
            $name = substr($file, 0, strlen($file) - 4);
            $path = $directory . $name;

            // Check if it has .rc file
            if (!is_file($path . ".rc")) {
                generate_file($path . ".rc", "0\n");
            }
            // Check if it has .out file
            if (!is_file($path . ".out")) {
                generate_file($path . ".out", "");
            }
            // Check if it has .in file
            if (!is_file($path . ".in")) {
                generate_file($path . ".in", "");
            }

            // We now have all necessary file - do the test!
            // Run parse.php (or whatever it is called) with current source file and save output to .my_out
            exec("php7.3 " . $parseFile . " < " . ($directory . $file) . " > " . $path . ".my_xml 2>/dev/null", $output, $my_rc);

            $ref_rc = intval(file_get_contents($path . ".rc"));

            if ($my_rc == 0) {
                // Compare outputs

                // Run interpret.py (or whatever it is called) with current source file and save output to .my_out
                exec("python3.6 " . $intFile . " --source=" . ($path . ".my_xml") . " --input=" . $path .".in >" . $path . ".my_out 2>/dev/null", $output, $my_rc);

                if ($ref_rc != $my_rc) {
                    $stat->testResult($directory . $name, false);
                    $detail = "Expected exit code " . $ref_rc . ", received " . $my_rc;
                    $stat->generateErrorDetail($detail);
                }
                elseif ($my_rc == 0) {
                    // Compare outputs
                    exec("diff " . $path . ".out " . $path . ".my_out", $out, $status);
                    if ($status != 0) {
                        $stat->testResult($directory . $name, false);
                        $detail = "Output differs";
                        $stat->generateErrorDetail($detail);
                    }
                    else {
                        $stat->testResult($directory . $name, true);
                    }
                }
                else {
                    $stat->testResult($directory . $name, true);
                }
            }
            elseif (in_array($ref_rc, array(21, 22, 23)) && $ref_rc != $my_rc) {
                $stat->testResult($directory . $name, true);
            }

            // Remove tmp files
            if (file_exists($path . ".my_out"))
                remove_tmp_file($path . ".my_out"); // remove .my_out
            if (file_exists($path . ".my_xml"))
                remove_tmp_file($path . ".my_xml"); // remove .my_out
            if (file_exists($directory . "diffs.xml"))
                remove_tmp_file($directory . "diffs.xml"); // remove diffs.xml
            if (file_exists($path . ".out.log"))
                remove_tmp_file($path . ".out.log"); // remove .log
        }
    }
}

function test_both_recursive($directory, $parseFile, $intFile, $stat) {
    $files = scandir($directory);
    foreach ($files as $file) {
        if (is_dir($directory . $file) && $file != "." && $file != "..") {
            // Is directory
            $currentDir = $directory . $file . "/";
            test_both_non_recursive($currentDir, $parseFile, $intFile, $stat);
        }
    }

    // Non-recursive test in main directory
    test_both_non_recursive($directory, $parseFile, $intFile, $stat);
}

# Main body

$params = new Parameters();

// Check provided arguments
$ret_code = $params->checkProgramArguments();
if ($ret_code != ERR_OK)
    exit($ret_code);

// Check for conflicts between parameters
if(!$params->checkOptionsWithoutConflicts())
    exit(ERR_SCRIPT_PARAMS);

// Create object for HTML output
$stat = new HTMLStats();
$stat->generateHead($params);

// Decide course of action based on parameters
$ret_code = $params->decideCourseOfAction($stat);

$stat->generateEnd();
$stat->outputHTML();

exit($ret_code);
?>
