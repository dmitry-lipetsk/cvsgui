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

/*
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * Authen.h --- class to manage CVS authentication kind
 */

#ifndef AUTHEN_H
#define AUTHEN_H

#if INTERNAL_AUTHEN

#include "MultiString.h"

//! Authentications
/*! Enum to specify the authentications supported */
typedef enum
{
	rhosts,		/*!< .rhosts file (rsh) */
	pserver,	/*!< pserver */
	kserver,	/*!< kserver */
	local,		/*!< local mount */
	ssh,		/*!< ssh */
	ntserver,	/*!< ntserver */
	gserver,	/*!< gserver */
	sspi,		/*!< sspi */
	server		/*!< server */
} AuthenKind;

/*! \class Authen Authen.h "common/Authen.h"
 *  \brief Authentication class
 */
class Authen
{
public :
	Authen() {}
	
	AuthenKind kind(void);
	const char* kindstr(void);
	void setkind(AuthenKind newkind);
	
	static const char* kindToToken(AuthenKind k);
	static AuthenKind tokenToKind(const char* t);
	static short kindToNum(AuthenKind k);
	static AuthenKind numToKind(short num);

	static bool parse_cvsroot(const char* CVSroot,
		std::string& theMethod, std::string& theUser, std::string& theHost, std::string& thePath);

	// ":pserver:", ":kserver:"
	const char* token(void);
	static const char* token(AuthenKind k);
	static const char* skiptoken(const char* cvsroot);
};

/*! \class AuthenModel Authen.h "common/Authen.h"
 *  \brief Authentication model class
 */
class AuthenModel
{
public :
	AuthenModel();
	virtual ~AuthenModel();

	static std::vector<AuthenModel*>& GetAllInstances(void);
	static AuthenModel* GetInstance(AuthenKind k);

	virtual AuthenKind GetKind(void) const = 0;
	virtual const char* GetToken(void) const = 0;

	virtual bool HasSettings(void) const { return false; }
	virtual bool HasHost(void) const { return false; }
	virtual bool HasUser(void) const { return false; }

	virtual bool HasEncryption(void) const { return false; }
	virtual bool HasLogin(void) const { return false; }

	virtual void DoSettings(void) {}
	virtual void GetSettingsDesc(std::string& desc) { desc = ""; }

	virtual const char* OnGetEnv(const char* env) { return 0L; }

	/* This is intended for "cvs release". This command spawns
	another cvs and use the output for doing the release. The
	problem is that it beats the cvsgui protocol and thus we need
	to set up the env. variable correctly */
	virtual void OnSetupEnv(void);
	virtual void OnRestoreEnv(void);

protected:
	static std::vector<AuthenModel*>* m_instances;
  /* we have not to change the HOME enviroment on unix */
#ifndef qUnix
  static string oldHome;
#endif
};

extern Authen gAuthen;

#endif /* INTERNAL_AUTHEN */

#endif /* AUTHEN_H */
