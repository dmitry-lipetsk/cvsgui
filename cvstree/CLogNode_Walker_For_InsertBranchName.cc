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
#ifndef _CLogNode_Walker_For_InsertBranchName_CC_
#define _CLogNode_Walker_For_InsertBranchName_CC_

#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
//Source code of original InsertBranchName

#if 0
static bool InsertBranchName(CLogNode *node, const CRevNumber& symb)
{
    vector<CLogNode *>::iterator i;
    for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
    {
        /*POINT1*/
        CLogNode *subnode = *i;
        if(subnode->GetType() == kNodeRev)
        {
            CLogNodeRev & rev = *(CLogNodeRev *)subnode;
            if((*rev).RevNum().ispartof(symb))
            {
                // insert the branch name as previous node of the
                // first node of that branch
                CLogNodeBranch *branch = new CLogNodeBranch(symb.Tag(), node);
                branch->Next() = subnode;
                *i = branch;
                return true;
            }
        }
        /*POINT4*/
        if(InsertBranchName(subnode, symb))
            return true;
    }

    /*POINT2*/
    if(node->Next() != 0L && /*POINT5*/ InsertBranchName(node->Next(), symb))
        return true;

    /*POINT3*/
    // we didn't find to connect this branch because there is no
    // revision in this branch (empty branch) : so add it as a child of this node.
    if(node->GetType() == kNodeRev)
    {
        CLogNodeRev & rev = *(CLogNodeRev *)node;
        if(symb.issubbranchof((*rev).RevNum()))
        {
            CLogNodeBranch *branch = new CLogNodeBranch(symb.Tag(), node);
            node->Childs().push_back(branch);
        }
    }

    return false;
}//InsertBranchName
#endif

////////////////////////////////////////////////////////////////////////////////
//class CLogNode_Walker_For_InsertBranchName

template<class TNode>
CLogNode_Walker_For_InsertBranchName<TNode>::CLogNode_Walker_For_InsertBranchName(node_type* const pNode)
 :m_pCurNode       (pNode),
  m_CurNodeIsChild (false)
{
 //Go to "POINT1" or "POINT3"
 if(m_pCurNode!=NULL)
 {
  this->NextChild();

  assert(m_pCurNode!=NULL);
 }//if
}//CLogNode_Walker_For_InsertBranchName

//------------------------------------------------------------------------
template<class TNode>
CLogNode_Walker_For_InsertBranchName<TNode>::~CLogNode_Walker_For_InsertBranchName()
{
}

//------------------------------------------------------------------------
template<class TNode>
typename CLogNode_Walker_For_InsertBranchName<TNode>::node_type*
 CLogNode_Walker_For_InsertBranchName<TNode>::CurNode()const
{
 assert(m_pCurNode!=NULL);

 return m_pCurNode;
}//CurNode

//------------------------------------------------------------------------
template<class TNode>
bool CLogNode_Walker_For_InsertBranchName<TNode>::CurNodeIsChild()const
{
 assert(m_pCurNode!=NULL);

 return m_CurNodeIsChild;
}//CurNodeIsChild

//------------------------------------------------------------------------
template<class TNode>
typename CLogNode_Walker_For_InsertBranchName<TNode>::node_type*
 CLogNode_Walker_For_InsertBranchName<TNode>::CurNodeParent()const
{
 assert(m_pCurNode!=NULL);
 assert(m_CurNodeIsChild);
 assert(!m_Stack.empty());
 assert(m_Stack.back().m_pNode!=NULL);
 assert(m_Stack.back().m_iChild<m_Stack.back().m_pNode->Childs().size());
 assert(m_Stack.back().m_pNode->Childs()[m_Stack.back().m_iChild]==m_pCurNode);

 return m_Stack.back().m_pNode;
}//CurNodeParent

//------------------------------------------------------------------------
template<class TNode>
size_t CLogNode_Walker_For_InsertBranchName<TNode>::CurNodeIndex()const
{
 assert(m_pCurNode!=NULL);
 assert(m_CurNodeIsChild);
 assert(!m_Stack.empty());
 assert(m_Stack.back().m_pNode!=NULL);
 assert(m_Stack.back().m_iChild<m_Stack.back().m_pNode->Childs().size());
 assert(m_Stack.back().m_pNode->Childs()[m_Stack.back().m_iChild]==m_pCurNode);

 return m_Stack.back().m_iChild;
}//CurNodeIndex

//------------------------------------------------------------------------
template<class TNode>
bool CLogNode_Walker_For_InsertBranchName<TNode>::Next()
{
 if(m_pCurNode==NULL)
  return false;

 //----------------------------------------- First step
 if(m_CurNodeIsChild)
 {
  this->NextChild();

  return true;
 }//if m_CurNodeIsChild

 //----------------------------------------- Rollback
 assert(!m_CurNodeIsChild);

 if(!m_Stack.empty())
 {
  assert(!m_Stack.empty());
  assert(m_pCurNode!=NULL);

  stack_item_type& BackItem=m_Stack.back();

  assert(BackItem.m_pNode!=NULL);
  assert(BackItem.m_iChild<=BackItem.m_pNode->Childs().size());

  assert(m_pCurNode->Root()==BackItem.m_pNode);

  if(BackItem.m_iChild==BackItem.m_pNode->Childs().size())
  {
   //return from Next [POINT5 returns false]

   assert(BackItem.m_pNode->Next()==m_pCurNode);

   m_pCurNode=BackItem.m_pNode;

   m_Stack.pop_back();

   /*THIS IS POINT3*/
   return true;
  }//if return from Next

  /*return from child. POINT4 returns false.*/

  assert(BackItem.m_iChild<BackItem.m_pNode->Childs().size());

  assert(BackItem.m_pNode->Childs()[BackItem.m_iChild]==m_pCurNode);

  ++m_Stack.back().m_iChild;

  if(BackItem.m_iChild<BackItem.m_pNode->Childs().size())
  {
   /*POINT1*/
   m_pCurNode=BackItem.m_pNode->Childs()[BackItem.m_iChild];

   assert(m_pCurNode!=NULL);
   assert(m_pCurNode->Root()==BackItem.m_pNode);

   m_CurNodeIsChild=true;

   return true;
  }//if

  assert(BackItem.m_iChild==BackItem.m_pNode->Childs().size());
  assert(!m_CurNodeIsChild);

  /*POINT2*/
  if(BackItem.m_pNode->Next()!=NULL)
  {
   /*POINT5 recursive call*/

   m_pCurNode=BackItem.m_pNode->Next();

   //m_CurNodeIsChild=false;

   this->NextChild();

   assert(m_pCurNode!=NULL);

   return true;
  }//if

  /*POINT3*/
  assert(BackItem.m_pNode->Next()==NULL);

  m_pCurNode=BackItem.m_pNode;

  m_Stack.pop_back();

  return true;
 }//if !m_Stack.empty()

 assert(m_Stack.empty());
 assert(!m_CurNodeIsChild);

 m_pCurNode=NULL;

 m_CurNodeIsChild=false;

 return false;
}//Next

//------------------------------------------------------------------------
template<class TNode>
void CLogNode_Walker_For_InsertBranchName<TNode>::NextChild()
{
 assert(m_pCurNode!=NULL);

 for(;;)
 {
  if(!m_pCurNode->Childs().empty())
  {
   /*this is POINT1*/
   m_Stack.push_back(m_pCurNode);

   m_pCurNode=m_pCurNode->Childs()[0];

   m_CurNodeIsChild=true;

   assert(m_pCurNode!=NULL);
   assert(m_pCurNode->Root()==m_Stack.back().m_pNode);

   return;
  }//if

  assert(m_pCurNode->Childs().empty());

  /*this is POINT2*/
  if(m_pCurNode->Next()!=NULL)
  {
   /*this is POINT5 [recursive call]*/
   m_Stack.push_back(m_pCurNode);

   m_pCurNode=m_pCurNode->Next();

   assert(m_pCurNode!=NULL);
   assert(m_pCurNode->Root()==m_Stack.back().m_pNode);

   continue;
  }//if

  assert(m_pCurNode->Next()==NULL);

  /*this is POINT3*/
  m_CurNodeIsChild=false;

  return;
 }//for[ever]
}//NextChild

////////////////////////////////////////////////////////////////////////////////
#endif
