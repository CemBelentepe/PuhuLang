//
// Created by cembelentepe on 08/09/22.
//

#include "Stmt.h"
#include "AstVisitor.h"

template<typename T>
T Stmt::accept(StmtVisitor<T>* visitor)
{
	do_accept(visitor);
	return visitor->result;
}

template<>
void Stmt::accept(StmtVisitor<void>* visitor)
{
	doAccept(visitor);
}

#define VISITOR_ACCEPT(T)\
void T::doAccept(StmtVisitorBase* visitor)\
{\
	visitor->visit(this);\
}

VISITOR_ACCEPT(StmtExpr)

#undef VISITOR_ACCEPT