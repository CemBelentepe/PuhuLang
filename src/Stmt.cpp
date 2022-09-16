//
// Created by cembelentepe on 08/09/22.
//

#include "Stmt.h"
#include "AstVisitor.h"

template<typename T>
T Stmt::accept(StmtVisitor<T>* visitor)
{
	doAccept(visitor);
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
VISITOR_ACCEPT(StmtBlock)
VISITOR_ACCEPT(StmtIf)
VISITOR_ACCEPT(StmtWhile)
VISITOR_ACCEPT(StmtFor)
VISITOR_ACCEPT(StmtReturn)

VISITOR_ACCEPT(StmtDeclVar)
VISITOR_ACCEPT(StmtDeclFunc)

#undef VISITOR_ACCEPT

