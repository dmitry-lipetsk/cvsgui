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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * This is a unix tool which makes the same thing than the cvslog
 * mpw tool. The advantage is that it goes much faster
 * sometimes. You compile that with CPStr.cpp (and move the things
 * up a little bit in order to compile). Then you use it like it :
 *
 * cvs log -b -walexp -d '>=98-3-24' | cvslog -o "Sources:Temp:" -p "Sources:" > log.txt
 *
 * after in log.txt you should have something you can use in MPW...
 */

#include <stdio.h>
#include <getopt.h>
#include "CPStr.h"

// 1 if error, 0 if OK, -1 if eof
static int get_line(FILE * fd, char * line, int max_line)
{
	int i = 0;
	while(!feof(fd) && !ferror(fd) && i < max_line)
	{
		char c = (char)getc(fd);
		if(c != 0x0a && c != 0x0d)
			*line++ = c;
		else
			break;
		i++;
	}
	
	if(ferror(fd))
		return 1;

	if(i >= max_line)
		return 1;
	
	*line++ = '\0';
	
	if(feof(fd))
		return -1;
	
	return 0;
}

static void do_output(const char *path, CStr & rcsentry, CStr & rcsdate, CStr & rcsrev, CPStr & log_output)
{
	// we got one which matches the search...
	if(!log_output.empty())
	{
		if(!log_output.endsWith(':'))
			log_output << ':';
		
		// extract the file name :
		const char *filename = 0L, *runner = rcsentry;
		while((runner = strchr(runner, '/')) != 0L)
		{
			filename = ++runner;
		}
		if(filename == 0L)
			filename = rcsentry;
		CStr subpath;
		subpath = rcsentry;
		char *tmp = (char *)(const char *)subpath;
		tmp[rcsentry.length() - strlen(filename)] = '\0';
		
		while((tmp = strchr(tmp, '/')) != 0L)
		{
			*tmp++ = ':';
		}
		
		CStr outstr;
		
		fprintf(stdout, "# %s (rev. %s %s)\n", filename, (const char *)rcsrev,
			(const char *)rcsdate);
		
		/* cvs checkout -p -r rev 'rcsfile' > 'output:file' */
		outstr = "cvs checkout -p";
		outstr << " -r " << rcsrev; /* -r rev */
		outstr << " '" << rcsentry << '\''; /* 'rcsentry' */
		outstr << " > '" << log_output << filename << '\''; /* > 'output:file' */
		fprintf(stdout, "%s\n", (const char *)outstr);
		
		CStr fullPath;
		fullPath = path;
		if(!fullPath.endsWith(':'))
			fullPath << ':';
		fullPath << subpath;

		/* setfile -a "l" -m "." 'path:file' */
		fprintf(stdout, "setfile -a 'l' -m '.' '%s%s'\n",
			(const char *)fullPath, filename);

		/* comparefiles -16 'output:file' 'path:file' */
		fprintf(stdout, "comparefiles -16 '%s%s' '%s%s'\n",
			(const char *)log_output, filename,
			(const char *)fullPath, filename);
		
		/* directory 'path'; cvs update -j rev 'file' */
		fprintf(stdout, "directory '%s'; cvs update -j %s '%s' # optional\n",
			(const char *)fullPath, (const char *)rcsrev, filename);
	}
	else
	{
		fprintf(stdout, "%s # rev. %s %s\n", (const char *)rcsentry,
			(const char *)rcsrev, (const char *)rcsdate);
	}
}

static int do_cvs_log(FILE * output, const char *path, CPStr & log_output)
{
	static const char *rcs_line_head = "RCS file: ";
	static const char *rev_line_head = "revision ";
	static const char *date_line_head = "date: ";
	const char *CVSroot_directory = "/cvsroot/cvs/";

	char line[1024], *tmp;
	int rcs_line_head_size = strlen(rcs_line_head);
	int rev_line_head_size = strlen(rev_line_head);
	int date_line_head_size = strlen(date_line_head);
	int root_directory_size = strlen(CVSroot_directory);
	CStr rcsentry;
	CStr rcsrev;
	CStr rcsdate;
	
	while(get_line(output, line, 1023) == 0)
	{
retry:
		if(strncmp(line, rcs_line_head, rcs_line_head_size) == 0)
		{
			tmp = line + rcs_line_head_size;
			if(strncmp(tmp, CVSroot_directory, root_directory_size) == 0)
			{
				tmp += root_directory_size;
				if(tmp[0] == '/')
					tmp++;
				int len = strlen(tmp);
				if(len >= 3 && tmp[len - 2] == ',' && tmp[len - 1] == 'v')
				{
					tmp[len - 2] = '\0';
					rcsentry = tmp;
				}
			}
		}
		else if(!rcsentry.empty() && strncmp(line, rev_line_head, rev_line_head_size) == 0)
		{
			rcsrev = line + rev_line_head_size; // the rev number
		}
		else if(!rcsentry.empty() && !rcsrev.empty() && strncmp(line, date_line_head, date_line_head_size) == 0)
		{
			rcsdate = line;

			do_output(path, rcsentry, rcsdate, rcsrev, log_output);

			rcsentry = "";
			rcsdate = "";
			rcsrev = "";
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	int err = 0;
	CPStr log_path, log_output;
	
	// initialize
	log_output = "";
	log_path = "";
	
	optind = 0;
	while ((c = getopt (argc, (char *const*)argv, "+p:o:")) != -1)
	{
		switch (c)
		{
			case 'p':
				log_path = optarg;
				break;
			case 'o':
				log_output = optarg;
				break;
			case '?':
			default:
				break;
		}
	}

	int exitc = do_cvs_log(stdin, log_path, log_output);
	
	return exitc;
}
