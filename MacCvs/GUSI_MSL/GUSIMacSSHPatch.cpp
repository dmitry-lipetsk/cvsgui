#include <GUSIInternal.h>
#include <GUSIBasics.h>
#include <GUSIContext.h>
#include <GUSIConfig.h>
#include <GUSIDiag.h>
#include <GUSISocket.h>
#include <GUSIPThread.h>
#include <GUSIFactory.h>
#include <GUSIDevice.h>
#include <GUSIDescriptor.h>
#include <GUSIOTNetDB.h>

#include "GUSIMSLSetup.h"
#include <unistd.h>
#include <sys/socket.h>
#include "lsh_context.h"

extern "C" {
	void add_one_file(struct lshcontext *context, int fd);
	void remove_one_file(struct lshcontext *context, int fd);
}

/*
 * we need to track open()/dup()/close()/socket() calls to close files/sockets
 * upon abort/exit
 */

/*
 * open.
 */

int	open(const char * path, int mode, ...)
{
	GUSIErrorSaver			saveError;
	GUSIDeviceRegistry *	factory	= GUSIDeviceRegistry::Instance();
	GUSIDescriptorTable *	table	= GUSIDescriptorTable::Instance();
	GUSISocket * 			sock;
	int						fd;
	
	if (sock = factory->open(path, mode)) {
		if ((fd = table->InstallSocket(sock)) > -1) {
			lshcontext *context = (lshcontext *)pthread_getspecific(ssh2threadkey);
			if ( context ) {
				add_one_file(context, fd);
			}
			return fd;
		}
		sock->close();
	}
	if (!errno)
		return GUSISetPosixError(ENOMEM);
	else
		return -1;
}

/*
 * dup.
 */

int dup(int s)
{
	GUSIDescriptorTable		*table	= GUSIDescriptorTable::Instance();
	GUSISocket				*sock	= GUSIDescriptorTable::LookupSocket(s);
	int						fd;
	lshcontext				*context = (lshcontext *)pthread_getspecific(ssh2threadkey);

	if (!sock)
		return -1;

	fd = table->InstallSocket(sock);
	if ( context ) {
		add_one_file(context, fd);
	}
	return fd;
}

/*
 * dup2.
 */

int dup2(int s, int s1)
{
	GUSIDescriptorTable		*table	= GUSIDescriptorTable::Instance();
	GUSISocket				*sock	= GUSIDescriptorTable::LookupSocket(s);
	int						fd;
	lshcontext				*context = (lshcontext *)pthread_getspecific(ssh2threadkey);

	if (!sock)
		return -1;

	table->RemoveSocket(s1);
	fd = table->InstallSocket(sock, s1);
	if ( context && s1 != fd ) {
		remove_one_file(context, s1);
		add_one_file(context, fd);
	}
	return fd;
}

/*
 * socket.
 */

int socket(int domain, int type, int protocol)
{
	GUSIErrorSaver			saveError;
	GUSISocketFactory *		factory	= GUSISocketDomainRegistry::Instance();
	GUSIDescriptorTable *	table	= GUSIDescriptorTable::Instance();
	GUSISocket * 			sock;
	int						fd;
	
	if (sock = factory->socket(domain, type, protocol)) {
		if ((fd = table->InstallSocket(sock)) > -1) {
			lshcontext *context = (lshcontext *)pthread_getspecific(ssh2threadkey);
			if ( context ) {
				add_one_file(context, fd);
			}
			return fd;
		}
		sock->close();
	}
	if (!errno)
		return GUSISetPosixError(ENOMEM);
	else
		return -1;
}

/*
 * close.
 */

int close(int s)
{
	if ( s > STDERR_FILENO ) {
		GUSIDescriptorTable *	table	= GUSIDescriptorTable::Instance();
		lshcontext *context = (lshcontext *)pthread_getspecific(ssh2threadkey);
		if ( context ) {
			remove_one_file(context, s);
		}
		return table->RemoveSocket(s);
	} else {
		/* we don't close stdin/stdout/stderr */
		return 0;
	}
}

/*
 * we don't use the /etc/services files from prefs...
 */

class GUSIFileServiceDB : public GUSIServiceDB {
public:
	static GUSIFileServiceDB * Instance();
protected:
	FILE *			fFile;
	pthread_mutex_t	fLock;
	
	GUSIFileServiceDB(FILE * file) 
		: fFile(file), fLock(0) { }
	
	virtual void	Reset();
	virtual void 	Next();
};

/*
 * we don't use the /etc/services files from prefs...
 */

GUSIFileServiceDB * GUSIFileServiceDB::Instance()
{
	return static_cast<GUSIFileServiceDB *>(nil);
}
