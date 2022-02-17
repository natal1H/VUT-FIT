# VUT FIT - IPP - Project - interpret.py
# Author: Natália Holková (xholko02)

import sys
import xml.etree.ElementTree as ET
import re

# Variables serving as constants with exit codes
ERR_SCRIPT_PARAMS = 10  # Missing or wrong script parameter
ERR_INPUT_FILES = 11  # Error with opening input files
ERR_OUTPUT_FILES = 12  # Error with opening output files
ERR_INTERNAL = 99  # Internal error
ERR_XML_FORMAT = 31  # Wrong XML format
ERR_XML_STRUCTURE = 32  # Wrong XML structure - lexical or syntax error
ERR_SEMANTIC = 52  # Semantic errors (e.g. using undefined label)
ERR_RUNTIME_OPERANDS = 53  # Runtime error - wrong types of operands
ERR_RUNTIME_NONEXIST_VAR = 54  # Runtime error - accessing non-existing variable (but frame exists)
ERR_RUNTIME_FRAME = 55  # Runtime error - non-existing frame (e.g. reading from empty frame stack)
ERR_RUNTIME_MISSING_VAL = 56  # Runtime error - missing value (in variable, in data stack, in call stack)
ERR_RUNTIME_OPERAND_VAL = 57  # Runtime error - wrong operand value (e.g. zero division, wrong return value of EXIT)
ERR_RUNTIME_STRING = 58  # Runtime error - faulty work with string


def exit_with_message(message, code):
    """Function to display error message and exit program with desired error code."""
    print(message, file=sys.stderr)
    sys.exit(code)


def is_opcode(opcode):
    """Function to determine if string it valid instruction OPCODE."""

    # List of all opcodes - case sensitive
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
        "DPRINT", "BREAK",  # Debugging
        # STACK extension
        "CLEARS", "ADDS", "SUBS", "MULS", "IDIVS", "LTS", "GTS", "EQS", "ANDS", "ORS", "NOTS",
        "INT2CHARS", "STRI2INTS", "JUMPIFEQS", "JUMPIFNEQS",
        # FLOAT extension
        "INT2FLOAT", "FLOAT2INT", "DIV"
    ]

    if opcode in opcodes:
        return True
    else:
        return False


def get_xml_from_file(filename):
    """Function to get string containing XML input from file with provided location."""
    try:
        file = open(filename, "r")
    except FileNotFoundError:
        # File could not be opened
        exit_with_message("Error! Could not open source file.", ERR_INPUT_FILES)

    xml_string = file.read()

    file.close()

    try:
        xml = ET.fromstring(xml_string)
    except:
        # String could not be parsed to XML - wrong format error
        exit_with_message("Error! Wrong XML format.", ERR_XML_FORMAT)

    return xml

def get_xml_from_stdin():
    """Function to get string containing XML input from stdin."""
    file = sys.stdin
    xml_string = ""
    for line in file.readlines():
        xml_string += line  # Add line from stdin to XML string

    try:
        xml = ET.fromstring(xml_string)
    except:
        # String could not be parsed to XML - wrong format error
        exit_with_message("Error! Wrong XML format.", ERR_XML_FORMAT)

    return xml

class Instruction:
    """Class for program instruction, each instruction has an opcode and arguments"""
    opcode = ""
    args = [] # Array containing all arguments

    def __init__(self, opcode):
        """Function to initialize Instruction object with provided opcode."""
        self.opcode = opcode

    def __repr__(self):
        """Function to get string representation of Instruction object"""
        return "<Instruction opcode:%s args: %s>" % (self.opcode, self.args)

    def get_correct_number_of_args(self):
        """Function to return correct number of arguments for instruction."""
        # List of opcodes with no arguments
        no_args = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK",
                   "CLEARS", "ADDS", "SUBS", "MULS", "IDIVS", "LTS", "GTS", "EQS",
                   "ANDS", "ORS", "NOTS", "INT2CHARS", "STRI2INTS"]
        # List of opcodes with 1 argument
        one_arg = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT", "JUMPIFEQS", "JUMPIFNEQS"]
        # List of opcodes with 2 arguments
        two_args = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT", "INT2FLOAT", "FLOAT2INT"]
        # Opcodes in none of those lists have 3 arguments

        if self.opcode in no_args:
            return 0
        elif self.opcode in one_arg:
            return 1
        elif self.opcode in two_args:
            return 2
        else:
            return 3

    def set_number_of_args(self):
        """Function to pre-allocate space for arguments in array, so that they can be placed on correct index."""
        self.args = [None] * self.get_correct_number_of_args()

    def set_arg(self, arg_num, arg_type, arg_value):
        """Fucntion to add argument to array in correct format."""

        # chage value from string to correct type if int or bool
        if arg_type == "int": arg_value = int(arg_value)
        elif arg_type == "bool": arg_value = True if arg_value == "true" else False
        elif arg_type == "string" and arg_value == None: arg_value = ""
        elif arg_type == "float": arg_value = float.fromhex(arg_value)

        # Add to array
        self.args[arg_num - 1] = {"type": arg_type, "value": arg_value}

    def is_symb(self, arg_type):
        """Function to determine if argument could be considered <symb>."""
        return arg_type == "var" or arg_type == "int" or arg_type == "bool" or arg_type == "string" or arg_type == "nil" or arg_type == "float"

    def check_arg_types(self):
        """Function to check if instruction syntax is correct."""

        if len(self.args) == 1:
            # Instruction syntax: <var>
            if self.opcode in ["DEFVAR", "POPS"]:
                return self.args[0]["type"] == "var"

            # Instruction syntax: <label>
            elif self.opcode in ["CALL", "LABEL", "JUMP", "JUMPIFEQS", "JUMPIFNEQS"]:
                return self.args[0]["type"] == "label"

            # Instruction syntax: <symb> (var | int | bool | string | nil)
            else:
                return self.is_symb(self.args[0]["type"])
        elif len(self.args) == 2:
            # Instruction syntax: <var> <symb>
            if self.opcode in ["MOVE", "INT2CHAR", "STRLEN", "TYPE", "NOT", "INT2FLOAT", "FLOAT2INT"]:
                return self.args[0]["type"] == "var" and self.is_symb(self.args[1]["type"])

            # Instruction syntax: <var> <type>
            else:
                return self.args[0]["type"] == "var" and self.args[1]["type"] == "type"

        elif len(self.args) == 3:
            # Instruction syntax: <label> <symb> <symb>
            if self.opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
                return self.args[0]["type"] == "label" and self.is_symb(self.args[1]["type"]) and self.is_symb(self.args[2]["type"])

            # Instruction syntax: <var> <symb> <symb>
            else:
                return self.args[0]["type"] == "var" and self.is_symb(self.args[1]["type"]) and self.is_symb(self.args[2]["type"])
        else:
            return True


def check_if_arg_correct(arg_type, arg_val):
    """Function to check if argument is of correct type."""
    if arg_type == "type":
        if arg_val not in ["int", "string", "bool", "float"]:
            exit_with_message("Error! Nil cannot be \"type\".", ERR_XML_STRUCTURE)
    elif arg_type == "string":
        if arg_val:
            # Check if contains # character
            if "#" in arg_val:
                exit_with_message("Error! String cannot contain #.", ERR_XML_STRUCTURE)
    elif arg_type not in ("var", "int", "bool", "nil", "float", "label"):
        exit_with_message("Error! Unknown argument type", ERR_XML_STRUCTURE)


def escape_string(str):
    """Function to remove escape sequences from string."""
    new_str = ""
    i = 0
    while i < len(str):
        if str[i] == "\\":  # Beginning of escape sequence
            esc_seq = str[i + 1: i + 4]
            new_str += chr(int(esc_seq))
            i += 4
        else:  # Normal character
            new_str += str[i]
            i += 1
    return new_str


def check_additional_text(where):
    """Function to check if there isn't text outside of element"""
    if where != None:
        if len(where.replace(" ", "").replace("\t", "").replace("\n", "")) > 0:
            exit_with_message("Error! Excessive text in program element.", ERR_XML_STRUCTURE)


def parse_xml_instructions(root):
    """Function to parse XML instruction."""
    program = [None] * len(root)  # Prepare array to store instructions

    # Check program language
    if root.attrib["language"] == None or root.attrib["language"] != "IPPcode19":
        exit_with_message("Error! Wrong language.", ERR_XML_STRUCTURE)

    # Check attributes in <program> element
    for key in root.attrib.keys():
        if key not in ["language", "name", "description"]:  # Only allowed language, name and description attributes
            exit_with_message("Error! Wrong program attributes.", ERR_XML_STRUCTURE)

    check_additional_text(root.text)  # Check for unwanted text in root.text
    check_additional_text(root.tail)  # Check for unwanted text in root.tail

    # Iterate through instructions
    for instruction in root:
        # Check number of attributes and if they are opcode and order
        if len(instruction.attrib) != 2 or instruction.attrib["opcode"] == None or instruction.attrib["order"] == None:
            exit_with_message("Error! Wrong instruction attributes.", ERR_XML_STRUCTURE)

        check_additional_text(instruction.text)  # Check for unwanted text in instruction.text
        check_additional_text(instruction.tail)  # Check for unwanted text in instruction.tail

        # Get and check instruction opcode
        opcode = instruction.attrib["opcode"]
        if not is_opcode(opcode):
            exit_with_message("Error! Invalid OPCODE in XML.", ERR_XML_STRUCTURE)

        # Get order and check if order in range
        order = int(instruction.attrib["order"]) - 1
        if order < 0 or order >= len(root):
            exit_with_message("Error! Instruction order is out of bounds.", ERR_XML_STRUCTURE)

        # Check if other instruction wasn't placed on the same position
        if program[order] != None:
            exit_with_message("Error! Instruction with order {} was already defined".format(order + 1), ERR_XML_STRUCTURE)

        program[order] = Instruction(opcode)

        # Check if correct number of args
        if program[order].get_correct_number_of_args() != len(instruction):
            exit_with_message("Error! Incorrect number of arguments for instruction.", ERR_XML_STRUCTURE)

        program[order].set_number_of_args()  # Prepare array for args
        for arg in instruction:  # Iterate through args and add them to correct position
            if len(arg.attrib) != 1 or arg.attrib["type"] == None:
                exit_with_message("Error! Wrong arg attributes.", ERR_XML_STRUCTURE)

            check_additional_text(arg.tail)  # Check for unwanted text in arg.tail

            if len(arg) > 0:  # Argument cannot have its own elements
                exit_with_message("Error! Argument has element.", ERR_XML_STRUCTURE)

            if arg.tag not in ["arg1", "arg2", "arg3"]:  # Wrong argument tag
                exit_with_message("Error! Wrong tag for arguments.", ERR_XML_STRUCTURE)

            # Get argument number and check if in range
            arg_num = int(arg.tag[3:])
            if arg_num > len(instruction):
                exit_with_message("Error! Wrong argument numbers.", ERR_XML_STRUCTURE)

            check_if_arg_correct(arg.attrib["type"], arg.text)  # Check argument validity

            # If argument is string and has text, remove escape sequences
            if arg.attrib["type"] == "string" and arg.text != None:
                arg.text = escape_string(arg.text)

            program[order].set_arg(arg_num, arg.attrib["type"], arg.text)

        # Check argument syntax
        if not program[order].check_arg_types():
            exit_with_message("Error! Wrong types of argument in instruction.", ERR_XML_STRUCTURE)

    return program


def display_help():
    """Function to display program help."""
    print(
'''interpret.py is a script which reads XML representation of program and interprets this program using standart input and output.
It firstly perform lexical and syntax check of XML source code and afterward proceeds with executing the program,
while still checking for semantic and run-time errors.

Usage: python3.6 interpret.py [options]

Arguments (write in place of [options]):
    --help          display help, no additional arguments can be called
    --source=file   input file with XML, if not present, input will be taken from stdin
    --input=file    file with inputs for program itself, if not present, will be taken from stdin
    --stats=file    where to output statistics about executed program
    --insts         count number of executed instructions, --stats has to be called before
    --vars          count max number of initialized variables, --stats has to be called before
Note: either --source, --input or both parameters has to been present.   
''', end="")


class Interpreter:
    def __init__(self, statistic):
        self.GF = {}  # Global frame
        self.LF = []  # Local frame stack
        self.TF = None  # Temporary frame
        self.data_stack = []  # Data stack - stored in format {"type": ..., "value": ...}
        self.call_stack = []  # Call stack
        self.labels = {}  # Dict of user defined labels with their position
        self.position = 0  # Actual position
        self.executed_instructions = 0  # Number of executed instructions
        self.program_input = None  # None -> input from stdin, != None -> input from file
        self.statistic = statistic  # Object where statistics are stored

    def __repr__(self):
        return "<Interpret: position in code: %d, executed instructions: %d, GF:%s ,LF: %s ,TF: %s data_stack: %s>" % (self.position + 1, self.executed_instructions, str(self.GF), str(self.LF), str(self.TF), str(self.data_stack))

    def get_var_frame_name(self, var):
        """Function to get separately variable frame and name."""
        # Check if correct frame
        if not re.match(r'^(?:GF@|LF@|TF@)(.*)', var):
            exit_with_message("Error! Variable in XML has to have valid frame identifier.", ERR_XML_STRUCTURE)

        frame, name = var.split("@", 1)  # Split frame and variable
        return frame, name

    def get_val_from_var(self, name, frame):
        """Function to get actual value from variable."""
        if frame == "GF":  # Global frame
            if name not in self.GF.keys():  # Non-existing variable
                exit_with_message("Error! Attempting to access non-existing variable in GF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            elif self.GF[name] == None:  # Uninitialized variable
                exit_with_message("Error! Attempting to access value of uninitialized variable in GF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
            else:  # OK
                return self.GF[name]  # format: {"type": ..., "value": ...}
        elif frame == "LF":  # Local frame
            if len(self.LF) == 0:  # Empty LF
                exit_with_message("Error! Empty LF stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.LF[-1].keys():  # Non-existing variable
                exit_with_message("Error! Non-existing variable in LF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            elif self.LF[-1][name] == None:  # Uninitialized variable
                exit_with_message("Error! Attempting to access uninitialized variable in LF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
            else:  # OK
                return self.LF[-1][name]  # format: {"type": ..., "value": ...}

        else:  # Temporary frame
            if self.TF == None:  # Non-existing TF
                exit_with_message("Error! Non-existing TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.TF.keys():  # Non-existing variable
                exit_with_message("Error! Non-existing variable in TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            elif self.TF[name] == None:  # Uninitialized variable
                exit_with_message("Error! Uninitialzed variable in TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
            else:  # OK
                return self.TF[name]  # format: {"type": ..., "value": ...}

    def store_to_var(self, var, symb):
        """Function to store symbol to variable"""
        frame, name = self.get_var_frame_name(var)  # Get variable frame and name

        if frame == "GF":  # Global frame
            if name not in self.GF.keys():
                exit_with_message("Error! Attempting to store symbol in non-existing variable. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.GF[name] = symb

        elif frame == "LF":  # Local frame
            if len(self.LF) == 0:
                exit_with_message("Error! Empty LF stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.LF[-1].keys():
                exit_with_message("Error! Non-existing variable in top LF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.LF[-1][name] = symb

        else:  # Temporary frame
            if self.TF == None:
                exit_with_message("Error! Non-existing TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.TF.keys():
                exit_with_message("Error! Non-existing variable in TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.TF[name] = symb

    def get_from_stack(self):
        """Function to get symbol from data stack."""
        if len(self.data_stack) == 0:  # Empty data stack
            exit_with_message("Error! Empty data stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
        return self.data_stack.pop()

    def expand_symbol(self, symb):
        """Function to try to expand symbol if it is variable."""
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            return self.get_val_from_var(name, frame)
        else:
            return symb

    def count_intialized_vars(self):
        """Function to count current number of initialized variables in GF, all of LF and TF."""
        count = 0

        # Count intialized vars in GF
        for key in self.GF.keys():
            if self.GF[key] != None: count += 1

        # Count initialized vars in LF
        for frame in self.LF:
            for key in frame.keys():
                if frame[key] != None: count += 1

        # Count initialized vars in TF
        if self.TF != None:
            for key in self.TF.keys():
                if self.TF[key] != None: count += 1

        return count

    def MOVE(self, var, symb):
        """Instruction MOVE <var> <symb>"""
        symb = self.expand_symbol(symb)  # If symb is variable, get actual value
        self.store_to_var(var, symb)  # Store value to variable

    def CREATEFRAME(self):
        """Instruction CREATEFRAME"""
        # Create new TF and throws out old one
        self.TF = {}

    def PUSHFRAME(self):
        """Instruction PUSHFRAME"""
        # Move TF to LF
        if self.TF == None:  # TF was not created - error 55
            exit_with_message("Error! PUSHFRAME requires TF to be initialized. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
        else:
            self.LF.append(self.TF)
            self.TF = None

    def POPFRAME(self):
        """Instruction POPFRAME"""
        # Move top frame from LF to TF
        if len(self.LF) == 0: # Empty LF - error 55
            exit_with_message("Error! POPFRAME requires LF to not be empty. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
        else:
            self.TF = self.LF.pop()

    def DEFVAR(self, var):
        """Instruction DEFVAR <var>"""
        frame, name = self.get_var_frame_name(var)

        # Define variable in appropriate frame
        if frame == "GF":  # Define variable in GF
            if name in self.GF.keys():
                exit_with_message("Error! Attempting to redefine variable in GF. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
            else:
                self.GF[name] = None
        elif frame == "LF":  # Define variable in LF
            if len(self.LF) == 0:
                exit_with_message("Error! Attempting to define variable in empty LF stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name in self.LF[-1].keys():
                exit_with_message("Error! Attempting to redefine variable in LF. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
            else:
                self.LF[-1][name] = None
        else:  # Define variable in TF
            if self.TF == None:
                exit_with_message("Error! Attempting to define variable in non-existing frame TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name in self.TF.keys():
                exit_with_message("Error! Attempting to redefine variable in TF. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
            else:
                self.TF[name] = None

    def CALL(self, label):
        """Instruction CALL <label>"""
        if label not in self.labels:  # Unknown label
            exit_with_message("Error! CALL to unknown label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
        else:
            self.call_stack.append(self.position)  # append to call stack actual position
            self.position = self.labels[label]  # set new position

    def RETURN(self):
        """Instruction RETURN"""
        if len(self.call_stack) == 0:  # Empty call stack
            exit_with_message("Error! RETURN with empty call stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
        else:
            self.position = self.call_stack.pop()

    def PUSHS(self, symb):
        """Instruction PUSHS <symb>"""
        symb = self.expand_symbol(symb)
        self.data_stack.append(symb)

    def POPS(self, var):
        """Instruction POPS <var>"""
        frame, name = self.get_var_frame_name(var)

        # Check if data stack not empty
        if len(self.data_stack) == 0:
            exit_with_message("Error! Empty data stack in POPS instruction. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)

        if frame == "GF":
            if name not in self.GF.keys():
                exit_with_message("Error! Attempting POPS with undefined variable in GF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.GF[name] = self.data_stack.pop()

        elif frame == "LF":
            if len(self.LF) == 0:
                exit_with_message("Error! Attempting POPS with empty LF stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.LF[-1].keys():
                exit_with_message("Error! Variable undefined in top LF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.LF[-1][name] = self.data_stack.pop()

        else:  # TF
            if self.TF == None:
                exit_with_message("Error! Attempting with non-existing TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
            elif name not in self.TF.keys():
                exit_with_message("Error! Variable undefined in TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_MISSING_VAL)
            else:
                self.TF[name] = self.data_stack.pop()

    def ADD(self, var, symb1, symb2):
        """Instruction ADD <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if not ( (symb1["type"] == "int" and symb2["type"] == "int") or (symb1["type"] == "float" and symb2["type"] == "float") ):
            exit_with_message("Error! Both operands in ADD have to be both int or float. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            type = "float" if symb1["type"] == "float" or symb2["type"] == "float" else "int"  # Determine type
            self.store_to_var(var, {"type": type, "value": symb1["value"] + symb2["value"]})

    def SUB(self, var, symb1, symb2):
        """Instruction SUB <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if not ((symb1["type"] == "int" and symb2["type"] == "int") or (
                symb1["type"] == "float" and symb2["type"] == "float")):
            exit_with_message("Error! Both operands in ADD have to be both int or float. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            type = "float" if symb1["type"] == "float" or symb2["type"] == "float" else "int"  # Determine type
            self.store_to_var(var, {"type": type, "value": symb1["value"] - symb2["value"]})

    def MUL(self, var, symb1, symb2):
        """Instruction MUL <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if not ((symb1["type"] == "int" and symb2["type"] == "int") or (
                symb1["type"] == "float" and symb2["type"] == "float")):
            exit_with_message("Error! Both operands in ADD have to be both int or float. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            type = "float" if symb1["type"] == "float" or symb2["type"] == "float" else "int"  # Determine type
            self.store_to_var(var, {"type": type, "value": symb1["value"] * symb2["value"]})

    def IDIV(self, var, symb1, symb2):
        """Instruction IDIV <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in IDIV have to be int. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            # Check zero division:
            if symb2["value"] == 0:
                exit_with_message("Error! Zero division. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERAND_VAL)

            self.store_to_var(var, {"type": "int", "value": symb1["value"] // symb2["value"]})

    def LT(self, var, symb1, symb2):
        """Instruction LT <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != symb2["type"]:
            exit_with_message("Errot! Different types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            exit_with_message("Error! Cannot use LT with nil. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            if symb1["type"] == "bool":
                res = True if (not symb1["value"] and symb2["value"]) else False
                self.store_to_var(var, {"type": "bool", "value": res})
            else:
                self.store_to_var(var, {"type": "bool", "value": symb1["value"] < symb2["value"]})

    def GT(self, var, symb1, symb2):
        """Instruction GT <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != symb2["type"]:
            exit_with_message("Errot! Different types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            exit_with_message("Error! Cannot use LT with nil. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            if symb1["type"] == "bool":
                res = True if (symb1["value"] and not symb2["value"]) else False
                self.store_to_var(var, {"type": "bool", "value": res})
            else:
                self.store_to_var(var, {"type": "bool", "value": symb1["value"] > symb2["value"]})

    def EQ(self, var, symb1, symb2):
        """Instruction EQ <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                self.store_to_var(var, {"type": "bool", "value": True})
            else:
                self.store_to_var(var, {"type": "bool", "value": symb1["value"] == symb2["value"]})
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            self.store_to_var(var, {"type": "bool", "value": False})
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    def AND(self, var, symb1, symb2):
        """Instruction AND <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in AND have to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "bool", "value": symb1["value"] and symb2["value"]})

    def OR(self, var, symb1, symb2):
        """Instruction OR <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in OR have to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "bool", "value": symb1["value"] or symb2["value"]})

    def NOT(self, var, symb):
        """Instruction NOT <var> <symb>"""
        symb = self.expand_symbol(symb)

        # Check operand
        if symb["type"] != "bool":
            exit_with_message("Error! Operand in NOT has to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "bool", "value": not symb["value"]})

    def INT2CHAR(self, var, symb):
        """Instruction INT2CHAR <var> <symb>"""
        symb = self.expand_symbol(symb)

        # Check operand
        if symb["type"] != "int":
            exit_with_message("Error! Wrong operand type in INT2CHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

        try:
            char = {"type": "string", "value": chr(symb["value"])}  # Try to get character
            self.store_to_var(var, char)
        except ValueError:
            exit_with_message("Error! Value out of range for INT2CHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)

    def STRI2INT(self, var, symb1, symb2):
        """Instruction STRI2INT <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "string" or symb2["type"] != "int":
            exit_with_message("Error! Wrong types or operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb2["value"] < 0 or symb2["value"] >= len(symb1["value"]):
            exit_with_message("Error! Index out of range. (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)
        else:
            self.store_to_var(var, {"type": "int", "value": ord(symb1["value"][symb2["value"]])})

    def READ(self, var, type):
        """Instruction READ <var> <type>"""
        if self.program_input["from_stdin"]:  # Read from stdin
            input_str = input()
        else:  # Read line from file
            if len(self.program_input["file"]) > 0:
                input_str = self.program_input["file"].pop(0)
            else:  # Nothing could be read - default values
                if type == "string":
                    self.store_to_var(var, {"type": "string", "value": ""})
                elif type == "int":
                    self.store_to_var(var, {"type": "int", "value": 0})
                elif type == "float":
                    self.store_to_var(var, {"type": "float", "value": 0.0})
                else: # bool
                    self.store_to_var(var, {"type": "bool", "value": False})
                return

        # Store input to variable
        if type == "int":
            try:
                input_int = int(input_str)
                self.store_to_var(var, {"type": "int", "value": input_int})
            except ValueError:
                self.store_to_var(var, {"type": "int", "value": 0})

        elif type == "bool":
            if input_str.upper() != "TRUE" and input_str.upper() != "FALSE":
                self.store_to_var(var, {"type": "bool", "value": False})
            else:
                self.store_to_var(var, {"type": "bool", "value": True if input_str.upper() == "TRUE" else False})

        elif type == "string":
            self.store_to_var(var, {"type": "string", "value": input_str})

        elif type == "float":
            self.store_to_var(var, {"type": "float", "value": float.fromhex(input_str)})


    def WRITE(self, symb):
        """Instruction WRITE <var> <symb>"""
        symb = self.expand_symbol(symb)

        if symb["type"] in ["int", "type"]:
            print(symb["value"], end="")  # Can be directly printed out
        elif symb["type"] == "string" and symb["value"] != None:
            print(symb["value"], end="")
        elif symb["type"] == "bool":
            print("true" if symb["value"] else "false", end="")
        elif symb["type"] == "float":
            print(symb["value"].hex(), end="")

    def CONCAT(self, var, symb1, symb2):
        """Instruction CONCAT <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "string" or symb2["type"] != "string":
            exit_with_message("Error! Both operands in CONCAT have to be string. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "string", "value": symb1["value"] + symb2["value"]})

    def STRLEN(self, var, symb):
        """Instruction STRLEN <var> <symb>"""
        symb = self.expand_symbol(symb)

        # Check operands
        if symb["type"] != "string":
            exit_with_message("Error! Operand in STRLEN has to be string. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            len_full = len(symb["value"])
            self.store_to_var(var, {"type": "int", "value": len_full})

    def GETCHAR(self, var, symb1, symb2):
        """Instruction GETCHAR <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "string" or symb2["type"] != "int":
            exit_with_message("Error! Wrong operands in GETCHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb2["value"] < 0 or symb2["value"] >= len(symb1["value"]):
            exit_with_message("Error! Index out of range. (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)
        else:
            self.store_to_var(var, {"type": "string", "value": symb1["value"][symb2["value"]]})

    def SETCHAR(self, var, symb1, symb2):
        """Instruction SETCHAR <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        frame, name = self.get_var_frame_name(var)
        val = self.get_val_from_var(name, frame)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "string" or val["type"] != "string":
            exit_with_message("Error! Wrong operands in GETCHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb1["value"] < 0 or symb1["value"] >= len(val["value"]) or symb2["value"] == "":
            exit_with_message("Error! Index out of range or empty string (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)
        else:
            new_char = symb2["value"][0]
            place_index = symb1["value"]
            val["value"] = val["value"][:place_index] + new_char + val["value"][place_index + 1:]
            self.store_to_var(var, {"type": "string", "value": val["value"]})

    def TYPE(self, var, symb):
        """Instruction TYPE <var> <symb>"""
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])

            if frame == "GF":
                if name not in self.GF.keys():
                    exit_with_message("Error! Attempting to access non-existing variable in GF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
                elif self.GF[name] == None:
                    type = ""
                else:
                    type = self.GF[name]["type"]
            elif frame == "LF":
                if len(self.LF) == 0:
                    exit_with_message("Error! Empty LF stack. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
                elif name not in self.LF[-1].keys():
                    exit_with_message("Error! Non-existing variable in LF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
                elif self.LF[-1][name] == None:
                    type = ""
                else:
                    type = self.LF[-1][name]["type"]

            else:
                if self.TF == None:
                    exit_with_message("Error! Non-existing TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_FRAME)
                elif name not in self.TF.keys():
                    exit_with_message("Error! Non-existing variable in TF. (line " + str(self.position + 1) + ")", ERR_RUNTIME_NONEXIST_VAR)
                elif self.TF[name] == None:
                    type = ""
                else:
                    type = self.TF[name]["type"]
        else:
            type = symb["type"]

        self.store_to_var(var, {"type": "type", "value": type})

    def LABEL(self, label):
        """Instruction LABEL <label>"""
        if label in self.labels.keys():  # Already defined
            exit_with_message("Error! Label redefinition. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
        else:  # Remember position of this label
            self.labels[label] = self.position

    def JUMP(self, label):
        """Instruction JUMP <label>"""
        if label not in self.labels.keys():  # Label not defined
            exit_with_message("Error! Unknown label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)
        else:  # Change position in program
            self.position = self.labels[label]

    def JUMPIFEQ(self, label, symb1, symb2):
        """Instruction JUMPIFEQ <label> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        if label not in self.labels.keys():
            exit_with_message("Error! Undefined label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)

        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                jump = True
            else:
                jump = symb1["value"] == symb2["value"]

            if jump:
                self.position = self.labels[label]
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    def JUMPIFNEQ(self, label, symb1, symb2):
        """Instruction JUMPIFNEQ <label> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        if label not in self.labels.keys():
            exit_with_message("Error! Undefined label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)

        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                jump = False
            else:
                jump = symb1["value"] != symb2["value"]

            if jump:
                self.position = self.labels[label]
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    def EXIT(self, symb):
        """Instruction EXIT <symb>"""
        symb = self.expand_symbol(symb)

        # Check operand
        if symb["type"] != "int":
            exit_with_message("Error! Wrong operand in EXIT. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

        if symb["value"] < 0 or symb["value"] > 49:
            exit_with_message("Error! Exit codes have to be in interval <0,49>. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERAND_VAL)
        else:
            exit(symb["value"])  # Exit program with error code

    def DPRINT(self, symb):
        """Instruction DPRINT <symb>"""
        symb = self.expand_symbol(symb)

        # Print out actual value of symbol
        if symb["type"] == "int" or symb["type"] == "string":
            print(symb["value"], end="", file=sys.stderr)  # Can be directly printed out
        elif symb["type"] == "bool":
            print("true" if symb["value"] else "false", end="", file=sys.stderr)

    def BREAK(self):
        """Instruction BREAK"""
        print(self.__repr__(), file=sys.stderr)  # Print out actual state of interpreter

    # STACK instructions
    def CLEARS(self):
        """Instruction CLEARS"""
        self.data_stack = []  # Data stack is empty now

    def ADDS(self):
        """Instruction ADDS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in ADD have to be int. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:  # Store result back to data stack
            self.data_stack.append({"type": "int", "value": symb1["value"] + symb2["value"]})

    def SUBS(self):
        """Instruction SUBS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in ADD have to be int. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:  # Store result back to data stack
            self.data_stack.append({"type": "int", "value": symb1["value"] - symb2["value"]})

    def MULS(self):
        """Instruction MULS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in ADD have to be int. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:  # Store result back to data stack
            self.data_stack.append({"type": "int", "value": symb1["value"] * symb2["value"]})

    def IDIVS(self):
        """Instruction IDIVS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in ADD have to be int. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            # Check zero division:
            if symb2["value"] == 0:
                exit_with_message("Error! Zero division.", ERR_RUNTIME_OPERAND_VAL)

            self.data_stack.append({"type": "int", "value": symb1["value"] // symb2["value"]})  # Store result back to data stack

    def LTS(self):
        """Instruction LTS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != symb2["type"]:
            exit_with_message("Errot! Different types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            exit_with_message("Error! Cannot use LT with nil. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            if symb1["type"] == "bool":
                res = True if (symb1["value"] == False and symb2["value"] == True) else False
                self.data_stack.append({"type": "bool", "value": res})
            else:
                self.data_stack.append({"type": "bool", "value": symb1["value"] < symb2["value"]})

    def GTS(self):
        """Instruction GTS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != symb2["type"]:
            exit_with_message("Errot! Different types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            exit_with_message("Error! Cannot use LT with nil. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            if symb1["type"] == "bool":
                res = True if (symb1["value"] and not symb2["value"]) else False
                self.data_stack.append({"type": "bool", "value": res})
            else:
                self.data_stack.append({"type": "bool", "value": symb1["value"] > symb2["value"]})

    def EQS(self):
        """Instruction EQS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                self.data_stack.append({"type": "bool", "value": True})
            else:
                self.data_stack.append({"type": "bool", "value": symb1["value"] == symb2["value"]})
        elif symb1["type"] == "nil" or symb2["type"] == "nil":
            self.data_stack.append({"type": "bool", "value": False})
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    def ANDS(self):
        """Instruction ANDS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in AND have to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.data_stack.append({"type": "bool", "value": symb1["value"] and symb2["value"]})

    def ORS(self):
        """Instruction ORS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in AND have to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.data_stack.append({"type": "bool", "value": symb1["value"] or symb2["value"]})

    def NOTS(self):
        """Instruction NOTS"""
        symb = self.get_from_stack()
        symb = self.expand_symbol(symb)

        # Check operand
        if symb["type"] != "bool":
            exit_with_message("Error! Operand in NOT has to be bool. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.data_stack.append({"type": "bool", "value": not symb["value"]})

    def INT2CHARS(self):
        """Instruction INT2CHARS"""
        symb = self.get_from_stack()
        symb = self.expand_symbol(symb)

        # Check operands
        if symb["type"] != "int":
            exit_with_message("Error! Wrong operand type in INT2CHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

        try:
            char = {"type": "string", "value": chr(symb["value"])}
            self.data_stack.append(char)
        except ValueError:
            exit_with_message("Error! Value out of range for INT2CHAR. (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)

    def STRI2INTS(self):
        """Instruction STRI2INTS"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if symb1["type"] != "string" or symb2["type"] != "int":
            exit_with_message("Error! Wrong types or operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        elif symb2["value"] < 0 or symb2["value"] >= len(symb1["value"]):
            exit_with_message("Error! Index out of range. (line " + str(self.position + 1) + ")", ERR_RUNTIME_STRING)
        else:
            self.data_stack.append({"type": "int", "value": ord(symb1["value"][symb2["value"]])})

    def JUMPIFEQS(self, label):
        """Instruction JUMPIFEQS <label>"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        if label not in self.labels.keys():
            exit_with_message("Error! Undefined label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)

        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                jump = True
            else:
                jump = symb1["value"] == symb2["value"]

            if jump:
                self.position = self.labels[label]
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    def JUMPIFNEQS(self, label):
        """Instruction JUMPIFNEQS <label>"""
        symb2 = self.get_from_stack()
        symb1 = self.get_from_stack()
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        if label not in self.labels.keys():
            exit_with_message("Error! Undefined label. (line " + str(self.position + 1) + ")", ERR_SEMANTIC)

        if symb1["type"] == symb2["type"]:
            if symb1["type"] == "nil":
                jump = False
            else:
                jump = symb1["value"] != symb2["value"]

            if jump:
                self.position = self.labels[label]
        else:
            exit_with_message("Error! Wrong types of operands. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)

    # FLOAT extension instructions
    def INT2FLOAT(self, var, symb):
        """Instruction INT2FLOAT <var> <symb>"""
        symb = self.expand_symbol(symb)

        if symb["type"] != "int":
            exit_with_message("Error! Wrong operands in INT2FLOAT. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "float", "value": float(symb["value"])})

    def FLOAT2INT(self, var, symb):
        """Instruction FLOAT2INT <var> <symb>"""
        symb = self.expand_symbol(symb)

        if symb["type"] != "float":
            exit_with_message("Error! Wrong operands in FLOAT2INT. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": int(symb["value"])})

    def DIV(self, var, symb1, symb2):
        """Instruction DIV <var> <symb1> <symb2>"""
        symb1 = self.expand_symbol(symb1)
        symb2 = self.expand_symbol(symb2)

        # Check operands
        if not ((symb1["type"] == "int" and symb2["type"] == "int") or (
                symb1["type"] == "float" and symb2["type"] == "float")):
            exit_with_message("Error! Both operands in ADD have to be both int or float. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERANDS)
        else:
            # Check zero division:
            if symb2["value"] == 0:
                exit_with_message("Error! Zero division. (line " + str(self.position + 1) + ")", ERR_RUNTIME_OPERAND_VAL)

            type = "float" if symb1["type"] == "float" or symb2["type"] == "float" else "int"
            self.store_to_var(var, {"type": type, "value": symb1["value"] / symb2["value"]})

    def run_code(self, data):
        """Function to run code."""
        instructions = data["instructions"]  # Instructions to execute
        self.program_input = data["input"]  # Where to get input from

        # Run through all instructions just to set labels
        for self.position in range(0, len(instructions)):
            if instructions[self.position].opcode == "LABEL":
                self.LABEL(instructions[self.position].args[0]["value"])

        self.position = 0;

        # Real run
        while self.position < len(instructions):
            instruction = instructions[self.position]

            if instruction.opcode == "MOVE":
                self.MOVE(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "CREATEFRAME":
                self.CREATEFRAME()
            elif instruction.opcode == "PUSHFRAME":
                self.PUSHFRAME()
            elif instruction.opcode == "POPFRAME":
                self.POPFRAME()
            elif instruction.opcode == "DEFVAR":
                self.DEFVAR(instruction.args[0]["value"])  # No need to pass arg type if only 1 allowed, already checked
            elif instruction.opcode == "CALL":
                self.CALL(instruction.args[0]["value"])  # Only 1 arg type allowed, no need to pass type
            elif instruction.opcode == "RETURN":
                self.RETURN()
            elif instruction.opcode == "PUSHS":
                self.PUSHS(instruction.args[0])  # Pass whole arg, because it could be multiple types
            elif instruction.opcode == "POPS":
                self.POPS(instruction.args[0]["value"])
            elif instruction.opcode == "ADD":
                self.ADD(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "SUB":
                self.SUB(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "MUL":
                self.MUL(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "IDIV":
                self.IDIV(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "LT":
                self.LT(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "GT":
                self.GT(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "EQ":
                self.EQ(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "AND":
                self.AND(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "OR":
                self.OR(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "NOT":
                self.NOT(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "INT2CHAR":
                self.INT2CHAR(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "STRI2INT":
                self.STRI2INT(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "READ":
                self.READ(instruction.args[0]["value"], instruction.args[1]["value"])
            elif instruction.opcode == "WRITE":
                self.WRITE(instruction.args[0])
            elif instruction.opcode == "CONCAT":
                self.CONCAT(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "STRLEN":
                self.STRLEN(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "GETCHAR":
                self.GETCHAR(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "SETCHAR":
                self.SETCHAR(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "TYPE":
                self.TYPE(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "JUMP":
                self.JUMP(instruction.args[0]["value"])
            elif instruction.opcode == "JUMPIFEQ":
                self.JUMPIFEQ(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "JUMPIFNEQ":
                self.JUMPIFNEQ(instruction.args[0]["value"], instruction.args[1], instruction.args[2])
            elif instruction.opcode == "EXIT":
                self.EXIT(instruction.args[0])
            elif instruction.opcode == "DPRINT":
                self.DPRINT(instruction.args[0])
            elif instruction.opcode == "BREAK":
                self.BREAK()
            # STACK extension instructions
            elif instruction.opcode == "CLEARS":
                self.CLEARS()
            elif instruction.opcode == "ADDS":
                self.ADDS()
            elif instruction.opcode == "SUBS":
                self.SUBS()
            elif instruction.opcode == "MULS":
                self.MULS()
            elif instruction.opcode == "IDIVS":
                self.IDIVS()
            elif instruction.opcode == "LTS":
                self.LTS()
            elif instruction.opcode == "GTS":
                self.GTS()
            elif instruction.opcode == "EQS":
                self.EQS()
            elif instruction.opcode == "ANDS":
                self.ANDS()
            elif instruction.opcode == "ORS":
                self.ORS()
            elif instruction.opcode == "NOTS":
                self.NOTS()
            elif instruction.opcode == "INT2CHARS":
                self.INT2CHARS()
            elif instruction.opcode == "STRI2INTS":
                self.STRI2INTS()
            elif instruction.opcode == "JUMPIFEQS":
                self.JUMPIFEQS(instruction.args[0]["value"])
            elif instruction.opcode == "JUMPIFNEQS":
                self.JUMPIFNEQS(instruction.args[0]["value"])
            # FLOAT extension
            elif instruction.opcode == "INT2FLOAT":
                self.INT2FLOAT(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "FLOAT2INT":
                self.FLOAT2INT(instruction.args[0]["value"], instruction.args[1])
            elif instruction.opcode == "DIV":
                self.DIV(instruction.args[0]["value"], instruction.args[1], instruction.args[2])

            self.position += 1  # Move to next instruction
            self.executed_instructions += 1  # Increase number of executed instruction
            self.statistic.inc_insts()  # Increase statistics
            self.statistic.try_change_vars(self.count_intialized_vars())  # Determine max number of initialized variables

        # Finished executing instructions
        if statistic.enabled:
            statistic.output_stats()


class Stats:
    """Class containing statistics about executed program"""
    def __init__(self):
        self.file = ""
        self.options = []
        self.stats = {"--insts": 0, "--vars": 0}
        self.enabled = False

    def enable(self):
        """Set statistics output to enabled"""
        self.enabled = True

    def set_file(self, file):
        """Set file where stats will be stored to"""
        self.file = file

    def add_option(self, option):
        """Add option to output"""
        self.options.append(option)

    def inc_insts(self):
        self.stats["--insts"] += 1

    def try_change_vars(self, count):
        self.stats["--vars"] =  max(self.stats["--vars"], count)

    def output_stats(self):
        """Output statistics to file"""
        try:
            file = open(self.file, "w")
        except FileNotFoundError:
            exit_with_message("Error! Could not open output file for stats.", ERR_OUTPUT_FILES)

        for option in self.options:
            file.write(str(self.stats[option]) + "\n")

        file.close()

# MAIN

source_filename = None
input_filename = None
statistic = Stats()

# Parse command line arguments
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
    elif re.match(r'--stats=(.*)', arg):
        statistic.enable()
        statistic.set_file(arg[arg.index('=') + 1:])
    elif arg == "--insts" or arg == "--vars":
        if not statistic.enabled:
            exit_with_message("Error! --stats has to be called before.", ERR_SCRIPT_PARAMS)
        else:
            statistic.add_option(arg)
    else:
        exit_with_message("Error! Unknown parameter.", ERR_SCRIPT_PARAMS)

# Check if source and input are from stdin at the same time
if not source_filename and not input_filename:
    exit_with_message("Error! Neither source nor input file were provided.", ERR_SCRIPT_PARAMS)

if source_filename:
    root = get_xml_from_file(source_filename)  # Get xml from file
else:
    root = get_xml_from_stdin()  # Get xml from stdin

program = parse_xml_instructions(root)  # Parse XML and get list of instructions

interpret = Interpreter(statistic)
if input_filename == None:
    interpret_input = {"from_stdin": True, "file_content": None}  # Interpret input from stdin
else: # Interpret will have input from file
    try:
        file = open(input_filename, "r")
        content = file.read().splitlines()
        file.close()
    except FileNotFoundError:
        exit_with_message("Error! Could not open input file.", ERR_INPUT_FILES)

    interpret_input = {"from_stdin": False, "file": content}

# Run the code
interpret.run_code({"instructions": program, "input": interpret_input})
