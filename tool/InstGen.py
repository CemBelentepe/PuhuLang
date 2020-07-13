def tokenize (line):
    pieces = line.split(':')
    inst_name = pieces[0].strip()
    operands = []
    
    if len(pieces) == 2:
        ops = pieces[1].split(',')
        for operand in ops:
            field = operand.split()
            operands.append(field)
    
    decleration = "class " + inst_name + " : public Instruction\n{\npublic:\n"
    for op in operands:
        decleration += "\t" + op[0] + " " + op[1] + ";\n"
    decleration += "\n\t" + inst_name + "("
    for i in range(len(operands)):
        if i > 0:
            decleration += ", "
        decleration += operands[i][0] + " " + operands[i][1]
    decleration += ")\n\t\t"
    if len(operands) > 0:
        decleration += ": "
        for i in range(len(operands)):
            if i > 0:
                decleration += ", "
            decleration += operands[i][1] + "(" + operands[i][1] + ")"
    decleration += " { } \n\n\t void accept(InstVisitor* visitor);\n};"
    definition = "void " + inst_name + "::accept(InstVisitor* visitor)\n{\n\tvisitor->visit(this);\n}"

    return (decleration, definition)

instructions = ["InstConst  : int id",
                "InstCast   : TypeTag from, TypeTag to",
                "InstAdd    : TypeTag type",
                "InstSub    : TypeTag type",
                "InstMul    : TypeTag type",
                "InstDiv    : TypeTag type",
                "InstNeg    : TypeTag type",
                "InstMod",
                "InstBit    : TokenType op_type",
                "InstNot",
                "InstInc    : TypeTag type, int inc",
                "InstLess   : TypeTag type",
                "InstLte    : TypeTag type",
                "InstGreat  : TypeTag type",
                "InstGte    : TypeTag type",
                "InstEq     : TypeTag type",
                "InstNeq    : TypeTag type"]

header = "#pragma once\n\nclass InstVisitor;\nclass Instruction\n{\npublic:\n\tvirtual void accept(InstVisitor* visitor) = 0;\n};\n\n"
source = '#include "Instruction.h"\n#include "InstVisitor.hpp"\n\n'

for inst in instructions:
    x, y = tokenize(inst)
    header += x + "\n\n"
    source += y + "\n\n"

header_file = open("Instruction.h", "w")
source_file = open("Instruction.cpp", "w")
header_file.write(header)
source_file.write(source)
header_file.close()
source_file.close()