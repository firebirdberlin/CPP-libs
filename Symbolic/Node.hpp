//		./Symbolic/Node.hpp
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

#ifndef NODE_HPP
#define NODE_HPP

/** \file Symbolic/Node.hpp
 * \brief Declares Node-class.
 *
 * This file declares the Node-class.
 */

#include "Definitions.h"

namespace Symbolic {

/** \class Node
 * \brief Represents a node in a treeform of a function.
 *
 *	\image html Doc_Images/BinaryTree.png
 *
 *	If you think of a function as a binary tree, these class represents a node
 *	of those three (the picture was made with mathematica and TreeForm and is
 *	only as example, ignoring that this isn't a binary tree).
 *
 *	The binary tree structure is achieved by pointers to the left and the right
 *	child.
 *	The operation for which the node stand is given as Opcode and if it
 *	represents a constant, these is stored in the direct member.
 *
 *	The nodes at the bottom (no childs) can only be constants and constants
 *	couldn't be within the tree.
 *	Unary operators or functions have one child, binary operators or functions
 *	have two.
 */
class Node {
	public:
		Node();
		Node(Opcode const opcode);
		Node(Opcode const opcode, Node const &left, Node const &right);
		Node(Opcode const opcode, Node const &branch);
		Node(Opcode const opcode, double const direct);

		Node(Node const & other);

		~Node();

		void Copy(Node const & other);

		Node const & operator=(Node const &other);

		void Set(Opcode const code, Node const &l, Node const &r);
		void Set(Opcode const code, Node const &branch);
		void Set(Opcode const code, double const dir);
		void Set(Opcode const code);

		void ReduceTree();
		void LinearSolve(Opcode const var);
		void LinearSolve(Opcode const var, Node & factor);

		Opcode GetOpcode() const {return opcode;};
		double GetDirect() const {return direct;};
		Node const * GetLeft() const {return left;};
		Node const * GetRight() const {return right;};

		size_t	Depth() const;
		size_t	SerializedLength() const;
		bool	TreeEqualTo(Node const & other) const;
		bool	NodeEqualTo(Node const & other) const;
		bool	Contains(Opcode const o) const;
		bool	Contains(Node const & other) const;
		Cell*	Postorder(Cell *bytecode) const;


#ifdef DEBUG
		void Info() const;
#endif

	private:
		Opcode opcode;//!< The operation this node represents.
		double direct;//!< Value, if this is represents a constant.
		Node *left;//!< Pointer to the left child.
		Node *right;//!< Pointer to the right child.
};

}

#endif // NODE_HPP
