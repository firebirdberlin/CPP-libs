//		./Symbolic/Node.cpp
//
//		Copyright 2011 Fabian Paul <fab@itp.tu-berlin.de>
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
//
//		This program is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with this program; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.
#ifndef NODE_CPP
#define NODE_CPP

/** \file Node.cpp
 * \brief Defines methods of Node-class
 *
 *	This class defines the methods of the Node-class, together with constructors
 *	and destructor.
 */

#include <cmath>
#include <iostream>
#include <sstream>

#include "Symbolic_Exception.hpp"
#include "Node.hpp"

namespace Symbolic {

/** name: Node::Node
 *	Standardconstructor, initializes the pointers with NULL and the other
 *	members with default values.
 */
Node::Node()
	:	opcode(OP_BAD), direct(0), left(NULL), right(NULL) {
	// empty
}

/** name: Node::Node
 *	Constructor, takes an Opcode, which is used to initialize the opcode-member.
 *	The pointers are initialized with NULL and direct gets a default value.
 * \param code: Type is Opcode, is used to initialize the opcode-member.
 */
Node::Node(Opcode const code)
	:	opcode(code), direct(0), left(NULL), right(NULL) {
	// empty
}

/** name: Node::Node
 *	Constructor, takes an Opcode, which is used to initialize the opcode-member.
 *	The pointers are initialized with NULL and direct gets a default value.<br>
 *	In the body are then copies created of the given nodes. These are stored
 *	with the pointers.
 * \param code: Type is Opcode, is used to initialize the opcode-member.
 * \param l:
 * \param r: References to Nodes, from these are created copies for the corres-
 *				ponding pointers.
 */
Node::Node(Opcode const code, Node const &l, Node const &r)
	:	opcode(code), direct(0), left(NULL), right(NULL) {
	this->left		= new Node(l);
	this->right		= new Node(r);
}

/** name: Node::Node
 *	Constructor, takes an Opcode, which is used to initialize the opcode-member.
 *	The pointers are initialized with NULL and direct gets a default value.<br>
 *	In the body is then a copy created of the given node. These is stored  with
 *	the right-pointer.
 * \param code: Type is Opcode, is used to initialize the opcode-member.
 * \param branch: Reference to a Node, from these is created a copy for the
 *				right-pointer.
 */
Node::Node(Opcode const code, Node const &branch)
	:	opcode(code), direct(0), left(NULL), right(NULL) {
	this->right		= new Node(branch);
}

/** name: Node::Node
 *	Constructor, takes an Opcode, which is used to initialize the opcode-member
 *	and a double which is used to initialize the direct-member. The pointers
 *	are set to NULL.
 * \param code: Type is Opcode, is used to initialize the opcode-member.
 * \param dir: Double value, which is used to initialize the direct-member.
 */
Node::Node(Opcode const code, double const dir)
	:	opcode(code), direct(dir), left(NULL), right(NULL) {
	// empty
}

/** name: Node::Node
 *	Copyconstructor, makes this object to a deep copy of the other Node.
 * \param other: Reference to the node that should be copied.
 */
Node::Node(Node const & other)
	:	opcode(other.GetOpcode()), direct(other.GetDirect()), left(NULL), right(NULL) {
	if(NULL != other.GetLeft())		left  = new Node(*(other.GetLeft()));
	if(NULL != other.GetRight())	right = new Node(*(other.GetRight()));
}

/** name: Node::Node
 *	Destructor, deletes the childs and sets the pointers to NULL.
 */
Node::~Node() {
	delete left;
	left = NULL;
	delete right;
	right = NULL;
}

/** name: Node::Copy
 *	\brief Makes this object to a copy of another.
 *
 *	This method makes this object to a deep copy of another. Thereby are childs
 *	which exists deleted, before new ones are created.
 * \param other: Reference to a Node which should be copied (including childs).
 * \return void
 */
void Node::Copy(Node const & other) {
	if((&(*this)) == (&other))	return;

	// Set the variables.
	opcode = other.GetOpcode();
	direct = other.GetDirect();

	// Save the childs for the case they are part of this tree.
	Node const * ltemp(this->left), * rtemp(this->right);

	// Set this childs to NULL.
	this->left = NULL;
	this->right = NULL;

	// Create the new childs.
	if(NULL != other.GetLeft())		left  = new Node(*(other.GetLeft()));
	if(NULL != other.GetRight())	right = new Node(*(other.GetRight()));

	// Delete the old childs.
	if(NULL != ltemp)	{delete ltemp; ltemp = NULL;}
	if(NULL != rtemp)	{delete rtemp; rtemp = NULL;}
}

/** name: Node::operator=
 *	\brief Uses Copy-method.
 *
 *	This overloaded assignment operator uses the Copy-method and returns a
 *	reference to this object.
 * \return Reference to this object.
 */
Node const & Node::operator=(Node const &other) {
	this->Copy(other);
	return (*this);
}

/** name: Node::Set
 *	\brief Set Opcode and childs.
 *
 *	This method sets the Opcode and the childs of this object. If there already
 *	childs, these are deleted before creating the new one.
 * \param code: New value for the opcode.
 * \param l:
 * \param r: References to Nodes, which should be used as left /right child
 *				respectivley (they are copied).
 * \return void
 */
void Node::Set(Opcode const code, Node const &l, Node const &r) {
	opcode = code;

	// Save the childs for the case they are part of this tree
	Node const * ltemp(this->left), * rtemp(this->right);

	// Create the new childs.
	this->left	= new Node(l);
	this->right	= new Node(r);

	// Delete the old childs.
	if(NULL != ltemp)	{delete ltemp; ltemp = NULL;}
	if(NULL != rtemp)	{delete rtemp; rtemp = NULL;}
}

/** name: Node::Set
 *	\brief Set Opcode and right child.
 *
 *	This method sets the Opcode and the right child of this object. If there
 *	already childs, these are deleted before.
 * \param code: New value for the opcode.
 * \param branch: Reference to a Node, which should be used as right child
 *				(those is copied).
 * \return void
 */
void Node::Set(Opcode const code, Node const &branch) {
	opcode = code;

	// Save the childs for the case they are part of this tree
	Node const * ltemp(this->left), * rtemp(this->right);

	// Create the new right child.
	this->right		= new Node(branch);
	this->left		= NULL;

	// Delete the old childs.
	if(NULL != ltemp)	{delete ltemp; ltemp = NULL;}
	if(NULL != rtemp)	{delete rtemp; rtemp = NULL;}
}

/** \fn void Node::Set(Opcode const, double const)
 *	\brief Set opcode and direct.
 *
 *	This method sets the Opcode and direct-member. Already existing childs are
 *	deleted.
 * \param code: New value for the opcode.
 * \param dir: New value for the direct member.
 */
void Node::Set(Opcode const code, double const dir) {
	// Set the new values.
	opcode = code;
	direct = dir;

	// Delete the childs.
	if(NULL != this->left)	{delete this->left; this->left = NULL;}
	if(NULL != this->right)	{delete this->right; this->right = NULL;}
}

/** \fn void Node::Set(Opcode const)
 *	\brief Set opcode.
 *
 *	This method sets the Opcode. Already existing childs are deleted.
 * \param code: New value for the opcode.
 * \return void
 */
void Node::Set(Opcode const code) {
	// Set the opcode.
	opcode = code;

	// Delete the members.
	if(NULL != this->left) {delete this->left; this->left = NULL;}
	if(NULL != this->right) {delete this->right; this->right = NULL;}
}

/** \fn Node::ReduceTree()
 * \brief Tries to reduce a this tree.
 *
 *	This method will try to reduce this tree, which means to reduce the number
 *	of nodes. This is done by replacing a node with which has only constants or
 *	a variable and a constant as subnode by one node with a constant, e.g. 1 + 1
 *	will be replaced by 2. Thereby the algorhythm is used recursive.<br>
 *	Configuations that can be reduced:
 *	- a + - * / \% ^ b
 *	- n + 0, 0 + n
 *	- n - 0, n - n
 *	- n * 0, 0 * n, n * 1, 1 * n,
 *	- n / 1, 0 / n, n / n
 *	- n ^ 1
 *	- fct(a)
 *
 *	Thereby mean a,b some (numeric) constants, n is a subtree and fct is a known
 *	function (e.g. exp() or ln()).
 */
void Node::ReduceTree() {
	size_t ltype(0), rtype(0);

	if(NULL != this->left) {
		this->left->ReduceTree();
		Opcode const opl(left->GetOpcode());
		if(OP_CONST == opl) ltype = 1;
		if((OP_X == opl) or (OP_Y == opl) or (OP_Z == opl) or (OP_T == opl)) {
			ltype == 2;
		}
	}
	if(NULL != this->right) {
		this->right->ReduceTree();
		Opcode const opr(right->GetOpcode());
		if(OP_CONST == opr) rtype = 1;
		if((OP_X == opr) or (OP_Y == opr) or (OP_Z == opr) or (OP_T == opr)) {
			rtype == 2;
		}
	}

	switch(this->GetOpcode()) {
		case OP_PLUS:
			if((1 == ltype) and (1 == rtype)) {// a + b
				this->Set(OP_CONST, left->GetDirect() + right->GetDirect());
			} else if((rtype == 1) and (right->GetDirect() == 0.0)) {// n + 0
				this->Copy(*left);
			} else if((ltype == 1) and (left->GetDirect() == 0.0)) {// 0 + n
				this->Copy(*right);
			}
			break;
		case OP_MINUS:
			if((1 == ltype) and (1 == rtype)) {// a - b
				this->Set(OP_CONST, left->GetDirect() - right->GetDirect());
			} else if((rtype == 1) and (right->GetDirect() == 0)) {// n - 0
				this->Copy(*left);
			} else if(right->TreeEqualTo(*left)) {// n - n
				this->Set(OP_CONST, 0.0);
			}
			break;
		case OP_TIMES:
			if((1 == ltype) and (1 == rtype)) {// a * b
				this->Set(OP_CONST, left->GetDirect() * right->GetDirect());
			} else if(		((rtype == 1) and (right->GetDirect() == 0.0))
			                or	((ltype == 1) and (left->GetDirect() == 0.0))) {// n * 0 or 0 * n
				this->Set(OP_CONST, 0.0);
			} else if((ltype == 1) and (left->GetDirect() == 1.0)) {// 1 * n
				this->Copy(*right);
			} else if((rtype == 1) and (right->GetDirect() == 1.0)) {// n * 1
				this->Copy(*left);
			}
			break;
		case OP_DIVIDES:
			if((1 == ltype) and (1 == rtype)) {// a / b
				this->Set(OP_CONST, left->GetDirect() / right->GetDirect());
			} else if((ltype == 1) and (left->GetDirect() == 0.0)) {// 0 / n
				this->Set(OP_CONST, 0.0);
			} else if((rtype == 1) and (right->GetDirect() == 1.0)) {// n / 1
				this->Copy(*left);
			} else if(right->TreeEqualTo(*left)) {// n / n
				this->Set(OP_CONST, 1.0);
			}
			break;
		case OP_MODULO:
			if((1 == ltype) and (1 == rtype)) {// a % b
				this->Set(OP_CONST, fmod(left->GetDirect(), right->GetDirect()));
			}
			break;
		case OP_EXPONENTATION:
			if((1 == ltype) and (1 == rtype)) {// a ^ b
				this->Set(OP_CONST, pow(left->GetDirect(), right->GetDirect()));
			} else if((1 == rtype) and (1.0 == right->GetDirect())) {// n^1
				this->Copy(*left);
			}
			break;
		case OP_EXP:
			if(1 == rtype)	this->Set(OP_CONST, exp(right->GetDirect()));
			break;
		case OP_LN:
			if(1 == rtype)	this->Set(OP_CONST, log(right->GetDirect()));
			break;
		case OP_SQRT:
			if(1 == rtype)	this->Set(OP_CONST, sqrt(right->GetDirect()));
			break;
		case OP_SIN:
			if(1 == rtype)	this->Set(OP_CONST, sin(right->GetDirect()));
			break;
		case OP_COS:
			if(1 == rtype)	this->Set(OP_CONST, cos(right->GetDirect()));
			break;
		case OP_TAN:
			if(1 == rtype)	this->Set(OP_CONST, tan(right->GetDirect()));
			break;
		case OP_SINH:
			if(1 == rtype)	this->Set(OP_CONST, sinh(right->GetDirect()));
			break;
		case OP_COSH:
			if(1 == rtype)	this->Set(OP_CONST, cosh(right->GetDirect()));
			break;
		case OP_TANH:
			if(1 == rtype)	this->Set(OP_CONST, tanh(right->GetDirect()));
			break;
		case OP_ASIN:
			if(1 == rtype)	this->Set(OP_CONST, asin(right->GetDirect()));
			break;
		case OP_ACOS:
			if(1 == rtype)	this->Set(OP_CONST, acos(right->GetDirect()));
			break;
		case OP_ATAN:
			if(1 == rtype)	this->Set(OP_CONST, atan(right->GetDirect()));
			break;
		case OP_ASINH:
			if(1 == rtype)	this->Set(OP_CONST, asinh(right->GetDirect()));
			break;
		case OP_ACOSH:
			if(1 == rtype)	this->Set(OP_CONST, acosh(right->GetDirect()));
			break;
		case OP_ATANH:
			if(1 == rtype)	this->Set(OP_CONST, atanh(right->GetDirect()));
			break;
		case OP_HEAVISIDE:
			if(1 == rtype)	this->Set(OP_CONST, ((right->GetDirect() > 0) ? 1 : 0));
			break;
	}
}

/** \fn void Node::LinearSolve(Opcode const)
 * \brief Tries to put a variable outside the brackets.
 *
 *	This method tries to put a variable given via Opcode out of brackets, e.g.
 *	to make a transformation like \f$ a*x + b*x = x*(a + b) \f$.<br>
 *	This is done by first using the Method
 *	Node::LinearSolve(Opcode const, Node &), to get the factor \f$ a+ b \f$
 *	which is then reduced, like this tree (which includes now only constants and
 *	other variables). As last step this node is set to a plus operation between
 *	the factor and this tree (the constants/variables left).
 *
 * \param var: An Opcode, which gives the variable that should be put outside
 *				the brackets.
 */
void Node::LinearSolve(Opcode const var) {
	if((OP_X != var) and (OP_Y != var) and (OP_Z != var) and (OP_T != var)) {
		std::stringstream err;
		err <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"\n";
		err <<"\tExpected variable, given was: "<<var<<".\n";
		throw NodeException(err.str(), NodeException::CANNOT_HANDLE_THIS_OPCODE);
		return;
	}

	if(NULL != left)	left->LinearSolve(var);
	if(NULL != right)	right->LinearSolve(var);

	Node factor;
	Node temp(*this);
	temp.LinearSolve(var, factor);

	temp.ReduceTree();
	factor.ReduceTree();

	// \note Here is no switch used, because this could give warnings, due to
	// the fact that not all members of the enum are considered.
	if(OP_X == var) {
		temp.LinearSolve(OP_Y);
		factor.LinearSolve(OP_Y);
	} else if(OP_Y == var) {
		temp.LinearSolve(OP_Z);
		factor.LinearSolve(OP_Z);
	} else if(OP_Z == var) {
		temp.LinearSolve(OP_T);
		factor.LinearSolve(OP_T);
	}

	this->Set(OP_PLUS, temp, Node(OP_TIMES, Node(var), factor));
	this->ReduceTree();
}

/** \fn void Node::LinearSolve(Opcode const, Node &)
 * \brief Tries to put a variable outside the brackets.
 *
 *	ax+bx -> x(a+b)
 *
 * \param var: An Opcode, which gives the variable that should be put outside
 *				the brackets.
 * \param factor: Reference to a node, which will store the factor of the
 *				variable.
 */
void Node::LinearSolve(Opcode const var, Node & factor) {
	if((OP_X != var) and (OP_Y != var) and (OP_Z != var) and (OP_T != var)) {
		std::stringstream err;
		err <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"\n";
		err <<"\tExpected variable, given was: "<<var<<".\n";
		throw NodeException(err.str(), NodeException::CANNOT_HANDLE_THIS_OPCODE);
		return;
	}

	if(var == this->GetOpcode()) {
		this->Set(OP_CONST, 0);
		factor.Set(OP_CONST, 1);
		return;
	}

	bool l,r;
	if(NULL != left)	l = left->Contains(var);
	else				l = false;
	if(NULL != right)	r = right->Contains(var);
	else				r = false;

	if((false == l) and (false == r)) {
		factor.Set(OP_CONST, 0.0);
		return;
	}
	switch(this->GetOpcode()) {
		case OP_PLUS:
			if((true == l) and (true == r)) {
				Node ltemp, rtemp;
				left->LinearSolve(var, ltemp);
				right->LinearSolve(var, rtemp);
				factor.Set(OP_PLUS, ltemp, rtemp);
			} else if(true == r) {
				right->LinearSolve(var, factor);
			} else if(true == l) {
				left->LinearSolve(var, factor);
			}
			break;
		case OP_MINUS:
			if((true == l) and (true == r)) {
				Node ltemp, rtemp;
				left->LinearSolve(var, ltemp);
				right->LinearSolve(var, rtemp);
				factor.Set(OP_MINUS, ltemp, rtemp);
			} else if(true == r) {
				Node rtemp;
				right->LinearSolve(var, rtemp);
				factor.Set(OP_TIMES, Node(OP_CONST, -1), rtemp);
			} else if(l) {
				left->LinearSolve(var, factor);
			}
			break;
		case OP_TIMES:
			if((true == l) and (true == r)) {
				factor.Set(OP_CONST, 0.0);
			} else if(true == r) {
				Node rtemp;
				right->LinearSolve(var, rtemp);
				factor.Set(OP_TIMES, *left, rtemp);
			} else if(l) {
				Node rtemp;
				left->LinearSolve(var, rtemp);
				factor.Set(OP_TIMES, *right, rtemp);
			}
			break;
		case OP_DIVIDES:
			if((true == l) and (true == r)) {
				factor.Set(OP_CONST, 0.0);
			} else if(true == r) {
				factor.Set(OP_CONST, 0.0);
			} else if(l) {
				Node ltemp;
				left->LinearSolve(var, ltemp);
				factor.Set(OP_DIVIDES, ltemp, *right);
			}
			break;
		default:
			factor.Set(OP_CONST, 0.0);
			break;
	}
}

/** name: Node::Depth
 * \brief Get depth of the tree.
 *
 *	Assuming that this object is the root of a tree, this method returns the
 *	depth as the maximum of the depth of the sub-trees plus one.<br>
 *	If there are no subtrees, the depth is one.
 *
 * \return The maximum of the depth of the left and right nodes increase by one.
 */
size_t Node::Depth() const {
	size_t left_depth	= 0;
	size_t right_depth	= 0;

	// Get the depth of the childs
	if(left)  left_depth  = left->Depth();
	if(right) right_depth = right->Depth();

	// Depth of this node is the maximum of the depths of childs increased by one.
	if(right_depth > left_depth)
		return right_depth + 1;
	else
		return left_depth + 1;
}

/** name: Node::SerializedLength
 * \brief Returns the total length of the tree/function.
 *
 *	Assuming that this Node is the root of a tree, which represents a function,
 *	the number of operations is returned, as the sum of the SerializedLength of
 *	the childs increased by two if the Node represents a constant (because it
 *	is assumed that the Opcode and the value are two different operations, see
 *	Node::Postorder), otherwise increased by one.
 *
 * \return The total length of the tree/function.
 */
size_t Node::SerializedLength() const {
	size_t length = 0;

	if(left)  length += left->SerializedLength();
	if(right) length += right->SerializedLength();

	length++;
	if(opcode == OP_CONST) length++;

	return length;
}

/** \fn bool Node::TreeEqualTo(Node const & ) const
 * \brief Test if two trees are equal.
 *
 *	This method will check if a tree is equal to another. Equality thereby means
 *	the subtrees are the same, the Opcodes are the same and if it is OP_CONST
 *	are the values the same.
 *
 * \param other: Reference to the root of the other tree, which should be
 *				compared with this one.
 * \return True if the subtrees are equal, the operationcodes are the same, and
 *				for the case of operationcode OP_CONST if the values are the
 *				same. False otherwise.
 */
bool Node::TreeEqualTo(Node const & other) const {
	if(NULL != other.GetLeft()) {
		if(NULL != this->GetLeft()) {
			if(not this->GetLeft()->TreeEqualTo(*other.GetLeft())) return false;
		} else return false;// Other has left-child and this not.
	} else {
		if(NULL != this->GetLeft()) return false;// this has left child and other not.
	}
	if(NULL != other.GetRight()) {
		if(NULL != this->GetRight()) {
			if(not this->GetRight()->TreeEqualTo(*other.GetRight())) return false;
		} else return false;// Other has right-child and this not.
	} else {
		if(NULL != this->GetRight()) return false;// this has right child and other not.
	}

	if(this->GetOpcode() != other.GetOpcode()) return false;

	if((this->GetOpcode() == OP_CONST) and (this->GetDirect() != other.GetDirect())) return false;// Value does only matter if this is a constant.

	return true;
}

/** \fn bool Node::NodeEqualTo(Node const &) const
 * \brief A flat check for equality.
 *
 *	This method will check if the operationcodes of this and an other Node are
 *	the same and additionaly if the values are the same, in the case the
 *	operationcode is OP_CONST.
 *
 * \param other: Reference to the Node which should be compared with this one.
 * \return False if either the operationcodes are different or they are both
 *				OP_CONST but the values are different. True otherwise.
 */
bool Node::NodeEqualTo(Node const & other) const {
	if(this->GetOpcode() != other.GetOpcode()) return false;

	if((this->GetOpcode() == OP_CONST) and (this->GetDirect() != other.GetDirect())) return false;// Value does only matter if this is a constant.

	return true;
}

/** \fn bool Node::Contains(Opcode const) const
 * \brief Test whether an Opcode is found in the tree.
 * \param o: The Opcode for which it should be checked if it is contained within
 *				this tree.
 * \return True if either of the childs Contains the Opcode or if the Opcode of
 *				this Node is equal to the given Opcode, false otherwise.
 */
bool Node::Contains(Opcode const o) const {
	if(NULL != left) {
		if(left->Contains(o)) return true;
	}
	if(NULL != right) {
		if(right->Contains(o)) return true;
	}

	if(o == this->GetOpcode()) return true;

	return false;
}

/** \fn bool Node::Contains(Node const &) const
 * \brief Test whether a Node is found in the tree.
 * \param other: Reference to the Node, for which to check if there is a copy
 *				within this tree.
 * \return True if either of the childs Contains the Node or if this Node is
 *				equal to the given one, false otherwise.
 */
bool Node::Contains(Node const & other) const {
	if(NULL != left) {
		if(left->Contains(other)) return true;
	}
	if(NULL != right) {
		if(right->Contains(other)) return true;
	}

	if(this->NodeEqualTo(other)) return true;

	return false;
}

/** \fn Cell * Node::Postorder(Cell *) const
 * \brief Creates the code for function the tree represents in postorder.
 *
 *	For the represented function, this method creates the code in postorder
 *	notation, which means the function follows the arguments (e.g. a+b
 *	is written as ab+). The created code is stored in an array given as pointer.
 *
 * \note If a operation stands for a constant, is the next item of the array
 *			used to store its value.
 *
 * \param bytecode: Pointer to array of Cell's where the code is stored.
 * \return Pointer to array of Cell's ('index' is increased at least by one).
 */
Cell* Node::Postorder(Cell *bytecode) const {
	// Create the code of the childs.
	if(left)  bytecode = left->Postorder(bytecode);
	if(right) bytecode = right->Postorder(bytecode);

	// Set the operationcode and increase the pointer.
	bytecode->opcode = opcode;
	++bytecode;

	// If this operation stands for a constant, its value is stored in the next position.
	if(opcode == OP_CONST) {bytecode->number = direct; ++bytecode;}

	bytecode->opcode = OP_END;

	return bytecode;
}

#ifdef DEBUG
/** name: Node::Info
 *	\brief Print the status of this Node.
 *
 *	This method first called the Print-method for the childs and then prints
 *	the values of the opcode- and direct-member.<br>
 *	To draw the tree that corresponds to the output, go line by line through
 *	the output and perform the following steps, depending on what is
 *	printed:
 *	- 'Printing Node' draw a rectangle at the lower end of the last line
 *		that was drawn (should have enough space for a integer and a
 *		float). If this is the first line simply draw the rectangle at
 *		the top.
 *		This is now the actual node.
 *	- 'Left'/'Right' draw a line from the actual node diagonal down in the
 *		corresponding direction.
 *	- 'this->opcode'/'this->direct' write the number in the actual node.
 *		(The first gives the operation/type of the node, the second the
 *		value if the opcode is OP_CONST).
 *	- 'left end'/'right end' should be ignored (this indicates that a
 *		left/right subtree is now complete).
 *	- 'Node end' the parent of the actual node becomes the new actual node.
 *
 *
 * \attention This method will only be defined if the define constant DEBUG
 *		exists.
 */
void Node::Info() const {
	// To get the whole tree, print the childs.
	std::cout<<"----- Printing Node -----\n";
	if(left) {std::cout<<"Left   "<<"\n"; left->Info();  std::cout<<"--- left end\n";}
	if(right) {std::cout<<"Right "<<"\n"; right->Info(); std::cout<<"--- right end\n";}

	// When the childs are printed, print the members.
	std::cout<<"this->opcode "<<this->opcode<<"\n";
	std::cout<<"this->direct "<<this->direct<<"\n";
	std::cout<<"----- Node end -----\n";
}
#endif // DEBUG
}

#endif // NODE_CPP
