/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef _CLogNode_Walker_Std_H_
#define _CLogNode_Walker_Std_H_

#include <list>

////////////////////////////////////////////////////////////////////////////////
//class CLogNode_Walker_Std

template<class TNode>
class CLogNode_Walker_Std
{
 private:
  typedef CLogNode_Walker_Std<TNode>        self_type;

  CLogNode_Walker_Std(const self_type&);
  self_type& operator = (const self_type&);

 public:
  typedef TNode                             node_type;

 public:
  CLogNode_Walker_Std(node_type* pNode);

 ~CLogNode_Walker_Std();

  //interface ------------------------------------------------------------
  node_type* CurNode()const;

  bool Next();

 private:
  class tag_stack_item
  {
   public:
    node_type* m_pNode;
    size_t     m_iChild;

   public:
    tag_stack_item(node_type* const pNode)
     :m_pNode  (pNode),
      m_iChild (0)
    {
     assert(m_pNode!=NULL);
    }//tag_stack_item
  };//class tagStackItem

  typedef tag_stack_item                    stack_item_type;
  typedef std::list<stack_item_type>        stack_type;

 private:
  stack_type m_Stack;
  node_type* m_pCurNode;
};//class CLogNode_Walker_Std

////////////////////////////////////////////////////////////////////////////////
#include "CLogNode_Walker_Std.cc"
////////////////////////////////////////////////////////////////////////////////
#endif
