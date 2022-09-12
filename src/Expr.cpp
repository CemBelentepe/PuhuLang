//
// Created by cembelentepe on 08/09/22.
//

#include "AstVisitor.h"
#include "Expr.h"

template<>
void Expr::accept(ExprVisitor<void>* visitor)
{
	doAccept(visitor);
}

template<>
std::shared_ptr<Type> Expr::accept(ExprVisitor<std::shared_ptr<Type>>* visitor)
{
	doAccept(visitor);
	return visitor->result;
}

#define VISITOR_ACCEPT(T)\
void T::doAccept(ExprVisitorBase* visitor)\
{\
	visitor->visit(this);\
}

VISITOR_ACCEPT(ExprBinary)
VISITOR_ACCEPT(ExprUnary)
VISITOR_ACCEPT(ExprLiteral)
VISITOR_ACCEPT(ExprVarGet)

#undef VISITOR_ACCEPT