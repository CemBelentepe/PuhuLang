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

void InstGetGlobal::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstSetGlobal::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstGetLocal::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstSetLocal::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstAlloc::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstFree::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstGetDeref::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstSetDeref::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstGetDerefOff::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstSetDerefOff::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstCall::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstPop::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstPush::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstReturn::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstJump::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

void InstLabel::accept(InstVisitor* visitor)
{
	visitor->visit(this);
}

