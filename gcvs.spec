# Note that this is NOT a relocatable package
%define ver		1.1b1
%define rel		1
%define serial		1
%define prefix		/usr/local
%define gcvsdocdir	%{prefix}/doc/gcvs-%{ver}

Summary:		gCvs is a GTK+ interface for CVS (Concurrent Version System)
Name:			%{name}
Version:		%{version}
Release:		%{release}
Release:		%{serial}
Copyright: 		GPL
Group: 			Development/Tools 
Source: 		gcvs-%{ver}.tar.gz
BuildRoot: 		/var/tmp/%[name]-%{vesion}
Obsoletes: 		gcvs

URL:			http://cvsgui.sourceforge.net
Autoreq:		0
Requires:		gtk+, tcl
Docdir: 		%{prefix}/doc

Distribution:		Any
Packer:			Karl-Heinz Bruenen <gcvs@bruenen-net.de>

%description
The gcvs package contains an interface for cvs written in C++ using
gtk+. It contains a version of cvs modified for communication
purposes with gcvs. gcvs is part of a bigger project named CvsGui
which provides several graphical clients on Mac and Windows as well.

Updates are available at http://www.wincvs.org or http://cvsgui.sourceforge.net/.

%decription i18n
Translations of gCvs UI elements in varios languages

%changelog
* Sun Dec 29 2002 Karl-Heinz Bruenen <gcvs@bruenen-net.de>
- update spec file (stolen from anjuta spec file)
- support for gnome menues

* Tue Jan 22 2002 Karl-Heinz Bruenen <gcvs@bruenen-net.de>
* Tue Jul 4  2000 Alexandre Parenteau <aubonbeurre@hotmail.com>
- first shot.
  
%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix --enable-final

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi


%install
rm -rf %{buildroot}

%makeinstall

mkdir -p %{buildroot}%{_datadir}/applications
desktop-file-install --vendor %{desktop_vendor} --delete-original \
  --dir %{buildroot}%{_datadir}/applications                      \
  --add-category X-Red-Hat-Base                                   \
  --add-category Application                                      \
  --add-category Development                                      \
  %{buildroot}%{_datadir}/gnome/apps/Development/%{name}.desktop

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)

%doc AUTHORS COPYING ChangeLog NEWS README README.SUN TODO
%{prefix}/bin/cvs
%{prefix}/bin/cvsbug
%{prefix}/bin/cvstree
%{prefix}/bin/gcvs
%{prefix}/bin/rcs2log
%{prefix}/info/cvs*
%{prefix}/lib/cvs/*/*
%{prefix}/man/man1/cvs.1*
%{prefix}/man/man5/cvs.5*
%{prefix}/man/man8/cvsbug.8*
%{prefix}/share/gcvs/*.tcl
%{prefix}/share/gcvs/*.txt
%{prefix}/share/gcvs/pixmaps/*.xpm
%{prefix}/share/gnome/apps/Development/gcvs.desktop
%config /etc/gcvs/*

%files i18n
%defattr(-, root, root)
%{prefix}/share/locale/*/LC_MESSAGES/gcvs.mo

