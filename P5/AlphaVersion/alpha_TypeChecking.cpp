/* PROGRAMMER: Justin Wade */
/* PROGRAM: Project 5: TypeChecking.cpp */
/* DUE: Wednesday, October 28th, 2020 */
/* INSTRUCTOR: Dr. Zhijiang Dong */

#include "TypeChecking.h"
#include <iostream>
#include <sstream>

using namespace symbol;


namespace semantics
{
	const string		TypeChecking::breakSign = "breaksign";

	//insert a variable into the var/function symbol table
	void TypeChecking::insertVar(string name, symbol::SymTabEntry entry)
	{
		string			msg;
		stringstream	ss(msg);

		if (env.getVarEnv()->localContains(name))
		{
			symbol::SymTabEntry		old = env.getVarEnv()->lookup(name);
			ss << "variable " << name << " is already defined at line " << old.node->getLineno();
			error(entry.node, ss.str());

		}
		else
			env.getVarEnv()->insert(name, entry);
	}

	//insert a function into the var/function symbol table
	void TypeChecking::insertFunc(string name, symbol::SymTabEntry entry)
	{
		string			msg;
		stringstream	ss(msg);

		if (env.getVarEnv()->localContains(name))
		{
			symbol::SymTabEntry		old = env.getVarEnv()->lookup(name);
			ss << "function " << name << " is already defined at line " << old.node->getLineno();
			error(entry.node, ss.str());
		}
		else
			env.getVarEnv()->insert(name, entry);
	}

	//insert a type into the type symbol table
	void TypeChecking::insertType(string name, symbol::SymTabEntry entry)
	{
		string			msg;
		stringstream	ss(msg);

		if (env.getTypeEnv()->localContains(name))
		{
			symbol::SymTabEntry		old = env.getTypeEnv()->lookup(name);
			ss << "variable " << name << " is already defined at line " << old.node->getLineno();
			error(entry.node, ss.str());

		}
		else
			env.getTypeEnv()->insert(name, entry);
	}

	const types::Type* TypeChecking::visit(const Absyn* v)
	{
		if (dynamic_cast<const Exp*>(v) != NULL)
			return visit(dynamic_cast<const Exp*>(v));
		else if (dynamic_cast<const Var*>(v) != NULL)
			return visit(dynamic_cast<const Var*>(v));
		else if (dynamic_cast<const Dec*>(v) != NULL)
			return visit(dynamic_cast<const Dec*>(v));
		else
			throw runtime_error("invalid node");
	}

	const types::Type* TypeChecking::visit(const Exp* e)
	{
		if (dynamic_cast<const OpExp*>(e) != NULL)			return visit((const OpExp*)e);
		else if (dynamic_cast<const VarExp*>(e) != NULL)	return visit((const VarExp*)e);
		else if (dynamic_cast<const NilExp*>(e) != NULL)	return visit((const NilExp*)e);
		else if (dynamic_cast<const IntExp*>(e) != NULL)	return visit((const IntExp*)e);
		else if (dynamic_cast<const StringExp*>(e) != NULL) return visit((const StringExp*)e);
		else if (dynamic_cast<const CallExp*>(e) != NULL)	return visit((const CallExp*)e);
		//		else if (dynamic_cast<const RecordExp*>(e) != NULL) return visit((const RecordExp*)e);
		else if (dynamic_cast<const SeqExp*>(e) != NULL)	return visit((const SeqExp*)e);
		else if (dynamic_cast<const AssignExp*>(e) != NULL) return visit((const AssignExp*)e);
		else if (dynamic_cast<const IfExp*>(e) != NULL)		return visit((const IfExp*)e);
		else if (dynamic_cast<const WhileExp*>(e) != NULL)	return visit((const WhileExp*)e);
		else if (dynamic_cast<const ForExp*>(e) != NULL)	return visit((const ForExp*)e);
		else if (dynamic_cast<const BreakExp*>(e) != NULL)	return visit((const BreakExp*)e);
		else if (dynamic_cast<const LetExp*>(e) != NULL)	return visit((const LetExp*)e);
		else if (dynamic_cast<const ArrayExp*>(e) != NULL)	return visit((const ArrayExp*)e);
		else throw new runtime_error("ExpType.visit(Exp*)");
	}

	const types::Type* TypeChecking::visit(const Var* v)
	{
		if (dynamic_cast<const SimpleVar*>(v) != NULL)			return visit((const SimpleVar*)v);
		//		else if (dynamic_cast<const FieldVar *>(v) != NULL)		return visit((const FieldVar *) v);
		else if (dynamic_cast<const SubscriptVar*>(v) != NULL) return visit((const SubscriptVar*)v);
		else throw new runtime_error("ExpType.visit(Var*)");
	}

	const types::Type* TypeChecking::visit(const Ty* t)
	{
		if (dynamic_cast<const NameTy*>(t) != NULL)			return visit((const NameTy*)t);
		else if (dynamic_cast<const ArrayTy*>(t) != NULL)		return visit((const ArrayTy*)t);
		//		else if (dynamic_cast<const RecordTy *>(t) != NULL)		return visit((const RecordTy *)t);
		else throw new runtime_error("ExpType.visit(Ty*)");
	}

	const types::Type* TypeChecking::visit(const Dec* d)
	{
		if (dynamic_cast<const TypeDec*>(d) != NULL)			return visit((const TypeDec*)d);
		else if (dynamic_cast<const VarDec*>(d) != NULL)		return visit((const VarDec*)d);
		//		else if (dynamic_cast<const FunctionDec *>(d) != NULL)	return visit((const FunctionDec *)d);
		else throw new runtime_error("ExpType.visit(Dec*)");
	}

	const types::Type* TypeChecking::visit(const SimpleVar* v)
	{
		/* check if the variable is defined by looking up the symbol table*/
		if (env.getVarEnv()->contains(v->getName()))
		{
			return env.getVarEnv()->lookup(v->getName()).info;
		}
		else
		{
			error(v, "undefined variable");
			return NULL;
			//const types::Type* t = env.getVarEnv()->lookup(v->getName()).info->actual();
			//return t;

		}
	}

	const types::Type* TypeChecking::visit(const SubscriptVar* v)
	{
		/* check both the variable and index */
		visit(v->getIndex());
		return visit(v->getVar());
	}

	const types::Type* TypeChecking::visit(const OpExp* e)
	{
		/* check both operands */
		const types::Type* t1 = visit(e->getLeft());
		const types::Type* t2 = visit(e->getRight());

		if (t1 == t2) {
			return t1;
		}
		else {
			error(e, "semantic error: inequivalent types");
			return NULL;
		}
	}

	const types::Type* TypeChecking::visit(const VarExp* e)
	{
		/* check the variable */
		return visit(e->getVar());

	}

	const types::Type* TypeChecking::visit(const NilExp* e)
	{
		//don't need to do anything
		return NULL;
	}

	const types::Type* TypeChecking::visit(const IntExp* e)
	{
		//return visit(e->getValue());
		return NULL;
	}

	const types::Type* TypeChecking::visit(const StringExp* e)
	{
		//don't need to do anything
		return NULL;
	}


	const types::Type* TypeChecking::visit(const CallExp* e)
	{
		/*
			step 1: check the function name
			step 2: check every argument expression
		*/

		string funcName = e->getFunc();
		if (env.getVarEnv()->contains(funcName)) {
			const types::Type* t = NULL;
			const ExpList* args = e->getArgs();
			while (args != NULL) {
				t = visit(args->getHead());
				args = args->getRest();
			}
			return t;
		}
		else {
			//Else funcName is not defined, report an error
			error(e, "undefined function name");
			return NULL;
		}
	}

	const types::Type* TypeChecking::visit(const SeqExp* e)
	{
		/*	check every expression in the sequence */
		const types::Type* t = NULL;
		const absyn::ExpList* list = e->getList();
		while (list != NULL) {
			t = visit(list->getHead());
			list = list->getRest();
		}
		return t;
	}

	//This function checks type compatibility in an assignment statement
	const types::Type* TypeChecking::visit(const AssignExp* e)
	{
		/* check both variable and expression */
		visit(e->getVar());
		return visit(e->getExp());

	}

	//This function tests for errors in an if statement
	const types::Type* TypeChecking::visit(const IfExp* e) {
		/* check test condition, then-clause, and else-clause (if exists) */

		const types::Type* t = visit(e->getTest());
		if (e->getTest() != NULL) {
			return t;
		}

		const types::Type* t1 = visit(e->getThenClause());
		if (e->getThenClause() != NULL) {
			return t1;
		}
		const types::Type* t2 = visit(e->getElseClause());
		if (e->getElseClause() != NULL) {
			return t2;
		}

		return NULL;
	}

	//This function checks for errors in while statements
	const types::Type* TypeChecking::visit(const WhileExp* e)
	{
		const types::Type* t = visit(e->getTest());
		if (e->getTest() != NULL) {
			return t;
		}

		return NULL;
	}

	//This function tests for errors in for loops
	const types::Type* TypeChecking::visit(const ForExp* e)
	{
		// Open scope
		env.getVarEnv()->beginScope();

		// Check
		const types::Type* t1 = visit(e->getVar());
		const types::Type* t2 = visit(e->getHi());
		const types::Type* t3 = visit(e->getBody());

		//Close scopes
		env.getVarEnv()->endScope();

		return NULL;
	}

	//This function checks for break expression errors
	const types::Type* TypeChecking::visit(const BreakExp* e)
	{
		return NULL;
	}

	//This function detects errors in let expressions
	const types::Type* TypeChecking::visit(const LetExp* e)
	{
		/*
			step 1: begin a new scope for type and variable/function symbol tables
			step 2: check every declaraion in the decl list
			step 3: check the body expression
			step 4: end the scope for both symbol tables
		*/

		// Open scopes
		env.getVarEnv()->beginScope();
		env.getTypeEnv()->beginScope();

		// Check all declarations
		const absyn::DecList* decs = e->getDecs();
		int i = 0;
		while (decs) {
			visit(decs->getHead());
			decs = decs->getRest();
		}

		// Check body expression
		visit(e->getBody());

		//Close scopes
		env.getVarEnv()->endScope();
		env.getTypeEnv()->endScope();

		return NULL;
	}

	//This function checks for errors with array declarations
	const types::Type* TypeChecking::visit(const ArrayExp* e)
	{
		/*
			step 1: check the type of the array by looking up the type symbol table
			step 2: check the size expression
			step 3: check the initial expression
		*/

		if (env.getTypeEnv()->contains(e->getType())) {

			visit(e->getSize());
			return visit(e->getInit());
		}
		else {
			error(e, "undefined array type");
			return NULL;
		}

		return NULL;

	}

	const types::Type* TypeChecking::visit(const VarDec* d) {
		/*
			step 1: insert the variable to the var/function symbol table
			step 2: if the type information is provided, check the type
			step 3: check the initial expression
		*/

		insertVar(d->getName(), symbol::SymTabEntry(env.getVarEnv()->getLevel(), NULL, d));

		if (d->getType())
			visit(d->getType());

		return visit(d->getInit());
	}

	const types::Type* TypeChecking::visit(const TypeDec* d)
	{
		/*
			step 1: insert the name of the new type to the type symbol table
			step 2: check the type information of the new type
		*/


		insertType(d->getName(), symbol::SymTabEntry(env.getTypeEnv()->getLevel(), NULL, d));
		return visit(d->getTy());
	}


	const types::Type* TypeChecking::visit(const NameTy* t)
	{
		/*
			step 1: check if the type name is defined by looking up the type symbol table
		*/

		if (env.getTypeEnv()->contains(t->getName())) {

			return env.getTypeEnv()->lookup(t->getName()).info;
		}
		else {
			error(t, "undefined type");
			return NULL;
		}
	}

	const types::Type* TypeChecking::visit(const ArrayTy* t)
	{
		/*
			step 1: Check the name of the array type by looking up the type symbol table
		*/
		if (env.getTypeEnv()->contains(t->getName())) {

			return new types::ARRAY(env.getTypeEnv()->lookup(t->getName()).info);
		}
		else {
			error(t, "undefined type");
			return NULL;
		}
	}



} // end of namespace semantics