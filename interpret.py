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


# MAIN
source_filename = "example.xml"
input_filename = "input.txt"

root = get_xml_from_file(source_filename)
#root = get_xml_from_stdin()

program = [None] * len(root)

for instruction in root:
    opcode = instruction.attrib["opcode"]
    if not is_opcode(opcode):
        exit_with_message("Error! Invalid OPCODE in XML.", ERR_XML_STRUCTURE)

    print(opcode)

    order = int(instruction.attrib["order"]) - 1

    program[order] = Instruction(opcode)

    # Check if correct number of args
    if program[order].getCorrectNumberOfArgs() != len(instruction):
        exit_with_message("Error! Incorrect number of arguments for instruction.", ERR_XML_STRUCTURE)

    program[order].setNumberOfArgs() # Prepare array for args
    for arg in instruction: # Iterate through args and add them to correct position
        print("\t{}, {}: {}".format(arg.tag, arg.attrib["type"], arg.text))
        if (arg.tag not in ["arg1", "arg2", "arg3"]):
            exit_with_message("Error! Wrong tag for arguments.", ERR_XML_STRUCTURE)

        print("Arg num: {}".format(arg.tag[3:]))
        arg_num = int(arg.tag[3:])
        program[order].setArg(arg_num, arg.attrib["type"], arg.text)

    # Check argument syntax
    if not program[order].checkArgTypes():
        print("--- Error at instr: {}".format(program[order]))
        exit_with_message("Error! Wrong types of argument in instruction.", ERR_XML_STRUCTURE)

    print(program[order])


