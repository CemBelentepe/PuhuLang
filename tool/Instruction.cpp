#include "Instruction.h"
#include "InstVisitor.hpp"

void InstConst::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstCast::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstAdd::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstSub::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstMul::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstDiv::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstNeg::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstMod::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstBit::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstNot::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstInc::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstLess::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstLte::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstGreat::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstGte::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstEq::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstNeq::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

