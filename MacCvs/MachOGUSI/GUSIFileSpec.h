#pragma once

class GUSICatInfo {
public:
	bool		IsFile() const;

	const FSCatalogInfo &				Info() const	{	return info; }
	operator const FSCatalogInfo &() const				{	return info; }

	FSCatalogInfo	info;
};

inline bool GUSICatInfo::IsFile() const
{
	return !(info.nodeFlags & kFSNodeIsDirectoryMask);
}

class GUSIFileSpec
{
public:
 // Construct from full or relative path
 GUSIFileSpec(const char * path);
 GUSIFileSpec(const FSRef & spec);
 GUSIFileSpec(const FSSpec & spec);
 GUSIFileSpec(const GUSIFileSpec & src);
 
 OSStatus		Error() const;
 const GUSICatInfo * CatInfo() const;
 bool				Exists() const;

 char *	FullPath() const;

 operator const FSRef *() const;
 operator const FSRef &() const;
 GUSIFileSpec &	operator+=(const char * name);
 GUSIFileSpec &	operator+=(const UInt8 * name);
protected:
 FSRef							fSpec;
 mutable GUSICatInfo			fInfo;
 mutable bool					fValidInfo;
 mutable OSStatus				fError;

 GUSIFileSpec &	AddPathComponent(const char * name, int length);
};

inline GUSIFileSpec &	GUSIFileSpec::operator+=(const UInt8 * name)
{
	return AddPathComponent((char *) name+1, name[0]);
}

inline GUSIFileSpec &	GUSIFileSpec::operator+=(const char * name)
{
	return AddPathComponent(name, strlen(name));
}

inline OSStatus GUSIFileSpec::Error() const
{
	return fError;
}

inline bool GUSIFileSpec::Exists() const
{
	return CatInfo() != nil;
}

inline GUSIFileSpec::operator const FSRef &() const
{
	return fSpec;
}

inline GUSIFileSpec::operator const FSRef *() const
{
	return &fSpec;
}

extern UInt8 *posix_to_utf8(const char *path);
extern char *utf8_to_posix(const UInt8 *utfpath);
