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
ERR_SEMANTIC = 52  # Semantic errors (e.g. using undefined label)
ERR_RUNTIME_OPERANDS = 53  # Runtime error - wrong types of operands
ERR_RUNTIME_NONEXIST_VAR = 54  # Runtime error - accessing non-existing variable (but frame exists)
ERR_RUNTIME_FRAME = 55  # Runtime error - non-existing frame (e.g. reading from empty frame stack)
ERR_RUNTIME_MISSING_VAL = 56  # Runtime error - missing value (in variable, in data stack, in call stack)
ERR_RUNTIME_OPERAND_VAL = 57  # Runtime error - wrong operand value (e.g. zero division, wrong return value of EXIT)
ERR_RUNTIME_STRING = 58  # Runtime error - faulty work with string

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
        opcode = instruction.attrib["opcode"].upper()
        if not is_opcode(opcode):
            exit_with_message("Error! Invalid OPCODE in XML.", ERR_XML_STRUCTURE)

        order = int(instruction.attrib["order"]) - 1

        if order < 0 or order >= len(root):
            exit_with_message("Error! Instruction order is out of bounds.", ERR_XML_STRUCTURE)

        if program[order] != None:
            exit_with_message("Error! Instruction with order {} was already defined".format(order + 1), ERR_XML_STRUCTURE)

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

class Interpreter:
    def __init__(self):
        self.GF = {}
        self.LF = []
        self.TF = None
        self.data_stack = []  # Data stack - stored in format {"type": ..., "value": ...}
        self.call_stack = []  # Call stack
        self.labels = {}  # Dict of user defined labels with their position
        self.position = 0
        self.executed_instructions = 0

    def __repr__(self):
        return "<Interpret: position in code: %d, executed instructions: %d, GF:%s ,LF: %s ,TF: %s data_stack: %s>" % (self.position + 1, self.executed_instructions, str(self.GF), str(self.LF), str(self.TF), str(self.data_stack))

    def get_var_frame_name(self, var):
        # Check if correct frame
        if not re.match(r'^(?:GF@|LF@|TF@)(.*)', var):
            exit_with_message("Error! Variable in XML has to have valid frame identifier.", ERR_XML_STRUCTURE)

        frame, name = var.split("@", 1)  # Split frame and variable
        return frame, name

    def get_val_from_var(self, name, frame):

        if frame == "GF":
            if name not in self.GF.keys():
                exit_with_message("Error! Attempting to access non-existing variable in GF.", ERR_RUNTIME_NONEXIST_VAR)
            elif self.GF[name] == None:
                exit_with_message("Error! Attempting to access value of uninitialized variable in GF.", ERR_RUNTIME_MISSING_VAL)
            else:
                return self.GF[name]  # format: {"type": ..., "value": ...}
        elif frame == "LF":
            if len(self.LF) == 0:
                exit_with_message("Error! Empty LF stack.", ERR_RUNTIME_FRAME)
            elif name not in self.LF[-1].keys():
                exit_with_message("Error! Non-existing variable in LF.", ERR_RUNTIME_NONEXIST_VAR)
            elif self.LF[-1][name] == None:
                exit_with_message("Error! Attempting to access uninitialized variable in LF", ERR_RUNTIME_MISSING_VAL)
            else:
                return self.LF[-1][name]  # format: {"type": ..., "value": ...}

        else:
            if self.TF == None:
                exit_with_message("Error! Non-existing TF.", ERR_RUNTIME_FRAME)
            elif name not in self.TF.keys():
                exit_with_message("Error! Non-existing variable in TF.", ERR_RUNTIME_NONEXIST_VAR)
            elif self.TF[name] == None:
                exit_with_message("Error! Uninitialzed variable in TF.", ERR_RUNTIME_MISSING_VAL)
            else:
                return self.TF[name]  # format: {"type": ..., "value": ...}

    def store_to_var(self, var, symb):
        frame, name = self.get_var_frame_name(var)

        if frame == "GF":
            if name not in self.GF.keys():
                exit_with_message("Error! Attempting to store symbol in non-existing variable.", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.GF[name] = symb

        elif frame == "LF":
            if len(self.LF) == 0:
                exit_with_message("Error! Empty LF stack.", ERR_RUNTIME_FRAME)
            elif name not in self.LF[-1].keys():
                exit_with_message("Error! Non-existing variable in top LF.", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.LF[-1][name] = symb

        else:
            if self.TF == None:
                exit_with_message("Error! Non-existing TF.", ERR_RUNTIME_FRAME)
            elif name not in self.TF.keys():
                exit_with_message("Error! Non-existing variable in TF.", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.TF[name] = symb

    def MOVE(self, var, symb):
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            symb = self.get_val_from_var(name, frame)

        self.store_to_var(var, symb)

    def CREATEFRAME(self):
        # Create new TF and throws out old one
        self.TF = {}

    def PUSHFRAME(self):
        # Move TF to LF
        if self.TF == None:  # TF was not created - error 55
            exit_with_message("Error! PUSHFRAME requires TF to be initialized.", ERR_RUNTIME_FRAME)
        else:
            self.LF.append(self.TF)
            self.TF = None

    def POPFRAME(self):
        # Move top frame from LF to TF
        if len(self.LF) == 0: # Empty LF - error 55
            exit_with_message("Error! POPFRAME requires LF to not be empty.", ERR_RUNTIME_FRAME)
        else:
            self.TF = self.LF.pop()

    def DEFVAR(self, var):
        frame, name = self.get_var_frame_name(var)

        # Check if valid name for variable
        # TODO

        # Define variable in appropriate frame
        if frame == "GF":  # Define variable in GF
            if name in self.GF.keys():
                exit_with_message("Error! Attempting to redefine variable in GF.", ERR_SEMANTIC)
            else:
                self.GF[name] = None
        elif frame == "LF":  # Define variable in LF
            if len(self.LF) == 0:
                exit_with_message("Error! Attempting to define variable in empty LF stack.", ERR_RUNTIME_FRAME)
            elif name in self.LF[-1].keys():
                exit_with_message("Error! Attempting to redefine variable in LF.", ERR_SEMANTIC)
            else:
                self.LF[-1][name] = None
        else:  # Define variable in TF
            if self.TF == None:
                exit_with_message("Error! Attempting to define variable in non-existing frame TF.", ERR_RUNTIME_FRAME)
            elif name in self.TF.keys():
                exit_with_message("Error! Attempting to redefine variable in TF.", ERR_SEMANTIC)
            else:
                self.TF[name] = None

    def CALL(self, label):
        if label not in self.labels:
            exit_with_message("Error! CALL to unknown label.", ERR_SEMANTIC)
        else:
            self.call_stack.append(self.position)  # maybe should be + 1?
            self.position = self.labels[label]

    def RETURN(self):
        if len(self.call_stack) == 0:
            exit_with_message("Error! RETURN with empty call stack.", ERR_RUNTIME_MISSING_VAL)
        else:
            self.position = self.call_stack.pop()

    def PUSHS(self, symb):
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            symb = self.get_val_from_var(name, frame)

        self.data_stack.append(symb)

    def POPS(self, var):
        frame, name = self.get_var_frame_name(var)

        # Check if correct variable name
        # TODO

        # Check if data stack not empty
        if len(self.data_stack) == 0:
            exit_with_message("Error! Empty data stack in POPS instruction.", ERR_RUNTIME_MISSING_VAL)

        if frame == "GF":
            if name not in self.GF.keys():
                exit_with_message("Error! Attempting POPS with undefined variable in GF.", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.GF[name] = self.data_stack.pop()

        elif frame == "LF":
            if len(self.LF) == 0:
                exit_with_message("Error! Attempting POPS with empty LF stack.", ERR_RUNTIME_MISSING_VAL)
            elif name not in self.LF[-1].keys():
                exit_with_message("Error! Variable undefined in top LF.", ERR_RUNTIME_NONEXIST_VAR)
            else:
                self.LF[-1][name] = self.data_stack.pop()

        else:  # TF
            if self.TF == None:
                exit_with_message("Error! Attempting with non-existing TF.", ERR_RUNTIME_NONEXIST_VAR)
            elif name not in self.TF.keys():
                exit_with_message("Error! Variable undefined in TF.", ERR_RUNTIME_MISSING_VAL)
            else:
                self.TF[name] = self.data_stack.pop()

    def ADD(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in ADD have to be int.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] + symb2["value"]})

    def SUB(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in SUB have to be int.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] - symb2["value"]})

    def MUL(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in MUL have to be int.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] * symb2["value"]})

    def IDIV(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "int" or symb2["type"] != "int":
            exit_with_message("Error! Both operands in IDIV have to be int.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] // symb2["value"]})

    def LT(self, var, symb1, symb2):
        ... # TODO

    def GT(self, var, symb1, symb2):
        ... # TODO

    def EQ(self, var, symb1, symb2):
        ... # TODO

    def AND(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in AND have to be bool.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] and symb2["value"]})

    def OR(self, var, symb1, symb2):
        if symb1["type"] == "var":
            frame, name = self.get_var_frame_name(symb1["value"])
            symb1 = self.get_val_from_var(name, frame)
        if symb2["type"] == "var":
            frame, name = self.get_var_frame_name(symb2["value"])
            symb2 = self.get_val_from_var(name, frame)

        if symb1["type"] != "bool" or symb2["type"] != "bool":
            exit_with_message("Error! Both operands in OR have to be bool.", ERR_RUNTIME_OPERANDS)
        else:
            self.store_to_var(var, {"type": "int", "value": symb1["value"] or symb2["value"]})

    def NOT(self, var, symb):
        ... # TODO

    def INT2CHAR(self, var, symb):
        if symb["type"] != "var" and symb["type"] != "int":
            exit_with_message("Error! Wrong operand type in INT2CHAR.", ERR_RUNTIME_STRING)

        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            symb = self.get_val_from_var(name, frame)

        try:
            char = {"type": "string", "value": chr(symb["value"])}
            self.store_to_var(var, char)

        except ValueError:
            exit_with_message("Error! Value out of range for INT2CHAR.", ERR_RUNTIME_STRING)

    def STRI2INT(self, var, symb1, symb2):
        ... # TODO

    def READ(self, var, type):
        ... # TODO

    def WRITE(self, symb):
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            symb = self.get_val_from_var(name, frame)

        if symb["type"] == "int" or symb["type"] == "string":
            print(symb["value"], end="")  # Can be directly printed out
        elif symb["type"] == "bool":
            print("true" if symb["value"] else "false", end="")


    def CONCAT(self, var, symb1, symb2):
        ... # TODO

    def STRLEN(self, var, symb):
        ... # TODO

    def GETCHAR(self, var, symb1, symb2):
        ... # TODO

    def SETCHAR(self, var, symb1, symb2):
        ... # TODO

    def TYPE(self, var, symb):
        ... # TODO

    def LABEL(self, label):
        if label in self.labels.keys():
            exit_with_message("Error! Label redefinition.", ERR_SEMANTIC)
        else:
            self.labels[label] = self.position

    def JUMP(self, label):
        if label not in self.labels.keys():
            exit_with_message("Error! Unknown label", ERR_SEMANTIC)
        else:
            self.position = self.labels[label]

    def JUMPIFEQ(self, label, symb1, symb2):
        ... # TODO

    def JUMPIFNEQ(self, label, symb1, symb2):
        ... # TODO

    def EXIT(self, symb):
        if symb["type"] == "var":
            frame, name = self.get_var_frame_name(symb["value"])
            symb = self.get_val_from_var(name, frame)

        if symb["type"] != "int":
            exit_with_message("Error! Wrong operand in EXIT.", ERR_RUNTIME_OPERAND_VAL)

        if symb["value"] < 0 or symb["value"] > 49:
            exit_with_message("Error! Exit codes have to be in interval <0,49>.", ERR_RUNTIME_OPERAND_VAL)
        else:
            exit(symb["value"])

    def DPRINT(self, symb):
        ... # TODO

    def BREAK(self):
        print(self.__repr__(), file=sys.stderr)

    def run_code(self, data):
        instructions = data["instructions"]
        program_input = data["input"]

        # Run through all instructions just to set labels
        for self.position in range(0, len(instructions)):
            if instructions[self.position].opcode == "LABEL":
                self.LABEL(instructions[self.position].args[0]["value"])

        self.position = 0;

        # Real run
        while self.position < len(instructions):
            instruction = instructions[self.position]

            print("Before state:", self, file=sys.stderr)
            print("Executing instruction:", instruction, file=sys.stderr)

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
            #elif instruction.opcode == "LABEL":
            #    self.LABEL(instruction.args[0]["value"])
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

            print("After state:", self, "\n", file=sys.stderr)
            self.position += 1
            self.executed_instructions += 1

# MAIN

source_filename = None
#source_filename = "example.xml"
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

interpret = Interpreter()
interpret_input = {"from_stdin": True if input_filename else False, "file": input_filename if input_filename else None}
interpret.run_code({"instructions": program, "input": interpret_input})
