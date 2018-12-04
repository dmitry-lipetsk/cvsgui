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

#pragma warning(disable : 4786)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <new>
#include <sstream>
#include <algorithm>

#include "CvsLog.h"
#include "CLogNode_Walker_Std.h"
#include "CLogNode_Walker_For_InsertBranchName.h"
#include "common.h"

#if _MSC_VER >= 1400
#	pragma warning(disable : 4996)
#endif

using namespace std;

vector<CRcsFile> gRcsFiles;

CRcsFile* gRcsFile = NULL;
CRevFile* gRevFile = NULL;

#ifdef TARGET_OS_MAC
vector<void*> gMacAlloca;
#endif

////////////////////////////////////////////////////////////////////////////////

static bool InsertSymbName(CLogNode* const node, const CRevNumber& symb)
{
	assert(node!=NULL);

#if 1
	CLogNode_Walker_Std<CLogNode> walker(node);

	for(;;)
	{
		if(walker.CurNode()->GetType() == kNodeRev)
		{
			CLogNodeRev & rev = *(CLogNodeRev *)walker.CurNode();

			if((*rev).RevNum() == symb)
			{
				// insert the tag as achild of the node
				CLogNodeTag *tag = new CLogNodeTag(symb.Tag(), walker.CurNode());

				walker.CurNode()->Childs().push_back(tag);

				return true;
			}
		}//if

		if(!walker.Next())
		{
			return false;
		}
	}//for[ever]
#else
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
#endif
}//InsertSymbName

//------------------------------------------------------------------------
static void InsertBranchName(CLogNode* const node, const CRevNumber& symb)
{
	assert(node!=NULL);

#if 1
	CLogNode_Walker_For_InsertBranchName<CLogNode> walker(node);

#ifndef NDEBUG
	bool inserted_as_empty=false;
#endif

	for(;;)
	{
		if(walker.CurNodeIsChild())
		{
			CLogNode* const subnode = walker.CurNode();

			assert(subnode != NULL);

			if(subnode->GetType() == kNodeRev)
			{
				const CLogNodeRev& rev = *(CLogNodeRev*)subnode;

				if((*rev).RevNum().ispartof(symb))
				{
					// insert the branch name as previous node of the
					// first node of that branch

					assert(walker.CurNodeParent()->Childs()[walker.CurNodeIndex()]==subnode);

					//-----
					CLogNodeBranch* const branch = new CLogNodeBranch(symb.Tag(), walker.CurNodeParent());

					walker.CurNodeParent()->Childs()[walker.CurNodeIndex()] = branch;

					branch->Next() = subnode;

					subnode->Root() = branch;

					assert(!inserted_as_empty);

					return /*true*/;
				}//if
			}//if
		}
		else
		{
			assert(!walker.CurNodeIsChild());

			// we didn't find to connect this branch because there is no
			// revision in this branch (empty branch) : so add it as a child of this node.
			if(walker.CurNode()->GetType() == kNodeRev)
			{
				const CLogNodeRev& rev = *(CLogNodeRev*)walker.CurNode();

				if(symb.issubbranchof((*rev).RevNum()))
				{
					CLogNodeBranch* const branch = new CLogNodeBranch(symb.Tag(), walker.CurNode());

					//ACHTUNG - we change the state of current node!
					walker.CurNode()->Childs().push_back(branch);

				#ifndef NDEBUG
					assert(!inserted_as_empty);

					inserted_as_empty=true;
				#endif

					//[2012-03-31] sf_animal: I think, we can uncomment
					//  this return without any problems. see test check 'inserted_as_empty'
					//
					//[2012-11-08]
					//  1. sf_animal:
					//     exit from function after creation of (empty) branch node
					//	   Amen.
					//
					//  2. sf_animal:
					//     Seem, we should return 'true' instead 'false'
					//     Reason: we create branch-node
					return /*false*/;
				}//if
			}//if
		}//else

		if(!walker.Next())
		{
			return /*false*/;
		}
	}//for[ever]
#else /*[2012-11-08] sf_animal: #OLD_CODE*/
	vector<CLogNode*>::iterator i;

	for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
	{
		CLogNode* const subnode = *i;

		assert(subnode != NULL);
		assert(subnode->Root() == node);

		if(subnode->GetType() == kNodeRev)
		{
			CLogNodeRev& rev = *(CLogNodeRev*)subnode;

			if((*rev).RevNum().ispartof(symb))
			{
				// insert the branch name as previous node of the
				// first node of that branch
				CLogNodeBranch* const branch = new CLogNodeBranch(symb.Tag(), node);

				branch->Next() = subnode;

				subnode->Root() = branch;

				(*i) = branch;

				return true;
			}//if
		}//for

		if(InsertBranchName(subnode, symb))
			return true;
	}//for

	if(node->Next() != 0L && InsertBranchName(node->Next(), symb))
		return true;
	
	// we didn't find to connect this branch because there is no
	// revision in this branch (empty branch) : so add it as a child of this node.
	if(node->GetType() == kNodeRev)
	{
		const CLogNodeRev& rev = *(CLogNodeRev*)node;

		if(symb.issubbranchof((*rev).RevNum()))
		{
			CLogNodeBranch* const branch = new CLogNodeBranch(symb.Tag(), node);

			node->Childs().push_back(branch);
		}//if
	}//if
	
	return false;
#endif
}//InsertBranchName

#ifdef TARGET_OS_MAC
void *cvstree_alloca(unsigned size)
{
	void *res = malloc(size);
	if(res == 0L)
		return 0L;
	
	gMacAlloca.push_back(res);
	return res;
}

char *cvstree_strdup(const char *string)
{
	int size = strlen(string);
	char *result = (char *)malloc((size + 1) * sizeof(char));
	if(result == 0L)
		return 0L;
	strcpy(result, string);
	return result;
}
#endif /* TARGET_OS_MAC */

#if 0 //[2012-03-31] sf_animal: deprecated code

extern "C" {
	static int sortRevs(const void* r1, const void* r2);
}

#ifdef WIN32
extern "C"
#endif /* WIN32 */
static int sortRevs(const void* r1, const void* r2)
{
	CRevNumber& rev1 = ((CRevFile*)r1)->RevNum();
	CRevNumber& rev2 = ((CRevFile*)r2)->RevNum();
	
	return rev1.cmp(rev2);
}

#endif //deprecated code

////////////////////////////////////////////////////////////////////////////////
//class CRevFile::tag_less_by_rev_num

class CRevFile::tag_less_by_rev_num
{
public:
	bool operator () (const CRevFile& r1, const CRevFile& r2)const
	{
		const CRevNumber& rev1 = r1.RevNum();
		const CRevNumber& rev2 = r2.RevNum();

		return rev1.cmp(rev2)<0;
	}//operator ()
};//class CRevFile::tag_less_by_rev_num 

////////////////////////////////////////////////////////////////////////////////
/*!
	Parse a file and return the set of RCS files
	\param file File to parse
	\return RCS file collection
*/
std::vector<CRcsFile>& CvsLogParse(FILE* file)
{
#if qCvsDebug
	yydebug = 0;
#endif
	yy_flex_debug = 0;
	yyin = file;
	yyreset();
	yyrestart(yyin);
	yyparse();

#ifdef TARGET_OS_MAC
	vector<void *>::iterator i;
	for(i = gMacAlloca.begin(); i != gMacAlloca.end(); i++)
	{
		if(*i != 0L)
			free(*i);
		*i = 0L;
	}
	gMacAlloca.erase(gMacAlloca.begin(), gMacAlloca.end());
#endif /* TARGET_OS_MAC */

	return gRcsFiles;
}

/*!
	Free the memory used
*/
void CvsLogReset(void)
{
	yyreset();
}

/*!
	Build a tree (i.e. graph) of the history
	\param rcsfile RCS file
	\return Root log node
*/
CLogNode* CvsLogGraph(CRcsFile& rcsfile)
{
	// we sort the revisions in order to build the tree
	// using a stack-algorithm
	rcsfile.sort();

	CLogNodeHeader* const header = new CLogNodeHeader(rcsfile);
	CLogNodeRev* curNode = 0L;

	// append the revisions to the tree
	vector<CRevFile>::const_iterator i;
	for(i = rcsfile.AllRevs().begin(); i != rcsfile.AllRevs().end(); ++i)
	{
		const CRevFile& rev = *i;
		
		if(curNode == 0L)
		{
			CLogNodeRev* const nrev = new CLogNodeRev(rev, header);
			header->Childs().push_back(nrev);
			curNode = nrev;
			continue;
		}

		do
		{
			const CRevNumber& curRev = (**curNode).RevNum();
			const CRevNumber& thisRev = rev.RevNum();

			if(thisRev.ischildof(curRev))
			{
				CLogNodeRev* const nrev = new CLogNodeRev(rev, curNode);
				curNode->Childs().push_back(nrev);
				curNode = nrev;
				break;
			}
			else
			if(thisRev.issamebranch(curRev))
			{
				CLogNodeRev* const nrev = new CLogNodeRev(rev, curNode);

				assert(curNode->Next()==NULL);

				curNode->Next() = nrev;
				curNode = nrev;
				break;
			}//if

			if(curNode->Root() == header)
				curNode = 0L;
			else
				curNode = (CLogNodeRev *)curNode->Root();
		}
		while(curNode != 0L);

		if(curNode == 0L)
		{
			CLogNodeRev* const nrev = new CLogNodeRev(rev, header);
			header->Childs().push_back(nrev);
			curNode = nrev;
		}
	}//for

	// append the tags
	vector<CRevNumber>::const_iterator s;
	for(s = rcsfile.SymbolicList().begin(); s != rcsfile.SymbolicList().end(); ++s)
	{
		InsertSymbName(header, *s);
	}

	// append the branch names
	for(s = rcsfile.SymbolicList().begin(); s != rcsfile.SymbolicList().end(); ++s)
	{
		InsertBranchName(header, *s);
	}

	return header;
}//CvsLogGraph

//////////////////////////////////////////////////////////////////////////
// CRevNumber

CRevNumber::CRevNumber()
{
}

CRevNumber::~CRevNumber()
{
	reset();
}

void CRevNumber::reset(void)
{
	m_allDigits.erase(m_allDigits.begin(), m_allDigits.end());
	m_str.erase();
	m_tagName.erase();
}

CRevNumber& CRevNumber::operator+=(int adigit)
{
	m_allDigits.push_back(adigit);
	
	return *this;
}

CRevNumber& CRevNumber::operator=(const CRevNumber& arev)
{
	this->reset();

	assert(m_allDigits.empty());

	m_allDigits = arev.m_allDigits;

	m_tagName = arev.Tag();

	return *this;
}

int CRevNumber::operator[](int index) const
{
	if( index < 0 || index >= size() )
		return -1;
	
	return m_allDigits[index];
}

/*!
	Get the string representation of this revision
	\return String representation of revision
*/
const char* CRevNumber::c_str(void) const
{
	if( m_str.empty() && !m_allDigits.empty() )
	{
		ostringstream buf;

		for(vector<int>::const_iterator it = m_allDigits.begin(); it != m_allDigits.end(); it++)
		{
			if( m_allDigits.begin() != it )
			{
				buf << '.';
			}

			buf << *it;
		}

		buf << ends;
		m_str = buf.str().c_str();
	}

	return m_str.c_str();
}

/*!
	Compare with another revision
	\param arev Revision to compare with
	\return -1, 0, 1 as appropriate
*/
int CRevNumber::cmp(const CRevNumber& arev) const
{
	const vector<int>& rev1 = IntList();
	const vector<int>& rev2 = arev.IntList();

	vector<int>::const_iterator i = rev1.begin();
	vector<int>::const_iterator j = rev2.begin();
	
	for( ; i != rev1.end() && j != rev2.end(); ++i, ++j)
	{
		if( (*i) != (*j) )
		{
			return (*i) < (*j) ? -1 : 1;
		}
	}
	
	if( i == rev1.end() && j != rev2.end() )
		return -1;
	
	if( i != rev1.end() && j == rev2.end() )
		return 1;
	
	return 0;
}

bool CRevNumber::operator==(const CRevNumber& arev) const
{
	if( this->size() != arev.size() || this->empty() )
		return false;

	return this->IntList()==arev.IntList();
}

/*!
	Verify whether given revision is a child
	\param arev Revision
	\return true if it's a child, false otherwise
	\note 1.4.2.2 is child of 1.4
*/
bool CRevNumber::ischildof(const CRevNumber& arev) const
{
	if( (arev.size() + 2) != this->size() )
		return false;

	assert(this->size() >= 2);

	if( /*this->empty() ||*/ arev.empty() )
		return false;

	const items_type& this_nums = this->IntList();
	const items_type& arev_nums = arev.IntList();

	assert( (arev_nums.size() + 2) == this_nums.size() );

	return std::equal(arev_nums.begin(),
	                  arev_nums.end(),
	                  this_nums.begin());
}//ischildof

/*!
	Verify revision are on the same branch
	\param arev Revision
	\return true if it's the same branch, false otherwise
	\note 1.1.1.2 is same branch than 1.1.1.3
*/
bool CRevNumber::issamebranch(const CRevNumber& arev) const
{
	if( this->size() != arev.size() || this->size() == 0 )
		return false;
	
	const vector<int>& rev1 = IntList();
	const vector<int>& rev2 = arev.IntList();

	vector<int>::const_iterator i = rev1.begin();
	vector<int>::const_iterator j = rev2.begin();

	return memcmp(&*i, &*j, (size() - 1) * sizeof(int)) == 0;
}

/*!
	Verify the revision is a part of another
	\param arev Revision
	\return true if it's a part of another revision, false otherwise
	\note 1.1.1.2 is part of 1.1.1, 1.4.2.3 is part of 1.4.0.2
*/
bool CRevNumber::ispartof(const CRevNumber& arev) const
{
	const vector<int>& rev1 = IntList();
	const vector<int>& rev2 = arev.IntList();

	vector<int>::const_iterator i = rev1.begin();
	vector<int>::const_iterator j = rev2.begin();

	if( size() == 0 || arev.size() == 0 )
		return false;

	if( (arev.size() & 1) != 0 )
	{
		// special case for "1.1.1"
		if( (arev.size() + 1) != size() )
			return false;
		
		return memcmp(&*i, &*j, arev.size() * sizeof(int)) == 0;
	}

	// case for 1.1.1.1.2.4 is part of 1.1.1.1.0.2
	if( arev.size() != size() || size() < 2 )
		return false;

	if( memcmp(&*i, &*j, (size() - 2) * sizeof(int)) != 0 )
		return false;

	return arev.IntList()[size() - 2] == 0 &&
		IntList()[size() - 2] == arev.IntList()[size() - 1];
}

/*!
	Verify the revision is a sub-branch of another
	\param arev Revision
	\return true if it's sub-branch, false otherwise
	\note 1.1.1 is subbranch of 1.1, 1.4.0.2 is subbranch of 1.4
*/
bool CRevNumber::issubbranchof(const CRevNumber& arev) const
{
	const vector<int>& rev1 = IntList();
	const vector<int>& rev2 = arev.IntList();

	vector<int>::const_iterator i = rev1.begin();
	vector<int>::const_iterator j = rev2.begin();

	if( size() == 0 || arev.size() == 0 )
		return false;

	if( (size() & 1) != 0 )
	{
		// special case for "1.1.1"
		if( (arev.size() + 1) != size() )
			return false;

		return memcmp(&*i, &*j, arev.size() * sizeof(int)) == 0;
	}

	// case for 1.4.0.2 is subbranch of 1.4
	if( (arev.size() + 2) != size() || IntList()[arev.size()] != 0 )
		return false;

	return memcmp(&*i, &*j, arev.size() * sizeof(int)) == 0;
}

//////////////////////////////////////////////////////////////////////////
// CRcsFile

CRcsFile::CRcsFile()
{
	m_selRevisions = 0;
	m_totRevisions = 0;
	m_lockStrict = false;
}

CRcsFile::CRcsFile(const CRcsFile& afile)
{
	*this = afile;
}

CRcsFile::~CRcsFile()
{
}

CRcsFile& CRcsFile::operator=(const CRcsFile& afile)
{
	m_rcsFile = afile.m_rcsFile;
	m_workingFile = afile.m_workingFile;
	m_headRev = afile.m_headRev;
	m_branchRev = afile.m_branchRev;
	m_keywordSubst = afile.m_keywordSubst;
	m_accessList = afile.m_accessList;
	m_symbolicList = afile.m_symbolicList;
	m_locksList = afile.m_locksList;
	m_selRevisions = afile.m_selRevisions;
	m_totRevisions = afile.m_totRevisions;
	m_lockStrict = afile.m_lockStrict;
	m_allRevs = afile.m_allRevs;
	m_descLog = afile.m_descLog;
	
	return *this;
}

#ifdef qUnix
void CRcsFile::print(std::ostream& out) const
{
	out << "Rcs file : '" << RcsFile() << "'\n";
	out << "Working file : '" << WorkingFile() << "'\n";
	out << "Head revision : " << HeadRev().c_str() << '\n';
	out << "Branch revision : " << BranchRev().c_str() << '\n';

	out << "Locks :" << (LockStrict() ? " strict" : "") << '\n';
	vector<CRevNumber>::const_iterator s;
	for(s = LocksList().begin(); s != LocksList().end(); ++s)
	{
		const CRevNumber& lock = *s;
		out << '\t' << lock.c_str() << " : '" << lock.Tag() << "'\n";
	}

	out << "Access :\n";
	vector<string>::const_iterator a;
	for(a = AccessList().begin(); a != AccessList().end(); ++a)
	{
		out << "\t'" << *a << "'\n";
	}

	out << "Symbolic names :\n";
	vector<CRevNumber>::const_iterator n;
	for(n = SymbolicList().begin(); n != SymbolicList().end(); ++n)
	{
		const CRevNumber& symb = *n;
		out << '\t' << symb.c_str() << " : '" << symb.Tag() << "'\n";
	}

	out << "Keyword substitution : '" << KeywordSubst() << "'\n";
	out << "Total revisions : " << TotRevisions() << "\n";
	out << "Selected revisions : " << SelRevisions() << "\n";
	out << "Description :\n" << DescLog() << '\n';

	vector<CRevFile>::const_iterator i;
	for(i = AllRevs().begin(); i != AllRevs().end(); ++i)
	{
		i->print(out);
	}
}
#endif

void CRcsFile::sort()
{
#if 1
	//sf_animal: checked with STL from:
	// - VS2010
	// - VC6

	std::sort(AllRevs().begin(),AllRevs().end(), CRevFile::tag_less_by_rev_num());

#else

	//sf_animal: This is very dirty solution. Not work with STL from:
	// - VS2010

#if !defined(__MWERKS__) && (__GNUC__ < 3) && _MSC_VER < 0x514
	qsort(AllRevs().begin(), AllRevs().size(), sizeof(CRevFile), sortRevs);
#else /* __MWERKS__ || __GNUC__ > 2 */
	qsort(&*AllRevs().begin(), AllRevs().size(), sizeof(CRevFile), sortRevs);
#endif /* __MWERKS__ || __GNUCC__ > 2 */

#endif
}

//////////////////////////////////////////////////////////////////////////
// CRevFile

CRevFile::CRevFile()
{
	m_chgPos = 0;
	m_chgNeg = 0;

	memset(&m_revTime, 0, sizeof(m_revTime));
}

CRevFile::CRevFile(const CRevFile& afile)
{
	*this = afile;
}

CRevFile::~CRevFile()
{
}

CRevFile& CRevFile::operator=(const CRevFile& afile)
{
	m_revNum = afile.m_revNum;
	m_revTime = afile.m_revTime;
	m_locker = afile.m_locker;
	m_branchesList = afile.m_branchesList;
	m_author = afile.m_author;
	m_state = afile.m_state;
	m_chgPos = afile.m_chgPos;
	m_chgNeg = afile.m_chgNeg;
	m_descLog = afile.m_descLog;
	m_keywordSubst = afile.m_keywordSubst;
	m_commitID = afile.m_commitID;
	m_mergePoint = afile.m_mergePoint;
	m_filename = afile.m_filename;

	return *this;
}

#ifdef qUnix
void CRevFile::print(std::ostream& out) const
{
	out << "----------------------------\n";
	out << "Revision : " << RevNum().c_str() << '\n';
	if(!Locker().empty())
		out << "Locked by : '" << Locker() << "'\n";
	out << "Date : " <<
		(RevTime().tm_year + 1900) << '/' <<
		(RevTime().tm_mon + 1) << '/' <<
		RevTime().tm_mday << ' ' <<
		RevTime().tm_hour << ':' <<
		RevTime().tm_min << ':' <<
		RevTime().tm_sec << '\n';
	out << "Author : '" << Author() << "'\n";
	out << "State : '" << State() << "'\n";
	out << "Lines : +" << ChgPos() << ' ' << ChgNeg() << '\n';

	if(!BranchesList().empty())
	{
		out << "Branches :\n";
		vector<CRevNumber>::const_iterator s;
		for(s = BranchesList().begin(); s != BranchesList().end(); ++s)
		{
			const CRevNumber& branch = *s;
			out << '\t' << branch.c_str() << '\n';
		}
	}
	if(!KeywordSubst().empty())
		out << "Keyword : '" << KeywordSubst() << "'\n";
	if(!CommitID().empty())
		out << "CommitID : '" << CommitID() << "'\n";
	if(!MergePoint().empty())
		out << "MergePoint : '" << MergePoint().c_str() << "'\n";
	if(!Filename().empty())
		out << "Filename : '" << Filename() << "'\n";

	out << "Description :\n" << DescLog() << '\n';
}
#endif

//////////////////////////////////////////////////////////////////////////
// CLogNodeData

CLogNodeData::CLogNodeData(CLogNode* const node)
	: m_node(node)
{
}

CLogNodeData::~CLogNodeData()
{
}

//////////////////////////////////////////////////////////////////////////
// CLogNode

CLogNode::CLogNode(CLogNode* const root)
	: m_root(root), m_next(NULL), m_user(NULL)
{
}

CLogNode::~CLogNode()
{
#if 1
	delete m_user;

	//------
	self_type* pNode=this;

	for(;;)
	{
		assert(pNode!=NULL);

		while(!pNode->Childs().empty())
		{
			if(pNode->Childs().back()!=NULL)
				break;

			pNode->Childs().pop_back();
		}//while

		if(!pNode->Childs().empty())
		{
			assert(pNode->Childs().back()!=NULL);
			assert(pNode->Childs().back()!=pNode);

			pNode->Childs().back()->Root()=pNode;

			self_type* const pChildNode=pNode->Childs().back();

			pNode->Childs().pop_back();

			pNode=pChildNode;

			continue;
		}//if !pNode->Childs().empty()

		if(pNode->Next()!=NULL)
		{
			assert(pNode->Next()!=pNode);

			pNode->Next()->Root()=pNode;

			self_type* const pNextNode=pNode->Next();

			pNode->Next()=NULL;

			pNode=pNextNode;

			continue;
		}//if pNode->Next()!=NULL

		assert(pNode->Childs().empty());
		assert(pNode->Next()==NULL);

		if(pNode==this)
			break;

		assert(pNode->Root()!=NULL);
		assert(pNode->Root()!=pNode);

		self_type* const pDelNode=pNode;

		pNode=pNode->Root();

		delete pDelNode;
	}//for[ever]
#else
	vector<CLogNode*>::iterator i;
	for(i = m_childs.begin(); i != m_childs.end(); ++i)
	{
		delete *i;
		*i = NULL;
	}
	
	delete m_next;
	delete m_user;
#endif
}//~CLogNode

//////////////////////////////////////////////////////////////////////////
// CLogNodeHeader

CLogNodeHeader::CLogNodeHeader(const CRcsFile& header, CLogNode* root /*= NULL*/) 
	: CLogNode(root), m_header(header)
{
}

kLogNode CLogNodeHeader::GetType(void) const
{
	return kNodeHeader;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeRev

CLogNodeRev::CLogNodeRev(const CRevFile& rev, CLogNode* root /*= NULL*/)
	: CLogNode(root), m_rev(rev)
{
}

kLogNode CLogNodeRev::GetType(void) const
{
	return kNodeRev;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeTag

CLogNodeTag::CLogNodeTag(const std::string& tag, CLogNode* root /*= NULL*/)
	: CLogNode(root), m_tag(tag)
{
}

kLogNode CLogNodeTag::GetType(void) const
{
	return kNodeTag;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeBranch

CLogNodeBranch::CLogNodeBranch(const std::string& branch, CLogNode* root /*= NULL*/)
	: CLogNode(root), m_branch(branch)
{
}

kLogNode CLogNodeBranch::GetType(void) const
{
	return kNodeBranch;
}

////////////////////////////////////////////////////////////////////////////////
