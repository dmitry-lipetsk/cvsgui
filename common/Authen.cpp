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
 * Authen.cpp --- class to manage CVS authentication kind
 */

#include "stdafx.h"

#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "CvsPrefs.h"
#include "Authen.h"
#include "AppConsole.h"
#include "MultiFiles.h"
#include "PromptFiles.h"
#include "CvsArgs.h"
#include "uwidget.h"
#include "SmartComboBox.h"
#include "CvsCommands.h"
#include "cvsgui_i18n.h"

#include <algorithm>

#if INTERNAL_AUTHEN

#ifdef WIN32
#	include "resource.h"
#endif

using namespace std;

static CPersistentT<AuthenKind> gAuthenKind("P_Authen",
#ifdef TARGET_OS_MAC
	rhosts,
#else /* !TARGET_OS_MAC */
	local,
#endif /* !TARGET_OS_MAC */
	kNoClass);

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
	#if !TARGET_API_MAC_OSX
	#define putenv(a)
	#endif
#endif

// Under windows we need an equal sign, which would make a "real" unsetenv fail
#ifdef WIN32
	#define STR_UNSETENV_EQUAL "="
#else
	#define STR_UNSETENV_EQUAL
#endif

#ifdef WIN32
#define unsetenv(a) putenv(a STR_UNSETENV_EQUAL)
#endif

#ifndef qUnix
string AuthenModel::oldHome;
#endif


Authen gAuthen;

//////////////////////////////////////////////////////////////////////////
// Authentication class

/// Get authentication kind
AuthenKind Authen::kind(void)
{
	return gAuthenKind;
}

/// Set authentication kind
void Authen::setkind(AuthenKind newkind)
{
	gAuthenKind = newkind;
}

/// Get authentication kind text description
const char* Authen::kindstr(void)
{
	static string res;

	switch( kind() )
	{
	default:
	case rhosts:
		res = _i18n(".rhosts authentication");
		break;
	case pserver: 
		res = _i18n("password authentication");
		break;
	case kserver:
		res = _i18n("kerberos authentication");
		break;
	case local:
		res = _i18n("local mounted directory");
		break;
	case ssh:
		res = _i18n("ssh authentication");
		break;
	case ntserver:
		res = _i18n("NT server services");
		break;
	case gserver:
		res = _i18n("GSSAPI authentication");
		break;
	case sspi:
		res = _i18n("sspi authentication");
		break;
	case server:
		res = _i18n("server authentication");
		break;
	}

	if( kind() == pserver && gCvsPrefs.UseProxy() )
	{
		res += _i18n(" using a proxy");
	}

	return res.c_str();
}

/// Get current authentication token
const char* Authen::token(void)
{
	return Authen::token(kind());
}

/// Get specified authentication token
const char* Authen::token(AuthenKind kind)
{
	static string res;

	switch( kind )
	{
	default :
	case rhosts:
		res = "";
		break;
	case pserver :
		res = ":pserver";
		if(gCvsPrefs.UseProxy())
		{
			res += ";proxy=";
			res += gCvsPrefs.ProxyHost();
			res += ";proxyport=";
			res += gCvsPrefs.ProxyPort();
		}
		res += ':';
		break;
	case kserver:
		res = ":kserver:";
		break;
	case local:
		res = ":local:";
		break;
	case ssh: 
		res = ":ext:";
		break;
	case ntserver:
		res = ":ntserver:";
		break;
	case gserver:
		res = ":gserver:";
		break;
	case sspi:
		res = ":sspi:";
		break;
	case server:
		res = ":server:";
		break;
	}

	return res.c_str();
}

/// Skip token
const char* Authen::skiptoken(const char* cvsroot)
{
	const char* secondColon;

	if( cvsroot[0] != ':' || (secondColon = strchr(cvsroot + 1, ':')) == NULL )
		return cvsroot;

	return secondColon + 1;
}

/// Get authentication token for specified kind
const char* Authen::kindToToken(AuthenKind kind)
{
	AuthenModel* model = AuthenModel::GetInstance(kind);
	return model->GetToken();
}

/// Get authentication kind for specified token
AuthenKind Authen::tokenToKind(const char* token)
{
	std::vector<AuthenModel*>& allInstances = AuthenModel::GetAllInstances();
	std::vector<AuthenModel*>::const_iterator i;
	for(i = allInstances.begin(); i != allInstances.end(); ++i)
	{
		AuthenModel* model = *i;
		if( strcmp(model->GetToken(), token) == 0 )
			return model->GetKind();
	}

	cvs_err(_i18n("Unknown authentication kind '%s'\n"), token);

	return local;
}

/// Get authentication number given the kind
short Authen::kindToNum(AuthenKind kind)
{	
	std::vector<AuthenModel*>& allInstances = AuthenModel::GetAllInstances();
	std::vector<AuthenModel*>::const_iterator i;
	int cnt = 0;
	for(i = allInstances.begin(); i != allInstances.end(); ++i, cnt++)
	{
		AuthenModel* model = *i;
		if( model->GetKind() == kind )
			return cnt;
	}

	return 0;
}

/// Get authentication kind given the number
AuthenKind Authen::numToKind(short num)
{
	std::vector<AuthenModel*>& allInstances = AuthenModel::GetAllInstances();
	std::vector<AuthenModel*>::const_iterator i;
	int cnt = 0;
	for(i = allInstances.begin(); i != allInstances.end(); ++i, cnt++)
	{
		AuthenModel* model = *i;
		if( cnt == num )
			return model->GetKind();
	}

	return local;
}

/// Parse cvsroot
bool Authen::parse_cvsroot(const char* CVSroot, string& theMethod, string& theUser, string& theHost, string& thePath)
{
    char* p;
	string copyroot(CVSroot);
	const char* cvsroot_copy = copyroot.c_str();
	
	theMethod = "";
	theUser = "";
	theHost = "";
	thePath = "";

    if( *cvsroot_copy == ':' )
	{
		const char* method = ++cvsroot_copy;
		bool have_semicolon;

		/* Access method specified, as in
		* "cvs -d :pserver:user@host:/path",
		* "cvs -d :local:e:\path",
		* "cvs -d :kserver:user@host:/path", or
		* "cvs -d :fork:/path".
		* We need to get past that part of CVSroot before parsing the
		* rest of it.
		*/

		if( !(p = strchr(method, ':')) )
			return false;

		p = strpbrk(method, ":;#");
		if( p == 0L )
			return false;

		have_semicolon = (*p == ';' || *p == '#');
		*p = '\0';
		cvsroot_copy = ++p;

		theMethod = method;

		while(have_semicolon)
		{
			/* More elaborate implementation would allow multiple
			semicolons, for example:

			:server;rsh=34;command=cvs-1.6:

			we will allow
			:server;port=22;proxy=www-proxy;proxyport=8080:

			we will also allow # as well as ; as a separator to
			avoid having to quote the root in a shell.
			*/
			/* FIXME: lots of error conditions should be better handled,
			e.g. garbage after the number or no valid number.

			Would be nice to have testcases for some of these cases
			including the error cases.  */
			p = strpbrk(cvsroot_copy, ":;#");
			if( p == NULL )
				return false;

			/* pick up more options if we have them */
			have_semicolon = (*p == ';' || *p == '#');
			*p = '\0';

			cvsroot_copy = ++p;
		}
	}

    if( theMethod != "local" )
    {
		/* Check to see if there is a username in the string. */

		if( (p = strchr(cvsroot_copy, '@')) != NULL )
		{
		    *p = '\0';
		    theUser = cvsroot_copy;
		    cvsroot_copy = ++p;
		}

		if( (p = strchr(cvsroot_copy, ':')) != NULL )
		{
		    *p = '\0';
		    theHost = cvsroot_copy;
		    cvsroot_copy = ++p;
		}
    }

    thePath = cvsroot_copy;
    
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Authentications collection

/// Authentication container
std::vector<AuthenModel*>* AuthenModel::m_instances = 0L;

/// Constructor
AuthenModel::AuthenModel()
{
	if( m_instances == 0L )
	{
		m_instances = new std::vector<AuthenModel*>;
		if( m_instances == 0L )
			throw std::bad_alloc();
	}
	
	m_instances->push_back(this);
}

/// Destructor
AuthenModel::~AuthenModel()
{
	std::vector<AuthenModel*>::iterator i;
	i = std::find(m_instances->begin(), m_instances->end(), this);
	if( i != m_instances->end() )
		m_instances->erase(i);
	
	if( m_instances->empty() )
		delete m_instances;
}

/// Get authentication collection
std::vector<AuthenModel*>& AuthenModel::GetAllInstances(void)
{
	return *AuthenModel::m_instances;
}

/// Get authentication instance for given kind
AuthenModel* AuthenModel::GetInstance(AuthenKind k)
{
	std::vector<AuthenModel*>& allInstances = GetAllInstances();
	std::vector<AuthenModel*>::const_iterator i;
	for(i = allInstances.begin(); i != allInstances.end(); ++i)
	{
		AuthenModel* model = *i;
		if( model->GetKind() == k )
			return model;
	}
	
	// default to local
	cvs_err(_i18n("Unknown authentication kind %d\n"), k);

	return GetInstance(local);
}

/// Setup environment
void AuthenModel::OnSetupEnv(void)
{
	/* under unix there should always be the correct home path 
	 * in the emviroment. So we have not to overwrite it.*/
#ifndef qUnix
	static string home;
  
	oldHome = getenv("HOME");
  
	home = "HOME=";
	home += gCvsPrefs.Home();
	
	putenv(const_cast<char*>(home.c_str()));
#endif
}

/// Restore environment
void AuthenModel::OnRestoreEnv(void)
{
	/* we have not to clear the HOME enviroment on unix */
#ifndef qUnix
	if( oldHome.empty() )
		unsetenv("HOME");
	else
		putenv(const_cast<char*>(oldHome.c_str()));
#endif
}

//////////////////////////////////////////////////////////////////////////
// Authentication: local

class AuthenLocal : public AuthenModel
{
public :
	AuthenLocal() {}
	virtual ~AuthenLocal() {}

	virtual AuthenKind GetKind(void) const { return local; }
	virtual const char* GetToken(void) const { return "local"; }
};


//////////////////////////////////////////////////////////////////////////
// Authentication: ssh

class AuthenSSH : public AuthenModel
{
public :
	AuthenSSH() {}
	virtual ~AuthenSSH() {}

	virtual AuthenKind GetKind(void) const { return ssh; }
	virtual const char* GetToken(void) const { return "ssh"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);

	virtual void OnSetupEnv(void);
	virtual void OnRestoreEnv(void);

	static CPersistentString gSSHName;
	static CPersistentString gSSHOptions;
	static CPersistentBool gRsaIdentity;
	static CMString gRsaIdentityFile;
	static CPersistentString gCurRsaIdentity;

  static const char* RsaIdentityFile(void);
  
private:
  string oldRsh;
  string oldRsaOptions;
  string oldRsaIdentity;
};

CPersistentString AuthenSSH::gSSHName("P_SSHName", "ssh", kAddSettings);
CPersistentString AuthenSSH::gSSHOptions("P_SSHOptions", "", kAddSettings);
CPersistentBool AuthenSSH::gRsaIdentity("P_RsaIdentity", false, kAddSettings);
CMString AuthenSSH::gRsaIdentityFile(10, "P_RsaIdentityFiles");
CPersistentString AuthenSSH::gCurRsaIdentity("P_CurrentRsaIdentity", 0L, kAddSettings);

const char* AuthenSSH::RsaIdentityFile(void)
{
	if( gRsaIdentityFile.GetList().size() != 0 )
		gCurRsaIdentity = gRsaIdentityFile.GetList()[0].c_str();

#ifdef WIN32
	// Fill in a default value, if one empty slot
	if( gCurRsaIdentity.empty())
	{
		char* env = getenv( "HOME" );
		if( env != 0L )
		{
			char default_identity_folder[] = ".ssh/identity";
			char default_identity[ MAX_PATH ];
			
			_makepath(default_identity, NULL, env, default_identity_folder, NULL);
			gCurRsaIdentity = default_identity;
		}
	}
#endif

	return gCurRsaIdentity.empty() ? 0L : (const char*)gCurRsaIdentity.c_str();
}

class UAuthen_SSH : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_SSH)
public:
	UAuthen_SSH();
	virtual ~UAuthen_SSH() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,			// 1
		kRSACheck,			// 2
		kRSAEdit,			// 3
		kRSABtn,			// 4
		kSSHCheck,			// 5
		kSSHName,			// 6
		kOptionsText,		// 7
		kOptionsCheck,		// 8
		kOptionsEdit,		// 9
		kSSHBtn			// 10
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	bool m_rsaidentity;
	string m_rsaidentityfile;
	string m_options;
	
	ev_msg int OnCheckRSA(void);
	ev_msg int OnBtnRSA(void);
	ev_msg int OnCheckName(void);
	ev_msg int OnBtnName(void);
	ev_msg int OnCheckOptions(void);
	
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_SSH, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_SSH, UWidget)
	ON_UCOMMAND(kRSACheck, UAuthen_SSH::OnCheckRSA)
	ON_UCOMMAND(kRSABtn, UAuthen_SSH::OnBtnRSA)
	ON_UCOMMAND(kSSHCheck, UAuthen_SSH::OnCheckName)
	ON_UCOMMAND(kSSHBtn, UAuthen_SSH::OnBtnName)
	ON_UCOMMAND(kOptionsCheck, UAuthen_SSH::OnCheckOptions)
UEND_MESSAGE_MAP()

UAuthen_SSH::UAuthen_SSH() : UWidget(::UEventGetWidID())
{
}

int UAuthen_SSH::OnCheckRSA(void)
{
	m_rsaidentity = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRSACheck, 0L) != 0;

	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRSAEdit, m_rsaidentity), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRSABtn, m_rsaidentity), 0L);

	return 0;
}

int UAuthen_SSH::OnCheckName(void)
{
	bool state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kSSHCheck, 0L) != 0;

	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kSSHName, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kSSHBtn, state), 0L);

	return 0;
}

int UAuthen_SSH::OnCheckOptions(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kOptionsCheck, 0L);

	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kOptionsEdit, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kOptionsText, state), 0L);

	return 0;
}

int UAuthen_SSH::OnBtnRSA(void)
{
	string rsaidentityfile;
	UEventSendMessage(GetWidID(), EV_GETTEXT, kRSAEdit, &rsaidentityfile);
	
	MultiFiles mf;
	if( !rsaidentityfile.empty() )
	{
		string uppath;
		string file;

		if( SplitPath(rsaidentityfile.c_str(), uppath, file) )
		{
			mf.newdir(uppath.c_str());
			mf.newfile(file.c_str());
		}
	}

	if( !BrowserGetMultiFiles(_i18n("Select the RSA identity file"), mf, true) )
		return 0;

	CvsArgs args(false);
	const char* dir = (*mf.begin()).add(args);
	char* const* argv = args.Argv();

	string fullpath;
	fullpath = dir;

	NormalizeFolderPath(fullpath);

	fullpath += argv[0];

	UEventSendMessage(GetWidID(), EV_SETTEXT, kRSAEdit, (void*)fullpath.c_str());

	return 0;
}

int UAuthen_SSH::OnBtnName(void)
{
	string SSHName;
	UEventSendMessage(GetWidID(), EV_GETTEXT, kSSHName, &SSHName);
	
	MultiFiles mf;
	if( !SSHName.empty() )
	{
		string uppath;
		string file;

		if( SplitPath(SSHName.c_str(), uppath, file) )
		{
			mf.newdir(uppath.c_str());
			mf.newfile(file.c_str());
		}
	}

	if( !BrowserGetMultiFiles(_i18n("Select SSH"), mf, true, kSelectExecutable) )
		return 0;

	CvsArgs args(false);
	const char* dir = (*mf.begin()).add(args);
	char* const* argv = args.Argv();

	string fullpath;
	fullpath = dir;

	NormalizeFolderPath(fullpath);

	fullpath += argv[0];

	UEventSendMessage(GetWidID(), EV_SETTEXT, kSSHName, (void*)fullpath.c_str());

	return 0;
}

void UAuthen_SSH::DoDataExchange(bool fill)
{
	if( fill )
	{
		m_rsaidentity = AuthenSSH::gRsaIdentity;
		m_rsaidentityfile = AuthenSSH::RsaIdentityFile();

		string helpMsg = _i18n("* Some Cygwin/OpenSSH options :\n"
			"-v : Verbose mode\n"
			"-C : Request compression\n"
			"-1 : Forces ssh to try protocol version 1 only\n"
			"-2 : Forces ssh to try protocol version 2 only\n"
			"-4 : Forces ssh to use IPv4 addresses only\n"
			"-6 : Forces ssh to use IPv6 addresses only");

		UEventSendMessage(GetWidID(), EV_SETTEXT, kOptionsText, (void*)helpMsg.c_str());

		UEventSendMessage(GetWidID(), EV_SMARTCOMBO_SETFEATURE, kRSAEdit, (void*)USmartCombo::DefaultFeature);
		UEventSendMessage(GetWidID(), EV_SMARTCOMBO_SETITEMS, kRSAEdit, (void*)&AuthenSSH::gRsaIdentityFile);

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kRSAEdit, 0L);
		const CMString::list_t& rsastrings = AuthenSSH::gRsaIdentityFile.GetList();
		CMString::list_t::const_iterator i;
		for(i = rsastrings.begin(); i != rsastrings.end(); ++i)
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kRSAEdit, (void*)(const char*)(*i).c_str());
		}
		
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kRSACheck, m_rsaidentity), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kRSAEdit, (void*)m_rsaidentityfile.c_str());

		if( !AuthenSSH::gSSHName.empty() )
		{
			UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kSSHCheck, 1), 0L);
			UEventSendMessage(GetWidID(), EV_SETTEXT, kSSHName, (void*)AuthenSSH::gSSHName.c_str());
		}

		if( !AuthenSSH::gSSHOptions.empty() )
		{
			UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kOptionsCheck, 1), 0L);
			UEventSendMessage(GetWidID(), EV_SETTEXT, kOptionsEdit, (void*)AuthenSSH::gSSHOptions.c_str());
		}

		OnCheckRSA();
		OnCheckName();
		OnCheckOptions();
	}
	else
	{
		AuthenSSH::gRsaIdentity = m_rsaidentity;
		if( m_rsaidentity )
		{
			UEventSendMessage(GetWidID(), EV_GETTEXT, kRSAEdit, &m_rsaidentityfile);
			AuthenSSH::gRsaIdentityFile.Insert(m_rsaidentityfile.c_str());
			AuthenSSH::gCurRsaIdentity = m_rsaidentityfile.c_str();
		}

		if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kSSHCheck, 0L) )
			UEventSendMessage(GetWidID(), EV_GETTEXT, kSSHName, &AuthenSSH::gSSHName);
		else
			AuthenSSH::gSSHName = "";

		if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kOptionsCheck, 0L) )
			UEventSendMessage(GetWidID(), EV_GETTEXT, kOptionsEdit, &AuthenSSH::gSSHOptions);
		else
			AuthenSSH::gSSHOptions = "";
	}
}

void AuthenSSH::DoSettings(void)
{
	UAuthen_SSH* dlg = new UAuthen_SSH();
	
	try
	{
#ifdef WIN32
		dlg->DoModal(IDD_SSH);
#endif
#if TARGET_RT_MAC_MACHO
		dlg->DoModal(3005);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

void AuthenSSH::GetSettingsDesc(string& desc)
{
	if( !gSSHOptions.empty() )
	{
		desc += _i18n("Using ");
		desc += gSSHOptions.c_str();
		desc += " ";
	}
	
	if( (bool)gRsaIdentity )
	{
		if( gSSHOptions.empty() )
			desc += _i18n("Using ");

		desc += "-i ";
		desc += RsaIdentityFile();
	}
}

void AuthenSSH::OnSetupEnv(void)
{
	AuthenModel::OnSetupEnv();

	static string rsh;
	static string options;
	static string identity;
	
	rsh = "CVS_RSH=";

	oldRsh         = getenv("CVS_RSH");
	oldRsaOptions  = getenv("CVS_RSA_OPTIONS");
	oldRsaIdentity = getenv("CVS_RSA_IDENTITY");
	
	if( gSSHName.empty() )
		rsh += "ssh";
	else
		rsh += gSSHName.c_str();
	
	putenv(const_cast<char*>(rsh.c_str()));

	if( !gSSHOptions.empty() )
	{
		options = "CVS_RSA_OPTIONS=";
		options += gSSHOptions.c_str();

		putenv(const_cast<char*>(options.c_str()));
	}

	if( (bool)gRsaIdentity )
	{
		identity = "CVS_RSA_IDENTITY=";
		identity += RsaIdentityFile();

		putenv (const_cast<char*>(identity.c_str()));
	}
}

void AuthenSSH::OnRestoreEnv(void)
{
	AuthenModel::OnRestoreEnv();

	if( oldRsh.empty() )
		unsetenv("CVS_RSH");
	else
		putenv (const_cast<char*>(oldRsh.c_str()));
  
	if( oldRsaOptions.empty() )
		unsetenv("CVS_RSA_OPTIONS");
	else
		putenv (const_cast<char*>(oldRsaOptions.c_str()));
  
	if( oldRsaIdentity.empty() )
		unsetenv("CVS_RSA_IDENTITY");
	else
		putenv(const_cast<char*>(oldRsaIdentity.c_str()));
}

const char* AuthenSSH::OnGetEnv(const char* env)
{
	if( strcmp(env, "CVS_RSH") == 0 )
	{
		return gSSHName.empty() ? "ssh" : (char*)gSSHName.c_str();
	}
	else if( strcmp(env, "CVS_RSA_OPTIONS") == 0)
	{
		return gSSHOptions.empty() ? 0L : (char*)gSSHOptions.c_str();
	}
	else if( strcmp(env, "CVS_RSA_IDENTITY") == 0 && (bool)gRsaIdentity)
	{
		const char* tmp = RsaIdentityFile();
		if( tmp && *tmp )
		{
#ifdef WIN32
			while( *tmp && iswspace(*tmp) )
#else
			while( *tmp && isspace(*tmp) )
#endif
				tmp++;

			return *tmp ? (char*)tmp : 0L;
		}
		
		return 0L;
	}

	return 0L;
}


bool CvsPrefs::UseKeyChain(void) const
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: pserver

class AuthenPserver : public AuthenModel
{
public :
	AuthenPserver() {}
	virtual ~AuthenPserver() {}

	virtual AuthenKind GetKind(void) const { return pserver; }
	virtual const char* GetToken(void) const { return "pserver"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual bool HasLogin(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);

	virtual void OnSetupEnv(void);
	virtual void OnRestoreEnv(void);
  
private:
	string oldPserverPort;
};

class UAuthen_Pserver : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_Pserver)
public:
	UAuthen_Pserver();
	virtual ~UAuthen_Pserver() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,			// 1
		kCheckPort,			// 2
		kEditPort,			// 3
		kCheckProxy,		// 4
		kNameProxy,			// 5
		kEditProxy			// 6
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	int m_port;
	int m_pport;
	string m_pname;
	
	ev_msg int OnCheckPort(void);
	ev_msg int OnCheckProxy(void);
	
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_Pserver, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_Pserver, UWidget)
	ON_UCOMMAND(kCheckPort, UAuthen_Pserver::OnCheckPort)
	ON_UCOMMAND(kCheckProxy,  UAuthen_Pserver::OnCheckProxy)
UEND_MESSAGE_MAP()

UAuthen_Pserver::UAuthen_Pserver() : UWidget(::UEventGetWidID())
{
}

int UAuthen_Pserver::OnCheckPort(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);

	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, state), 0L);

	return 0;
}

int UAuthen_Pserver::OnCheckProxy(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckProxy, 0L);

	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kNameProxy, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditProxy, state), 0L);

	return 0;
}

void UAuthen_Pserver::DoDataExchange(bool fill)
{
	if( fill )
	{
		m_port = gCvsPrefs.PserverPort();

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, m_port != -1), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kEditPort, m_port == -1 ? 2401 : m_port), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckPort, m_port != -1), 0L);

		m_pport = gCvsPrefs.ProxyPort();
		m_pname = gCvsPrefs.ProxyHost();
		bool useProxy = gCvsPrefs.UseProxy();

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditProxy, useProxy), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kNameProxy, useProxy), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kEditProxy, m_pport), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kNameProxy, (void*)(const char*)m_pname.c_str());
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckProxy, useProxy), 0L);
	}
	else
	{
		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);

		int value = UEventSendMessage(GetWidID(), EV_GETINTEGER, kEditPort, 0L);
		gCvsPrefs.SetPserverPort(state ? value : -1);
		
		state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckProxy, 0L);
		value = UEventSendMessage(GetWidID(), EV_GETINTEGER, kEditProxy, 0L);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kNameProxy, &m_pname);
		gCvsPrefs.SetUseProxy(state != 0);
		
		if( state != 0 )
		{
			gCvsPrefs.SetProxyHost(m_pname.c_str());
			gCvsPrefs.SetProxyPort(value);
		}
	}
}

void AuthenPserver::DoSettings(void)
{
	UAuthen_Pserver* dlg = new UAuthen_Pserver();
	
	try
	{
#if qMacCvsPP
		dlg->DoModal(3002);
#endif
#ifdef WIN32
		dlg->DoModal(IDD_PSERVER);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

void AuthenPserver::GetSettingsDesc(string& desc)
{
	if( gCvsPrefs.PserverPort() == -1 )
		desc += _i18n("Using default port and ");
	else
		desc += _i18n("Using port ");
		desc += gCvsPrefs.PserverPort();
		desc += _i18n(" and ");

	if( !gCvsPrefs.UseProxy() )
		desc += _i18n("no proxy");
	else
		desc += _i18n("using proxy host ");
		desc += gCvsPrefs.ProxyHost();
		desc += _i18n(" with port ");
		desc += gCvsPrefs.ProxyPort();
}

void AuthenPserver::OnSetupEnv(void)
{
	AuthenModel::OnSetupEnv();

	static string options;	

	if( gCvsPrefs.PserverPort() != -1 )
	{
		oldPserverPort = getenv("CVS_PSERVER_PORT");
    
		options = "CVS_PSERVER_PORT=";
		options += gCvsPrefs.PserverPort();

		putenv (const_cast<char*>(options.c_str()));
	}
}

void AuthenPserver::OnRestoreEnv(void)
{
	AuthenModel::OnRestoreEnv();

	if( gCvsPrefs.PserverPort() != -1 )
	{
		if( oldPserverPort.empty() )
				unsetenv("CVS_PSERVER_PORT");
		else
				putenv (const_cast<char*>(oldPserverPort.c_str()));
	}
}

const char* AuthenPserver::OnGetEnv(const char* env)
{
	if( strcmp(env, "CVS_PSERVER_PORT") == 0 )
	{
		if( gCvsPrefs.PserverPort() != -1 )
		{
			static char port[10];
			sprintf(port, "%d", gCvsPrefs.PserverPort());

			return port;
		}
		
		return 0L;
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: rhosts

class AuthenRhosts : public AuthenModel
{
public :
	AuthenRhosts() {}
	virtual ~AuthenRhosts() {}

	virtual AuthenKind GetKind(void) const { return rhosts; }
	virtual const char* GetToken(void) const { return "rhosts"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);

	virtual void OnSetupEnv(void);
	virtual void OnRestoreEnv(void);
private:
	string oldRsa;
	string oldRhostPort;
	string oldServerName;
};

class UAuthen_Rhosts : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_Rhosts)
public:
	UAuthen_Rhosts();
	virtual ~UAuthen_Rhosts() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,			// 1
		kCheckPort,			// 2
		kEditPort,			// 3
		kCheckRSHName,		// 4
		kEditRSHName,		// 5
		kCheckServerName,	// 6
		kEditServerName		// 7
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	int m_port;
	string m_rshname;
	string m_servername;
	
	ev_msg int OnCheckPort(void);
	ev_msg int OnCheckRshName(void);
	ev_msg int OnCheckServerName(void);
	
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_Rhosts, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_Rhosts, UWidget)
	ON_UCOMMAND(kCheckPort, UAuthen_Rhosts::OnCheckPort)
	ON_UCOMMAND(kCheckRSHName, UAuthen_Rhosts::OnCheckRshName)
	ON_UCOMMAND(kCheckServerName, UAuthen_Rhosts::OnCheckServerName)
UEND_MESSAGE_MAP()

UAuthen_Rhosts::UAuthen_Rhosts() : UWidget(::UEventGetWidID())
{
}

int UAuthen_Rhosts::OnCheckPort(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);
	
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, state), 0L);

	return 0;
}

int UAuthen_Rhosts::OnCheckRshName(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRSHName, 0L);
	
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditRSHName, state), 0L);

	return 0;
}

int UAuthen_Rhosts::OnCheckServerName(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckServerName, 0L);
	
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditServerName, state), 0L);

	return 0;
}

void UAuthen_Rhosts::DoDataExchange(bool fill)
{
	if( fill )
	{
		m_port = gCvsPrefs.RhostPort();
		m_rshname = gCvsPrefs.RshName();
		m_servername = gCvsPrefs.ServerName();

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, m_port != -1), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kEditPort, m_port == -1 ? 0 : m_port), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckPort, m_port != -1), 0L);

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditRSHName, !m_rshname.empty()), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditRSHName, (void*)m_rshname.c_str());
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRSHName, !m_rshname.empty()), 0L);

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditServerName, !m_servername.empty()), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditServerName, (void*)m_servername.c_str());
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckServerName, !m_servername.empty()), 0L);
	}
	else
	{
		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);
		int value = UEventSendMessage(GetWidID(), EV_GETINTEGER, kEditPort, 0L);
		gCvsPrefs.SetRhostPort(state ? value : -1);
		
		state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRSHName, 0L);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditRSHName, &m_rshname);
		gCvsPrefs.SetRshName(state ? m_rshname.c_str() : "");

		state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckServerName, 0L);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditServerName, &m_servername);
		gCvsPrefs.SetServerName(state ? m_servername.c_str() : "");
	}
}

void AuthenRhosts::DoSettings(void)
{
	UAuthen_Rhosts* dlg = new UAuthen_Rhosts();
	
	try
	{
#ifdef qMacCvsPP
		dlg->DoModal(3001);
#endif
#ifdef WIN32
		dlg->DoModal(IDD_RSH);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

void AuthenRhosts::GetSettingsDesc(string& desc)
{
	if( gCvsPrefs.RhostPort() == -1 )
		desc += _i18n("Using default port and ");
	else
		desc += _i18n("Using port ");
		desc += gCvsPrefs.RhostPort();
		desc += _i18n(" and ");

	if( gCvsPrefs.RshName() == 0L )
		desc += _i18n("default shell name");
	else
		desc += _i18n("shell named ");
		desc += gCvsPrefs.RshName();
}

void AuthenRhosts::OnSetupEnv(void)
{
	AuthenModel::OnSetupEnv();

	static string rsh;
	static string port;
	static string server;
	
	oldRsa        = getenv("CVS_RSH");
	oldRhostPort  = getenv("CVS_RCMD_PORT");
	oldServerName = getenv("CVS_SERVER");
  
	if( gCvsPrefs.RshName() != 0L )
	{
		rsh = "CVS_RSH=";
		rsh += gCvsPrefs.RshName();

		putenv (const_cast<char*>(rsh.c_str()));
	}

	if( gCvsPrefs.RhostPort() != -1 )
	{
		port = "CVS_RCMD_PORT=";
		port += gCvsPrefs.RhostPort();
		
		putenv (const_cast<char*>(port.c_str()));
	}

	if( gCvsPrefs.ServerName() != 0L )
	{
		server = "CVS_SERVER=";
		server += gCvsPrefs.ServerName();

		putenv (const_cast<char*>(server.c_str()));
	}
}

void AuthenRhosts::OnRestoreEnv(void)
{
	AuthenModel::OnRestoreEnv();

	if( oldRsa.empty() )
		unsetenv("CVS_RSH");
	else
		putenv (const_cast<char*>(oldRsa.c_str()));
  
	if( oldRhostPort.empty() )
		unsetenv("CVS_RCMD_PORT");
	else
		putenv (const_cast<char*>(oldRhostPort.c_str()));
  
	if( oldServerName.empty() )
		unsetenv("CVS_SERVER");
	else
		putenv (const_cast<char*>(oldServerName.c_str()));
}

const char* AuthenRhosts::OnGetEnv(const char* env)
{
	if( strcmp(env, "CVS_RSH") == 0 )
	{
		return (char*)gCvsPrefs.RshName();
	}
	else if( strcmp(env, "CVS_RCMD_PORT") == 0 )
	{
		if( gCvsPrefs.RhostPort() != -1 )
		{
			static char port[10];
			sprintf(port, "%d", gCvsPrefs.RhostPort());

			return port;
		}
		
		return 0L;
	}
	else if( strcmp(env, "CVS_SERVER") == 0 )
	{
		return (char*)gCvsPrefs.ServerName();
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: kserver

class AuthenKserver : public AuthenModel
{
public :
	AuthenKserver() {}
	virtual ~AuthenKserver() {}

	virtual AuthenKind GetKind(void) const { return kserver; }
	virtual const char* GetToken(void) const { return "kserver"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);
};

class UAuthen_Kserver : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_Kserver)
public:
	UAuthen_Kserver();
	virtual ~UAuthen_Kserver() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,			// 1
		kCheckPort,			// 2
		kEditPort			// 3
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	int m_port;
	
	ev_msg int OnCheckPort(void);
	
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_Kserver, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_Kserver, UWidget)
	ON_UCOMMAND(kCheckPort, UAuthen_Kserver::OnCheckPort)
UEND_MESSAGE_MAP()

UAuthen_Kserver::UAuthen_Kserver() : UWidget(::UEventGetWidID())
{
}

int UAuthen_Kserver::OnCheckPort(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);
	
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, state), 0L);

	return 0;
}

void UAuthen_Kserver::DoDataExchange(bool fill)
{
	if( fill )
	{
		m_port = gCvsPrefs.KserverPort();

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kEditPort, m_port != -1), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kEditPort, m_port == -1 ? 0 : m_port), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckPort, m_port != -1), 0L);
	}
	else
	{
		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPort, 0L);
		int value = UEventSendMessage(GetWidID(), EV_GETINTEGER, kEditPort, 0L);
		gCvsPrefs.SetKserverPort(state ? value : -1);
	}
}

void AuthenKserver::DoSettings(void)
{
	UAuthen_Kserver* dlg = new UAuthen_Kserver();
	
	try
	{
#if qMacCvsPP
		dlg->DoModal(3003);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

void AuthenKserver::GetSettingsDesc(string& desc)
{
	if( gCvsPrefs.KserverPort() == -1 )
		desc += _i18n("Using default port");
	else
		desc += _i18n("Using port ");
		desc += gCvsPrefs.KserverPort();
}

const char* AuthenKserver::OnGetEnv(const char* env)
{
	return 0L;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: ntserver

class AuthenNtserver : public AuthenModel
{
public :
	AuthenNtserver() {}
	virtual ~AuthenNtserver() {}

	virtual AuthenKind GetKind(void) const { return ntserver; }
	virtual const char* GetToken(void) const { return "ntserver"; }

	virtual bool HasSettings(void) const { return false; }
	virtual bool HasHost(void) const { return true; }

	virtual const char* OnGetEnv(const char* env);
};

const char* AuthenNtserver::OnGetEnv(const char* env)
{
	return 0L;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: gserver

class AuthenGserver : public AuthenModel
{
public :
	AuthenGserver() {}
	virtual ~AuthenGserver() {}

	virtual AuthenKind GetKind(void) const { return gserver; }
	virtual const char* GetToken(void) const { return "gserver"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }

	virtual bool HasEncryption(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);
};

void AuthenGserver::GetSettingsDesc(string& desc)
{
	if( gCvsPrefs.EncryptCommunication() )
		desc += _i18n("Encryption turned on");
	else
		desc += _i18n("Encryption turned off");
}

const char* AuthenGserver::OnGetEnv(const char* env)
{
	return 0L;
}

class UAuthen_Gserver : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_Gserver)
public:
	UAuthen_Gserver();
	virtual ~UAuthen_Gserver() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kCheckEncrypt			// 2
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_Gserver, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_Gserver, UWidget)
UEND_MESSAGE_MAP()

UAuthen_Gserver::UAuthen_Gserver() : UWidget(::UEventGetWidID())
{
}

void UAuthen_Gserver::DoDataExchange(bool fill)
{
	if( fill )
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckEncrypt, gCvsPrefs.EncryptCommunication()), 0L);
	}
	else
	{
		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckEncrypt, 0L);
		gCvsPrefs.SetEncryptCommunication(state != 0);
	}
}

void AuthenGserver::DoSettings(void)
{
	UAuthen_Gserver* dlg = new UAuthen_Gserver();
	
	try
	{
#ifdef WIN32
		dlg->DoModal(IDD_GSERVER);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: sspi

class AuthenSspi : public AuthenModel
{
public :
	AuthenSspi() {}
	virtual ~AuthenSspi() {}

	virtual AuthenKind GetKind(void) const { return sspi; }
	virtual const char* GetToken(void) const { return "sspi"; }

	virtual bool HasSettings(void) const { return true; }
	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual bool HasEncryption(void) const { return true; }
	virtual bool HasLogin(void) const { return true; }

	virtual void DoSettings(void);
	virtual void GetSettingsDesc(string& desc);

	virtual const char* OnGetEnv(const char* env);
};

void AuthenSspi::GetSettingsDesc(string& desc)
{
	if( gCvsPrefs.EncryptCommunication() )
		desc += _i18n("Encryption turned on");
	else
		desc += _i18n("Encryption turned off");
}

const char* AuthenSspi::OnGetEnv(const char* env)
{
	return 0L;
}

class UAuthen_Sspi : public UWidget
{
	UDECLARE_DYNAMIC(UAuthen_Sspi)
public:
	UAuthen_Sspi();
	virtual ~UAuthen_Sspi() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kCheckEncrypt			// 2
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UAuthen_Sspi, UWidget)

UBEGIN_MESSAGE_MAP(UAuthen_Sspi, UWidget)
UEND_MESSAGE_MAP()

UAuthen_Sspi::UAuthen_Sspi() : UWidget(::UEventGetWidID())
{
}

void UAuthen_Sspi::DoDataExchange(bool fill)
{
	if( fill )
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckEncrypt, gCvsPrefs.EncryptCommunication()), 0L);
	}
	else
	{
		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckEncrypt, 0L);
		gCvsPrefs.SetEncryptCommunication(state != 0);
	}
}

void AuthenSspi::DoSettings(void)
{
	UAuthen_Sspi* dlg = new UAuthen_Sspi();
	
	try
	{
#ifdef WIN32
		dlg->DoModal(IDD_SSPI);
#endif
	}
	catch(...)
	{
	}
	
	delete dlg;
}

//////////////////////////////////////////////////////////////////////////
// Authentication: server

class AuthenServer : public AuthenModel
{
public :
	AuthenServer() {}
	virtual ~AuthenServer() {}

	virtual AuthenKind GetKind(void) const { return server; }
	virtual const char* GetToken(void) const { return "server"; }

	virtual bool HasHost(void) const { return true; }
	virtual bool HasUser(void) const { return true; }

	virtual const char* OnGetEnv(const char* env);
};

const char* AuthenServer::OnGetEnv(const char* env)
{
	return 0L;
}

//////////////////////////////////////////////////////////////////////////
// Authentication instances

static AuthenLocal localInstance;
static AuthenPserver pserverInstance;
static AuthenRhosts rhostsInstance;

#ifndef WIN32
	static AuthenKserver kserverInstance;
#else
	static AuthenNtserver ntserverInstance;
	static AuthenGserver gserverInstance;
	static AuthenSspi sspiInstance;
	static AuthenServer serverInstance;
#endif

static AuthenSSH sshInstance;

#endif /* INTERNAL_AUTHEN */
