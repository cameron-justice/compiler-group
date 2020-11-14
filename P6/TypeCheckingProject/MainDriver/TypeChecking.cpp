/*
PROGRAMMER: Hunter McGee
LEADER: Cameron Justice
TESTER: Justin Wade
INSTRUCTOR: DR. DONG
PROGRAM: PROJECT 6
DUE DATE: 11/13/2020
*/

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
		if (!(env.getVarEnv()->contains(v->getName())))
		{
			error(v, "undefined variable");
			//undeclared variables is treated as INT variable
			insertVar(v->getName(), SymTabEntry(env.getVarEnv()->getLevel(),
				new types::INT(),
				v));
			return new types::INT();
		}
		else
		{
			const types::Type* t = env.getVarEnv()->lookup(v->getName()).info->actual();

			if (dynamic_cast<const types::FUNCTION*>(t) != NULL)
			{
				error(v, "function with the same name exists");
				//undeclared variables is treated as INT variable
				return new types::INT();
			}
			return t;
		}
	}

	/*	const types::Type* TypeChecking::visit(const FieldVar *v)
		{
			//add your implementation here
			//syntax: lvalue.fieldname

			Algorithm:
				1.	Perform type checking on lvalue, and get its data type (say t)
				2.	if t is not a record type
						report an error
						return INT
					else
				3.		cast t to RECORD *;
				4.		For each filed in the RECORD definition
							if the fieldname is the one we are looking for
								return the type of current field
				5.		report an error for non-existing field
				6.		return INT.
		}
	*/

	//This function checks if the subscript is an int and if the lvalue is an array type
	//syntax: lvalue[index_exp]
	const types::Type* TypeChecking::visit(const SubscriptVar *v)
	{
		// Perform type checking on lvalue, and get its data type (say t)
		const types::Type* t = visit(v->getVar());

		// if t is an array type
		if (dynamic_cast<const types::ARRAY*>(t) != NULL) {
			// check the type of the index expression
			const types::Type* te = visit(v->getIndex());

			if (dynamic_cast<const types::INT*>(te) == NULL) {
				error(v, "index is not of integer type");
				return new types::INT();
			}

			// return array element type
			return (dynamic_cast<const types::ARRAY*>(t))->getElement()->actual();
		}
		else { // if t is not an array type
			error(v, "lvalue is not of array type");
			return new types::INT();
		}
	}

	//This function checks for valid operator expressions
	// syntax: left_exp Operator right_exp
	const types::Type* TypeChecking::visit(const OpExp *e)
	{
		//	Perform type checking on left_exp, right_exp
		const types::Type*	lt = visit(e->getLeft());
		const types::Type*	rt = visit(e->getRight());

		OpExp::OpType op = e->getOper();
		
		// if Operator is one of +, -, *, /
		if (op == OpExp::OpType::PLUS || op == OpExp::OpType::MINUS || op == OpExp::OpType::MUL || op == OpExp::OpType::DIV)
		{
			if (dynamic_cast<const types::INT *>(lt) == NULL)
			{
				error(e, "The left operand must be of type: INT");
			}
			if (dynamic_cast<const types::INT *>(rt) == NULL)
			{
				error(e, "The right operand must be of type: INT");
			}
			return new types::INT();
		}
		// if Operator is one of >, >=, <, <=
		else if (op == OpExp::OpType::GT || op == OpExp::OpType::LT || op == OpExp::OpType::GE || op == OpExp::OpType::LE)
		{
			if (dynamic_cast<const types::INT *>(lt) == NULL && dynamic_cast<const types::STRING*>(lt) == NULL)
			{
				error(e, "The left operand must be of type: INT or STRING");
			}
			if (dynamic_cast<const types::INT *>(rt) == NULL && dynamic_cast<const types::STRING*>(rt) == NULL)
			{
				error(e, "The right operand must be of type: INT or STRING");
			}
			if (lt->coerceTo(rt) == false || rt->coerceTo(lt) == false)
			{
				error(e, "The two operands are not compatible");
			}
			return new types::INT();
		}
		else // Operator is <>, =
		{
			if (dynamic_cast<const types::INT *>(lt) == NULL && dynamic_cast<const types::STRING *>(lt) == NULL &&
				dynamic_cast<const types::RECORD *>(lt) == NULL && dynamic_cast<const types::ARRAY *>(lt) == NULL &&
				dynamic_cast<const types::NIL *>(lt) == NULL)
			{
				error(e, "The left operand must be of type: INT, STRING, RECORD, ARRAY, or NIL");
			}
			if (dynamic_cast<const types::INT *>(rt) == NULL && dynamic_cast<const types::STRING *>(rt) == NULL &&
				dynamic_cast<const types::RECORD *>(rt) == NULL && dynamic_cast<const types::ARRAY *>(rt) == NULL 
				&& dynamic_cast<const types::NIL *>(rt) == NULL)
			{
				error(e, "The right operand must be of type: INT, STRING, RECORD, ARRAY, or NIL");
			}
			if (lt->coerceTo(rt) == false || rt->coerceTo(lt) == false)
			{
				error(e, "The two operands are not compatible");
			}
			if (dynamic_cast<const types::NIL *>(lt) != NULL && dynamic_cast<const types::NIL *>(rt) != NULL)
			{
				error(e, "The two operands cannot both be of type: NIL");
			}
			return new types::INT();
		}
	}

	const types::Type* TypeChecking::visit(const VarExp *e)
	{
		const types::Type*		t = visit(e->getVar());
		return t->actual();
	}

	const types::Type* TypeChecking::visit(const NilExp *e)
	{
		return new types::NIL();
	}

	const types::Type* TypeChecking::visit(const IntExp *e)
	{
		return new types::INT();
	}

	const types::Type* TypeChecking::visit(const StringExp *e)
	{
		return new types::STRING();
	}

	//This function checks for valid function calls
	const types::Type* TypeChecking::visit(const CallExp *e)
	{
		//check if fname is defined by looking up the symbol table
		string fname = e->getFunc();
		if (!( env.getVarEnv()->contains(fname) ))
		{
			//if fname is not defined, report an error, and return INT
			error(e, "function name is undefined");
			return new types::INT();
		}
		else
		{
			//if fname is defined, get its data type, say t
			const types::Type *t = env.getVarEnv()->lookup(fname).info->actual();

			//if t is not FUNCTION, report an error and return INT;
			if (dynamic_cast<const types::FUNCTION *>(t) == NULL)
			{
				error(e, "function name is already defined as variable");
				return new types::INT();
			}
			else
			{
				
				t = dynamic_cast<const types::FUNCTION*>(t); // cast t to FUNCTION*
				// Let c_arg refers to the first argument (argument list can be found in CallExp)
				const ExpList* c_arg = e->getArgs();
				// Let c_par refers to the first parameter(parameter list can be found in FUNCTION)
				vector <const types::Type*> c_par;
				c_par = dynamic_cast <const types::FUNCTION*>(env.getVarEnv()->lookup(e->getFunc()).info)->getFieldType();
				vector <const types::Type*>::iterator c_iter = c_par.begin();
				/*			repeat as long as both c_arg and c_par are not NULL
				perform type checking on c_arg and get its type, see ta
				if (ta is not compatible with type of c_par)
				report an error
				update c_arg to refer to next argument
				update c_par to refer to next parameter
				end repeat	*/
				while (c_arg != NULL && c_iter != c_par.end())
				{
					const types::Type* ta = visit(c_arg->getHead());
					if (!ta->coerceTo((*c_iter)))
					{
						error(e, "the type of exp_i doesn't match the type of param_i");
					}
					c_iter++;
					c_arg = c_arg->getRest();
				}
				// too many arguments
				if (c_arg == NULL && c_iter != c_par.end())
				{
					error(e, "the # of expressions doesn't match the # of parameters");
				}
				// too few arguments
				if (c_arg != NULL && c_iter == c_par.end())
				{
					error(e, "the # of expressions doesn't match the # of parameters");
				}
				return (dynamic_cast<const types::FUNCTION*>(t)->getResult()->actual());
			}
		}
	}

	/*	const types::Type* TypeChecking::visit(const RecordExp *e)
	{
		//add your implementation here
		//syntax: record_type {f1=exp1, ..., fn=expn}
	}
	*/

	// syntax: exp1; exp2; exp3; ....; expn
	// Type checking in an expression sequence
	const types::Type* TypeChecking::visit(const SeqExp *e)
	{
		const types::Type* t = nullptr;
		const ExpList* el = e->getList();

		// visit each expression in ExpList
		while (el != NULL)
		{
			t = visit(el->getHead());
			el = el->getRest();
		}
		return t;
	}

	// syntax: lvalue := exp
	// Type checking in assignment statements
	const types::Type* TypeChecking::visit(const AssignExp *e)
	{
		const types::Type*	t = visit(e->getVar()); // get lvalue type
		const types::Type*	te = visit(e->getExp()); // get expression type

		// evaluate whether types are compatible
		if (t->coerceTo(te) == false || te->coerceTo(t) == false)
		{
			error(e, "lvalue type and expression type are not compatible");
		}
		return new types::VOID();
	}

	//syntax: if test then
	//				exp1
	//			else
	//				exp2
	// Type checking in if-statements
	const types::Type* TypeChecking::visit(const IfExp *e)
	{
		// type checking on test component
		const types::Type*	t = visit(e->getTest());
		if (dynamic_cast<const types::INT *>(t) == NULL)
		{
			error(e, "test must be of type: INT");
		}

		// type checking on then-clause, else-clause
		const types::Type*	t1 = visit(e->getThenClause());

		if (e->getElseClause() == NULL) // no else-clause
		{
			if (dynamic_cast<const types::VOID *>(t1) == NULL)
			{
				error(e, "then-clause must be of type: VOID");
			}
			return new types::VOID();
		}
		else // else-clause present
		{
			const types::Type*	t2 = visit(e->getElseClause());

			// check to see if then-clause and else-clause are of compatible types
			if (t1->coerceTo(t2))
				return t2->actual();
			else if (t2->coerceTo(t1))
				return t1->actual();
			else // clauses are of different types
			{
				error(e, "these statements are not of compatible types");
				return t1->actual();
			}
		}
	}

	// syntax: while test do exp1
	// Type checking for while-statements
	const types::Type* TypeChecking::visit(const WhileExp *e)
	{
		// type checking on test
		const types::Type*	t = visit(e->getTest());
		if (dynamic_cast<const types::INT *>(t) != NULL)
		{
			error(e, "test must be of type: INT");
		}

		// type checking on body of while-statement
		const types::Type*	t1 = visit(e->getBody());
		if (dynamic_cast<const types::VOID *>(t) != NULL)
		{
			error(e, "body must be of type: VOID");
		}
		return new types::VOID();
	}

	//syntax: for vname := exp1 to exp2 do exp3
	// Type checking for for-loop expressions
	const types::Type* TypeChecking::visit(const ForExp *e)
	{
		string vname;
		env.getVarEnv()->beginScope(); // open scope for for-loop

		// type checking on for-loop control variable, add it to the symbol table
		const types::Type* t = visit(e->getVar());
		vname = e->getVar()->getName();
		const types::Type* t1 = env.getVarEnv()->lookup(vname).info->actual();
		if (dynamic_cast<const types::INT*>(t1) == NULL)
			error(e, "first exp must be of type: INT");

		// type checking on high value (upper bound of loop)
		const types::Type* t2 = visit(e->getHi());
		if (dynamic_cast<const types::INT*>(t2) == NULL)
			error(e, "high exp must be of type: INT");

		// type checking on body of for-loop
		const types::Type* t3 = visit(e->getBody());
		if (dynamic_cast<const types::VOID*>(t3) == NULL)
			error(e, "body of for loop should produce no value");

		env.getVarEnv()->endScope(); // close scope for for-loop

		return new types::VOID();
	}

	// Type checking on break expression
	const types::Type* TypeChecking::visit(const BreakExp *e)
	{
		return new types::VOID();
		/*Algorithm:
		return VOID if you don't want bonus points.
		*/
	}

	// syntax: let decls in exps end
	// Type checking for let-expressions
	const types::Type* TypeChecking::visit(const LetExp *e)
	{
		// open scope for type and variables
		env.getVarEnv()->beginScope();
		env.getTypeEnv()->beginScope();

		// declaration list
		if (e->getDecs() != NULL)
		{
			const DecList* dec_list = e->getDecs();
			const Dec* dec1 = dec_list->getHead();

			// for each declaration in the list
			while (dec_list != NULL)
			{
				visit(dec1);
				dec_list = dec_list->getRest();
				if (dec_list != NULL)
				{
					dec1 = dec_list->getHead();
				}
			}
		}

		// body of let-exp
		const types::Type* t = visit(e->getBody());
		env.getVarEnv()->endScope();
		env.getTypeEnv()->endScope();
		return t;
	}

	// syntax: array_type [exp1] of exp2
	// Type checking for array expressions
	const types::Type* TypeChecking::visit(const ArrayExp *e)
	{
		const types::Type* t = nullptr;
		
		// check if array_type exists in symbol table
		if (!env.getTypeEnv()->contains(e->getType()))
		{
			error(e, "array type does not exist");
			t = new types::ARRAY(new types::INT());
		}
		else // array_type does exist
		{
			t = env.getTypeEnv()->lookup(e->getType()).info->actual();

			// array_type is already defined in symbol table, but not as an array_type
			if (dynamic_cast <const types::ARRAY *>(t) == NULL)
			{
				error(e, "array_type is not an ARRAY");
				t = new types::ARRAY(new types::INT());
			}
		}

		// type checking on exp1 (size)
		const types::Type* t1 = visit(e->getSize());
		if (dynamic_cast <const types::INT*>(t1) == NULL)
		{
			error(e, "array size must be of type: INT");
			t = new types::ARRAY(new types::INT());
		}
		// type checking on exp2 (init value)
		const types::Type* t2 = visit(e->getInit());
		if (!t2->coerceTo(dynamic_cast <const types::ARRAY*>(t)->getElement()))
		{
			error(e, "init value is not compatible with type of elements in the array");
			t = new types::ARRAY(new types::INT());
		}
		return t;
	}

	/*	const types::Type* TypeChecking::visit(const FunctionDec *d)
	{
	//add your implementation
	//syntax: function fname(p1:type1, ..., pn:typen) : rtype = exp1
	}
	*/

	// syntax: var vname : Type = exp1
	// Type checking for variable declarations
	const types::Type* TypeChecking::visit(const VarDec *d)
	{
		const types::Type* type;
		const types::Type* tt;
		const types::Type* t1;
		string			msg;
		stringstream	ss(msg);
		string vname = d->getName();
		

		// variable is defined locally
		if (env.getVarEnv()->localContains(vname)) {
			symbol::SymTabEntry		old = env.getVarEnv()->lookup(vname);
			ss << "variable " << vname << " is already defined at line " << old.node->getLineno();
			error(d, ss.str());
		}
		else if (d->getType() != NULL) // TypeName is provided
		{
			tt = new types::INT();

			// TypeName does not exist in symbol table
			if (!env.getTypeEnv()->contains(d->getType()->getName()))
				error(d, "undefined type name");
			else
			{
				tt = env.getTypeEnv()->lookup(d->getType()->getName()).info->actual();
				t1 = visit(d->getInit());

				// type checking between TypeName and expression type
				if (!t1->coerceTo(tt))
					error(d->getInit(), "incompatible types in variable declaration expression");

				types::NAME* v = new types::NAME(d->getName());
				type = visit(d->getType());
				v->bind((types::Type*)type);
				insertVar(d->getName(), SymTabEntry(env.getVarEnv()->getLevel(), v, d));
			}

		}
		else // TypeName is not provided
		{
			t1 = visit(d->getInit());

			// the exp is not nil
			if (dynamic_cast <const types::NIL*>(t1) == NULL)
			{
				types::NAME* v = new types::NAME(d->getName());
				type = new types::INT();
				v->bind((types::Type*)type);
				insertVar(d->getName(), SymTabEntry(env.getVarEnv()->getLevel(), v, d));
			}
			else // the exp IS NIL
			{
				error(d, "cannot initialize variable to nil without providing TypeName");
			}
		}
		return NULL;
	}

	// Type checking for Type declarations
	const types::Type* TypeChecking::visit(const TypeDec *d)
	{
		const types::Type* type;
		types::NAME* name = new types::NAME(d->getName());

		//find type redefine in the consecutive type declarations 
		const absyn::TypeDec* td = d->getNext();
		while (td != NULL) {
			if (td->getName() == d->getName())
				error(td, "type redefined");
			td = td->getNext();
		}

		name->bind(new types::INT());	//just for avoiding the self loop, later it
							// will be replaced by actual value

		insertType(d->getName(), SymTabEntry(env.getVarEnv()->getLevel(), name, d));

		if (d->getNext() != NULL)
			visit(d->getNext());
		type = visit(d->getTy());

		name->bind((types::Type*)type);
		env.getTypeEnv()->lookup(d->getName()) = SymTabEntry(env.getVarEnv()->getLevel(),
			name,
			d);

		if (name->isLoop()) {
			error(d, "illegal cycle found in type definition");
			name->bind(new types::INT());
		}
		return NULL;
	}

	const types::Type* TypeChecking::visit(const NameTy *t)
	{
		if (!(env.getTypeEnv()->contains(t->getName())))
		{
			error(t, "undefined type name");
			return new types::INT();
		}

		return env.getTypeEnv()->lookup(t->getName()).info;
	}

	/*	const types::Type* TypeChecking::visit(const RecordTy *t)
		{
			const absyn::FieldList*		fl = t->getFields();

			if ( fl == NULL ) {
				//empty record
				return new types::RECORD( "", NULL, NULL );
			}
			else {
				types::RECORD		*r = NULL, *tail = NULL, *head = NULL;

				while ( fl != NULL ) {
					if ( !env.getTypeEnv()->contains(fl->getType()) )
						r = new types::RECORD(	fl->getName(),
												new types::INT(),
												NULL );
					else
						r = new types::RECORD(	fl->getName(),
												env.getTypeEnv()->lookup(fl->getType()).info,
												NULL );
					if ( head == NULL )
						head = tail = r;
					else {
						tail->setRest(r);
						tail = r;
					}
					fl = fl->getRest();
				}
				return head;
			}
		}
	*/

	// Type checking for array type
	const types::Type* TypeChecking::visit(const ArrayTy *t)
	{
		if (!(env.getTypeEnv()->contains(t->getName())))
		{
			error(t, "undefined type");
			return new types::INT();
		}

		return new types::ARRAY(env.getTypeEnv()->lookup(t->getName()).info);
	}
} // end of namespace semantics