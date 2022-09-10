def make_function_declaration(line: [str]) -> str:
    tok = line[0]
    out = line[2].upper()
    in1 = line[3].upper()
    in2 = line[4].upper()
    code = f"{{{{TokenType::{tok}, " \
           f"PrimitiveTag::{in1}, PrimitiveTag::{in2}}}, " \
           f"PrimitiveTag::{out} }}"
    return code


def make_function_definitions(line: [str]) -> str:
    tok = line[0]
    op = line[1]
    in1 = line[3]
    in2 = line[4]

    code = f"{{{{TokenType::{tok}, " \
           f"PrimitiveTag::{in1.upper()}, PrimitiveTag::{in2.upper()}}}, " \
           f"[](Value::Data lhs, Value::Data rhs){{" \
           f"return Value::Data(std::get<{in1}>(lhs) {op} std::get<{in2}>(rhs));" \
           f"}}}}"

    return code


def make_unary_function_declaration(line: [str]) -> str:
    tok = line[0]
    typ = line[2].upper()

    code = f"{{{{TokenType::{tok}, " \
           f"PrimitiveTag::{typ}}}, " \
           f"PrimitiveTag::{typ} }}"
    return code


def make_unary_function_definitions(line: [str]) -> str:
    tok = line[0]
    op = line[1]
    in1 = line[2]

    code = f"{{{{TokenType::{tok}, " \
           f"PrimitiveTag::{in1.upper()}}}, " \
           f"[](Value::Data rhs){{" \
           f"return Value::Data({op} std::get<{in1}>(rhs));" \
           f"}}}}"

    return code


def create_bin_decls():
    with open("../etc/binOp_table.txt") as f:
        lines = map(lambda x: x.split(), filter(lambda x: x != '\n', f.readlines()))
        decls = "std::vector<std::tuple<TypeChecker::BinaryFuncDef, PrimitiveTag>> TypeChecker::binaryOperations {" + (
            ',\n\t'.join(map(make_function_declaration, lines))) + "};"
        print(decls)


def create_bin_defs():
    with open("../etc/binOp_table.txt") as f:
        lines = map(lambda x: x.split(), filter(lambda x: x != '\n', f.readlines()))
        defs = "std::vector<std::tuple<Interpreter::BinaryFuncDef, Interpreter::BinaryFuncDec>> " \
               "Interpreter::binaryOps = {" + (',\n\t'.join(map(make_function_definitions, lines))) + "};"
        print(defs)


def create_unr_decls():
    with open("../etc/unrOp_table.txt") as f:
        lines = map(lambda x: x.split(), filter(lambda x: x != '\n', f.readlines()))
        defs = "std::vector<std::tuple<TypeChecker::UnaryFuncDef, PrimitiveTag>> " \
               "TypeChecker::unaryOps = {" + (',\n\t'.join(map(make_unary_function_declaration, lines))) + "};"
        print(defs)


def create_unr_defs():
    with open("../etc/unrOp_table.txt") as f:
        lines = map(lambda x: x.split(), filter(lambda x: x != '\n', f.readlines()))
        defs = "std::vector<std::tuple<Interpreter::UnaryFuncDef, Interpreter::UnaryFuncDec>> " \
               "Interpreter::unaryOps = {" + (',\n\t'.join(map(make_unary_function_definitions, lines))) + "};"
        print(defs)


if __name__ == "__main__":
    create_unr_decls()
