<?php
echo "IPP Project 2018/2019\n";

while ($line = fgets(STDIN)) { # Split input into lines
    echo "line: " . $line;
    $line = trim($line);
    foreach (preg_split("/\s+/", $line) as $word) { # Split line into words
        echo "\tword: " . $word . "\n";
    }
}

?>

