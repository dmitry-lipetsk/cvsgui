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
#ifndef _CLogNode_Walker_Std_CC_
#define _CLogNode_Walker_Std_CC_

#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
//Source code of original InsertSymbName

#if 0
static bool InsertSymbName(CLogNode *node, const CRevNumber& symb)
{
    if(node->GetType() == kNodeRev)
    {
        CLogNodeRev & rev = *(CLogNodeRev *)node;
        if((*rev).RevNum() == symb)
        {
            // insert the tag as achild of the node
            CLogNodeTag *tag = new CLogNodeTag(symb.Tag(), node);
            node->Childs().push_back(tag);
            return true;
        }
    }

    vector<CLogNode *>::iterator i;
    for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
    {
        if(InsertSymbName(*i, symb))
            return true;
    }
    if(node->Next() != 0L && InsertSymbName(node->Next(), symb))
        return true;

    return false;
}//InsertSymbName
#endif //0

////////////////////////////////////////////////////////////////////////////////
//class CLogNode_Walker_Std

template<class TNode>
CLogNode_Walker_Std<TNode>::CLogNode_Walker_Std(node_type* const pNode)
 :m_pCurNode(pNode)
{
}//CLogNode_Walker_Std

//------------------------------------------------------------------------
template<class TNode>
CLogNode_Walker_Std<TNode>::~CLogNode_Walker_Std()
{
}

//------------------------------------------------------------------------
template<class TNode>
typename CLogNode_Walker_Std<TNode>::node_type*
 CLogNode_Walker_Std<TNode>::CurNode()const
{
 assert(m_pCurNode!=NULL);

 return m_pCurNode;
}//CurNode

//------------------------------------------------------------------------
template<class TNode>
bool CLogNode_Walker_Std<TNode>::Next()
{
 if(m_pCurNode==NULL)
  return false;

#ifndef NDEBUG
 node_type* const pStartNode=m_pCurNode;
#endif

 //----------------------------------------- First step
 if(!m_pCurNode->Childs().empty())
 {
  //check tree structure
  assert(m_pCurNode->Childs()[0]->Root()==m_pCurNode);

  m_Stack.push_back(m_pCurNode);

  m_pCurNode=m_pCurNode->Childs()[0];

  assert(m_pCurNode!=NULL);
  assert(m_pCurNode!=pStartNode);

  return true;
 }//if

 assert(m_pCurNode->Childs().empty());

 if(m_pCurNode->Next()!=NULL)
 {
  //check tree structure
  assert(m_pCurNode->Next()->Root()==m_pCurNode);

  m_Stack.push_back(m_pCurNode);

  m_pCurNode=m_pCurNode->Next();

  assert(m_pCurNode!=NULL);
  assert(m_pCurNode!=pStartNode);

  return true;
 }//if

 //----------------------------------------- Rollback
 while(!m_Stack.empty())
 {
  assert(!m_Stack.empty());
  assert(m_pCurNode!=NULL);

  stack_item_type& BackItem=m_Stack.back();

  assert(BackItem.m_pNode!=NULL);
  assert(BackItem.m_iChild<=BackItem.m_pNode->Childs().size());

  //check tree structure
  assert(BackItem.m_pNode==m_pCurNode->Root());

  if(BackItem.m_iChild==BackItem.m_pNode->Childs().size())
  {
   //return from Next

   assert(BackItem.m_pNode->Next()==m_pCurNode);
  }
  else
  {
   assert(BackItem.m_iChild<BackItem.m_pNode->Childs().size());

   assert(BackItem.m_pNode->Childs()[BackItem.m_iChild]==m_pCurNode);
   assert(BackItem.m_pNode==m_pCurNode->Root());

   ++m_Stack.back().m_iChild;

   if(BackItem.m_iChild<BackItem.m_pNode->Childs().size())
   {
    m_pCurNode=BackItem.m_pNode->Childs()[BackItem.m_iChild];

    assert(m_pCurNode!=NULL);
    assert(m_pCurNode!=pStartNode);

    //check tree structure
    assert(m_pCurNode->Root()==BackItem.m_pNode);

    return true;
   }//if

   assert(BackItem.m_iChild==BackItem.m_pNode->Childs().size());

   if(BackItem.m_pNode->Next()!=NULL)
   {
    m_pCurNode=BackItem.m_pNode->Next();

    assert(m_pCurNode!=NULL);
    assert(m_pCurNode!=pStartNode);

    //check tree structure
    assert(m_pCurNode->Root()==BackItem.m_pNode);

    return true;
   }//if
  }//else

  m_pCurNode=BackItem.m_pNode;

  m_Stack.pop_back();
 }//while

 assert(m_Stack.empty());

 m_pCurNode=NULL;

 return false;
}//Next

////////////////////////////////////////////////////////////////////////////////
#endif
