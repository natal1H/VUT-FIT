# VUT FIT - IPP - Project
# Author: Natália Holková (xholko02)

import sys
import xml.etree.ElementTree as ET
import re

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
    try:
        file = open(filename, "r")
    except FileNotFoundError:
        exit_with_message("Error! Could not open source file.", ERR_INPUT_FILES)

    xml_string = ""
    for line in file:
        xml_string += line

    file.close()

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

class Instruction:
    opcode = ""
    args = []

    def __init__(self, opcode):
        self.opcode = opcode

    def __repr__(self):
        return "<Instruction opcode:%s args: %s>" % (self.opcode, self.args)

    def getCorrectNumberOfArgs(self):
        no_args = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]
        one_arg = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"]
        two_args = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"]

        if self.opcode in no_args:
            return 0
        elif self.opcode in one_arg:
            return 1
        elif self.opcode in two_args:
            return 2
        else:
            return 3

    def setNumberOfArgs(self):
        self.args = [None] * self.getCorrectNumberOfArgs()

    def setArg(self, arg_num, arg_type, arg_value):
        # chage value from string to correct type if int or bool
        if arg_type == "int": arg_value = int(arg_value)
        elif arg_type == "bool": arg_value = True if arg_value == "true" else False

        self.args[arg_num - 1] = {"type": arg_type, "value": arg_value}

    def isSymb(self, arg_type):
        return arg_type == "var" or arg_type == "int" or arg_type == "bool" or arg_type == "string" or arg_type == "nil"

    def checkArgTypes(self):
        if len(self.args) == 1:
            # Instruction syntax: <var>
            if self.opcode in ["DEFVAR", "POPS"]:
                return self.args[0]["type"] == "var"

            # Instruction syntax: <label>
            elif self.opcode in ["CALL", "LABEL", "JUMP"]:
                return self.args[0]["type"] == "label"

            # Instruction syntax: <symb> (var | int | bool | string | nil)
            else:
                return self.isSymb(self.args[0]["type"])
        elif len(self.args) == 2:
            # Instruction syntax: <var> <symb>
            if self.opcode in ["MOVE", "INT2CHAR", "STRLEN", "TYPE", "NOT"]:
                return self.args[0]["type"] == "var" and self.isSymb(self.args[1]["type"])

            # Instruction syntax: <var> <type>
            else:
                return self.args[0]["type"] == "var" and self.args[1]["type"] == "type"

        elif len(self.args) == 3:
            # Instruction syntax: <label> <symb> <symb>
            if self.opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
                return self.args[0]["type"] == "label" and self.isSymb(self.args[1]["type"]) and self.isSymb(self.args[2]["type"])

            # Instruction syntax: <var> <symb> <symb>
            else:
                return self.args[0]["type"] == "var" and self.isSymb(self.args[1]["type"]) and self.isSymb(self.args[2]["type"])
        else:
            return True


def parse_xml_instructions(root):
    program = [None] * len(root)

    for instruction in root:
        opcode = instruction.attrib["opcode"]
        if not is_opcode(opcode):
            exit_with_message("Error! Invalid OPCODE in XML.", ERR_XML_STRUCTURE)

        order = int(instruction.attrib["order"]) - 1

        program[order] = Instruction(opcode)

        # Check if correct number of args
        if program[order].getCorrectNumberOfArgs() != len(instruction):
            exit_with_message("Error! Incorrect number of arguments for instruction.", ERR_XML_STRUCTURE)

        program[order].setNumberOfArgs()  # Prepare array for args
        for arg in instruction:  # Iterate through args and add them to correct position
            if arg.tag not in ["arg1", "arg2", "arg3"]:
                exit_with_message("Error! Wrong tag for arguments.", ERR_XML_STRUCTURE)

            arg_num = int(arg.tag[3:])
            program[order].setArg(arg_num, arg.attrib["type"], arg.text)

        # Check argument syntax
        if not program[order].checkArgTypes():
            exit_with_message("Error! Wrong types of argument in instruction.", ERR_XML_STRUCTURE)

    return program

def display_help():
    print(
'''interpret.py is a script which reads XML representation of program and interprets this program using standart input and output.
It firstly perform lexical and syntax check of XML source code and afterward proceeds with executing the program,
while still checking for semantic and run-time errors.

Usage: python3.6 interpret.py [options]

Arguments (write in place of [options]):
    --help          display help, no additional arguments can be called
    --source=file   input file with XML, if not present, input will be taken from stdin
    --input=file    file with inputs for program itself, if not present, will be taken from stdin
Note: either --source, --input or both parameters has to been present.   
''', end="")

# MAIN

source_filename = None
input_filename = None

for arg in sys.argv[1:]:
    if arg == "--help":
        if len(sys.argv) == 2:
            display_help()
            exit(0)
        else:
            exit_with_message("Error! --help cannot be called with additional arguments.", ERR_SCRIPT_PARAMS)
    elif re.match(r'--source=(.*)', arg):
        source_filename = arg[arg.index('=') + 1:]
    elif re.match(r'--input=(.*)', arg):
        input_filename = arg[arg.index('=') + 1:]
    else:
        exit_with_message("Error! Unknown parameter.", ERR_SCRIPT_PARAMS)

if not source_filename and not input_filename:
    exit_with_message("Error! Neither source nor input file were provided.", ERR_SCRIPT_PARAMS)

if source_filename:
    root = get_xml_from_file(source_filename)
else:
    root = get_xml_from_stdin()

program = parse_xml_instructions(root)

# Prepare GF, LF, TF
GF = {}  # Global frame
LF = []  # Local frame
TF = None  # Temporary frame - not initialized at start

for instruction in program:
    print("Executing instruction: {}".format(instruction))
