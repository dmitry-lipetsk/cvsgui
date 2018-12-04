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

#ifndef CVSLOG_H
#define CVSLOG_H

#ifdef WIN32
#	ifdef _DEBUG
#		define qCvsDebug 1
#	else
#		define qCvsDebug 0
#	endif
#endif /* WIN32 */

#include <vector>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>

class CRcsFile;
class CLogNode;

/// Log node types
typedef enum
{
	kNodeHeader,
	kNodeBranch,
	kNodeRev,
	kNodeTag
} kLogNode;

std::vector<CRcsFile>& CvsLogParse(FILE* file);
void CvsLogReset(void);
CLogNode* CvsLogGraph(CRcsFile& rcsfile);

/// Class to store a revision number
class CRevNumber
{
public: //typedefs -------------------------------------------------------
	typedef std::vector<int>                items_type;
	typedef items_type::size_type           size_type;

public:
	CRevNumber();
	virtual ~CRevNumber();

	void reset(void);
	inline size_type size() const;
	inline bool empty() const;

	int cmp(const CRevNumber& arev) const;

	CRevNumber& operator+=(int adigit);
	CRevNumber& operator=(const CRevNumber& arev);
	bool operator==(const CRevNumber& arev) const;
	bool operator<(const CRevNumber& arev) const;
	
	int operator[](int index) const;
	const char* c_str(void) const;

	bool ischildof(const CRevNumber& arev) const;
	bool issamebranch(const CRevNumber& arev) const;
	bool ispartof(const CRevNumber& arev) const;
	bool issubbranchof(const CRevNumber& arev) const;

	inline const items_type& IntList(void) const;
	inline items_type& IntList(void);

	inline const std::string& Tag(void) const;
	inline std::string& Tag(void);

protected:
	items_type          m_allDigits; /*!< Revision number digits */
	mutable std::string m_str;       /*!< String representation */
	std::string         m_tagName;   /*!< Tag or an author */
};

/// Single revision for a file
class CRevFile
{
public:
	class tag_less_by_rev_num;

public:
	CRevFile();
	CRevFile(const CRevFile& afile);
	virtual ~CRevFile();

	CRevFile& operator=(const CRevFile& afile);
	bool operator<(const CRevFile& afile) const;
	bool operator==(const CRevFile& afile) const;

	void print(std::ostream& out) const;

	inline const CRevNumber& RevNum(void) const;
	inline CRevNumber& RevNum(void);

	inline const struct tm & RevTime(void) const;
	inline struct tm & RevTime(void);

	inline const std::string& Locker(void) const;
	inline std::string& Locker(void);

	inline const std::vector<CRevNumber>& BranchesList(void) const;
	inline std::vector<CRevNumber>& BranchesList(void);
	inline CRevNumber& LastBranche(void);

	inline const std::string& Author(void) const;
	inline std::string& Author(void);

	inline const std::string& State(void) const;
	inline std::string& State(void);

	inline int ChgPos(void) const;
	inline int& ChgPos(void);

	inline int ChgNeg(void) const;
	inline int& ChgNeg(void);

	inline const std::string& DescLog(void) const;
	inline std::string& DescLog(void);
	
	inline const std::string& KeywordSubst(void) const;
	inline std::string& KeywordSubst(void);

	inline const std::string& CommitID(void) const;
	inline std::string& CommitID(void);

	inline const std::string& Filename(void) const;
	inline std::string& Filename(void);

	inline const CRevNumber& MergePoint(void) const;
	inline CRevNumber& MergePoint(void);

protected:
	CRevNumber m_revNum;
	struct tm m_revTime;
	std::string m_locker;
	std::vector<CRevNumber> m_branchesList;
	std::string m_author;
	std::string m_state;
	int m_chgPos;
	int m_chgNeg;
	std::string m_descLog;
	std::string m_keywordSubst;
	std::string m_commitID;
	CRevNumber m_mergePoint;
	std::string m_filename;
};

/// RCS infos for a file
class CRcsFile
{
public:
	CRcsFile();
	CRcsFile(const CRcsFile& afile);
	virtual ~CRcsFile();

	CRcsFile& operator=(const CRcsFile& afile);
	bool operator<(const CRcsFile& afile) const;
	bool operator==(const CRcsFile& afile) const;

	void sort(void);

	void print(std::ostream& out) const;

	inline const std::string& RcsFile(void) const;
	inline std::string& RcsFile(void);

	inline const std::string& WorkingFile(void) const;
	inline std::string& WorkingFile(void);

	inline const CRevNumber& HeadRev(void) const;
	inline CRevNumber& HeadRev(void);

	inline const CRevNumber& BranchRev(void) const;
	inline CRevNumber& BranchRev(void);

	inline const std::string& KeywordSubst(void) const;
	inline std::string& KeywordSubst(void);

	inline int SelRevisions(void) const;
	inline int& SelRevisions(void);

	inline int TotRevisions(void) const;
	inline int& TotRevisions(void);

	inline const std::vector<std::string>& AccessList(void) const;
	inline std::vector<std::string>& AccessList(void);
	inline std::string& LastAccess(void);

	inline const std::vector<CRevNumber>& SymbolicList(void) const;
	inline std::vector<CRevNumber>& SymbolicList(void);
	inline CRevNumber& LastSymbName(void);

	inline const std::vector<CRevNumber>& LocksList(void) const;
	inline std::vector<CRevNumber>& LocksList(void);
	inline CRevNumber& LastLock(void);

	inline bool LockStrict(void) const;
	inline bool& LockStrict(void);

	inline const std::vector<CRevFile>& AllRevs(void) const;
	inline std::vector<CRevFile>& AllRevs(void);
	inline CRevFile& LastRev(void);

	inline const std::string& DescLog(void) const;
	inline std::string& DescLog(void);

protected:
	std::string m_rcsFile;
	std::string m_workingFile;
	CRevNumber m_headRev;
	CRevNumber m_branchRev;
	std::string m_keywordSubst;
	std::vector<std::string> m_accessList;
	std::vector<CRevNumber> m_symbolicList;
	std::vector<CRevNumber> m_locksList;
	int m_selRevisions;
	int m_totRevisions;
	bool m_lockStrict;
	std::vector<CRevFile> m_allRevs;
	std::string m_descLog;
};

//////////////////////////////////////////////////////////////////////////
// Classes used to build a tree off the CVS log output

/// Log node data
class CLogNodeData
{
private:
	typedef CLogNodeData self_type;

	CLogNodeData(const self_type&);
	self_type& operator = (const self_type&);

public:
	CLogNodeData(CLogNode* node);
	virtual ~CLogNodeData();

	inline CLogNode* Node(void);

protected:
	CLogNode* const m_node;	/*!< Node */
};

/// Log node
class CLogNode
{
private:
	typedef CLogNode    self_type;

	CLogNode(const self_type&);
	self_type& operator = (const self_type&);

public:
	CLogNode(CLogNode* root = NULL);
	virtual ~CLogNode();

	/// Get log node type
	virtual kLogNode GetType(void) const = 0;

	inline const std::vector<CLogNode*>& Childs(void) const;
	inline std::vector<CLogNode*>& Childs(void);

	inline const CLogNode* Root(void) const;
	inline CLogNode*& Root(void);

	inline const CLogNode* Next(void) const;
	inline CLogNode*& Next(void);

	inline void SetUserData(CLogNodeData* data);
	inline CLogNodeData* GetUserData(void);
	inline const CLogNodeData* GetUserData(void) const;

protected:
	std::vector<CLogNode*> m_childs;	/*!< Childs */
	CLogNode* m_root;					/*!< Root node */
	CLogNode* m_next;					/*!< Next node */
	CLogNodeData* m_user;				/*!< Log node data */
};

/// Header node
class CLogNodeHeader : public CLogNode
{
public:
	CLogNodeHeader(const CRcsFile& header, CLogNode* root = NULL);

	virtual kLogNode GetType(void) const;

	inline const CRcsFile& operator*() const;

protected:
	const CRcsFile m_header; /*!< Header */
};//class CLogNodeHeader

/// Revision node
class CLogNodeRev : public CLogNode
{
public:
	CLogNodeRev(const CRevFile& rev, CLogNode* root = NULL);

	virtual kLogNode GetType(void) const;

	inline const CRevFile& operator*() const;

protected:
	const CRevFile m_rev; /*!< Revision */
};//class CLogNodeRev

/// Tag node
class CLogNodeTag : public CLogNode
{
public:
	CLogNodeTag(const std::string& tag, CLogNode* root = NULL);

	virtual kLogNode GetType(void) const;

	inline const std::string& operator*() const;

protected:
	const std::string m_tag;	/*!< Tag */
};//class CLogNodeTag

/// Branch node
class CLogNodeBranch : public CLogNode
{
public:
	CLogNodeBranch(const std::string& branch, CLogNode* root = NULL);

	virtual kLogNode GetType(void) const;

	inline const std::string& operator*() const;

protected:
	const std::string m_branch; /*!< Branch */
};//class CLogNodeBranch

//////////////////////////////////////////////////////////////////////////
// CRevNumber inline implementation

inline CRevNumber::size_type CRevNumber::size() const
{
	return m_allDigits.size();
}

inline bool CRevNumber::empty() const
{
	return m_allDigits.empty();
}

inline const CRevNumber::items_type& CRevNumber::IntList(void) const
{
	return m_allDigits;
}

inline CRevNumber::items_type& CRevNumber::IntList(void)
{
	return m_allDigits;
}

inline const std::string& CRevNumber::Tag(void) const
{
	return m_tagName;
}

inline std::string& CRevNumber::Tag(void)
{
	return m_tagName;
}

//////////////////////////////////////////////////////////////////////////
// CRevFile	inline implementation

inline const CRevNumber& CRevFile::RevNum(void) const
{
	return m_revNum;
}

inline CRevNumber& CRevFile::RevNum(void)
{
	return m_revNum;
}

inline const struct tm& CRevFile::RevTime(void) const
{
	return m_revTime;
}

inline struct tm& CRevFile::RevTime(void)
{
	return m_revTime;
}

inline const std::string& CRevFile::Locker(void) const
{
	return m_locker;
}

inline std::string& CRevFile::Locker(void)
{
	return m_locker;
}

inline const std::vector<CRevNumber>& CRevFile::BranchesList(void) const
{
	return m_branchesList;
}

inline std::vector<CRevNumber>& CRevFile::BranchesList(void)
{
	return m_branchesList;
}

inline CRevNumber& CRevFile::LastBranche(void)
{
	return m_branchesList[m_branchesList.size() - 1];
}

inline const std::string& CRevFile::Author(void) const
{
	return m_author;
}

inline std::string& CRevFile::Author(void)
{
	return m_author;
}

inline const std::string& CRevFile::State(void) const
{
	return m_state;
}

inline std::string& CRevFile::State(void)
{
	return m_state;
}

inline int CRevFile::ChgPos(void) const
{
	return m_chgPos;
}

inline int& CRevFile::ChgPos(void)
{
	return m_chgPos;
}

inline int CRevFile::ChgNeg(void) const
{
	return m_chgNeg;
}

inline int& CRevFile::ChgNeg(void)
{
	return m_chgNeg;
}

inline const std::string& CRevFile::DescLog(void) const
{
	return m_descLog;
}

inline std::string& CRevFile::DescLog(void)
{
	return m_descLog;
}

inline const std::string& CRevFile::KeywordSubst(void) const
{
	return m_keywordSubst;
}

inline std::string& CRevFile::KeywordSubst(void)
{
	return m_keywordSubst;
}

inline const std::string& CRevFile::CommitID(void) const
{
	return m_commitID;
}

inline std::string& CRevFile::CommitID(void)
{
	return m_commitID;
}

inline const std::string& CRevFile::Filename(void) const
{
	return m_filename;
}

inline std::string& CRevFile::Filename(void)
{
	return m_filename;
}

inline const CRevNumber& CRevFile::MergePoint(void) const
{
	return m_mergePoint;
}

inline CRevNumber& CRevFile::MergePoint(void)
{
	return m_mergePoint;
}

//////////////////////////////////////////////////////////////////////////
// CRcsFile inline implementation

inline const std::string& CRcsFile::RcsFile(void) const
{
	return m_rcsFile;
}

inline std::string& CRcsFile::RcsFile(void)
{
	return m_rcsFile;
}

inline const std::string& CRcsFile::WorkingFile(void) const
{
	return m_workingFile;
}

inline std::string& CRcsFile::WorkingFile(void)
{
	return m_workingFile;
}

inline const CRevNumber& CRcsFile::HeadRev(void) const
{
	return m_headRev;
}

inline CRevNumber& CRcsFile::HeadRev(void)
{
	return m_headRev;
}

inline const CRevNumber& CRcsFile::BranchRev(void) const
{
	return m_branchRev;
}

inline CRevNumber& CRcsFile::BranchRev(void)
{
	return m_branchRev;
}

inline const std::string& CRcsFile::KeywordSubst(void) const
{
	return m_keywordSubst;
}

inline std::string& CRcsFile::KeywordSubst(void)
{
	return m_keywordSubst;
}

inline int CRcsFile::SelRevisions(void) const
{
	return m_selRevisions;
}

inline int& CRcsFile::SelRevisions(void)
{
	return m_selRevisions;
}

inline int CRcsFile::TotRevisions(void) const
{
	return m_totRevisions;
}

inline int& CRcsFile::TotRevisions(void)
{
	return m_totRevisions;
}

inline const std::vector<std::string>& CRcsFile::AccessList(void) const
{
	return m_accessList;
}

inline std::vector<std::string>& CRcsFile::AccessList(void)
{
	return m_accessList;
}

inline std::string& CRcsFile::LastAccess(void)
{
	return m_accessList[m_accessList.size() - 1];
}

inline const std::vector<CRevNumber>& CRcsFile::SymbolicList(void) const
{
	return m_symbolicList;
}

inline std::vector<CRevNumber>& CRcsFile::SymbolicList(void)
{
	return m_symbolicList;
}

inline CRevNumber& CRcsFile::LastSymbName(void)
{
	return m_symbolicList[m_symbolicList.size() - 1];
}

inline const std::vector<CRevNumber>& CRcsFile::LocksList(void) const
{
	return m_locksList;
}

inline std::vector<CRevNumber>& CRcsFile::LocksList(void)
{
	return m_locksList;
}

inline CRevNumber& CRcsFile::LastLock(void)
{
	return m_locksList[m_locksList.size() - 1];
}

inline bool CRcsFile::LockStrict(void) const
{
	return m_lockStrict;
}

inline bool& CRcsFile::LockStrict(void)
{
	return m_lockStrict;
}

inline const std::vector<CRevFile>& CRcsFile::AllRevs(void) const
{
	return m_allRevs;
}

inline std::vector<CRevFile>& CRcsFile::AllRevs(void)
{
	return m_allRevs;
}

inline CRevFile& CRcsFile::LastRev(void)
{
	return m_allRevs[m_allRevs.size() - 1];
}

inline const std::string& CRcsFile::DescLog(void) const
{
	return m_descLog;
}

inline std::string& CRcsFile::DescLog(void)
{
	return m_descLog;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeData inline implementation

inline CLogNode* CLogNodeData::Node(void)
{
	return m_node;
}

//////////////////////////////////////////////////////////////////////////
// CLogNode inline implementation

inline const std::vector<CLogNode*>& CLogNode::Childs(void) const
{
	return m_childs;
}

inline std::vector<CLogNode*>& CLogNode::Childs(void)
{
	return m_childs;
}

inline const CLogNode* CLogNode::Root(void) const
{
	return m_root;
}

inline CLogNode*& CLogNode::Root(void)
{
	return m_root;
}

inline const CLogNode* CLogNode::Next(void) const
{
	return m_next;
}

inline CLogNode*& CLogNode::Next(void)
{
	return m_next;
}

inline void CLogNode::SetUserData(CLogNodeData* const data)
{
	assert(m_user==NULL);

	m_user = data;
}

inline CLogNodeData* CLogNode::GetUserData(void)
{
	return m_user;
}

inline const CLogNodeData* CLogNode::GetUserData(void) const
{
	return m_user;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeHeader inline implementation

inline const CRcsFile& CLogNodeHeader::operator*() const
{
	return m_header;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeRev inline implementation

inline const CRevFile& CLogNodeRev::operator*() const
{
	return m_rev;
}

//////////////////////////////////////////////////////////////////////////
// CLogNodeTag inline implementation

inline const std::string& CLogNodeTag::operator*() const
{
	return m_tag;
}

//////////////////////////////////////////////////////////////////////////
/// CLogNodeBranch

inline const std::string& CLogNodeBranch::operator*() const
{
	return m_branch;
}

////////////////////////////////////////////////////////////////////////////////
#endif
