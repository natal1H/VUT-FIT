# VUT FIT - IPP - Project
# Author: Natália Holková (xholko02)

import sys
import xml.etree.ElementTree as ET

# Temp - variables with exit codes
ERR_SCRIPT_PARAMS = 10  # Missing or wrong script parameter
ERR_INPUT_FILES = 11  # Error with opening input files
ERR_OUTPUT_FILES = 12  # Error with opening output files
ERR_INTERNAL = 99  # Internal error
ERR_XML_FORMAT = 31  # Wrong XML format
ERR_XML_STRUCTURE = 32  # Wrong XML structure - lexical or syntax error

def exit_with_message(message, code):
    print(message, file=sys.stderr)
    sys.exit(code)

def is_opcode(opcode):
    opcodes = [
        "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN",
        # Work with frames, calling functions
        "PUSHS", "POPS",  # Work with data stack
        "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "NOT", "INT2CHAR", "STRI2INT",
        # Arithmetic, relational, boolean and conversion instructions
        "READ", "WRITE",  # I/O instructions
        "CONCAT", "STRLEN", "GETCHAR", "SETCHAR",  # Working with strings
        "TYPE",  # Working with types
        "LABEL", "JUMP", "JUMPIFEQ", "JUMPIFNEQ", "EXIT",  # Controlling program flow
        "DPRINT", "BREAK"  # Debugging
    ]

    if opcode in opcodes:
        return True
    else:
        return False

def get_xml_from_file(filename):
    file = open(filename, "r")
    xml_string = ""
    for line in file:
        xml_string += line

    try:
        xml = ET.fromstring(xml_string)
    except:
        exit_with_message("Error! Wrong XML format.", ERR_XML_FORMAT)

    return xml

def get_xml_from_stdin():
    file = sys.stdin
    xml_string = ""
    for line in file.readlines():
        xml_string += line

    try:
        xml = ET.fromstring(xml_string)
    except:
        exit_with_message("Error! Wrong XML format.", ERR_XML_FORMAT)

    return xml

# MAIN
source_filename = "example.xml"
input_filename = "input.txt"

#root = get_xml_from_file(source_filename)
root = get_xml_from_stdin()

for instruction in root:
    print(instruction.attrib["opcode"])
    if not is_opcode(instruction.attrib["opcode"]):
        exit_with_message("Error! Invalid OPCODE in XML.", ERR_XML_STRUCTURE)

