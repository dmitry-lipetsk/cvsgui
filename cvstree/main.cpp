#include <stdio.h>
#include "getopt.h"
#include <errno.h>
using namespace std;

#if defined(_MSC_VER) && _MSC_VER < 0x514 && __GNUC__<3  // VC7  and gcc 3
#       include <iostream.h>
#	include <fstream.h>
#else
#       include <iostream>
#	include <fstream>
#endif
#include "common.h"

static void yyoutput(ostream & out)
{
	std::vector<CRcsFile>::const_iterator i;
	for(i = gRcsFiles.begin(); i != gRcsFiles.end(); ++i)
	{
		const CRcsFile & rcs = *i;
		rcs.print(out);
	}
}

static void yygraph(const CRcsFile & rcs)
{
	CRcsFile rcsFile = rcs;

	rcsFile.sort();

	const std::vector<CRevFile> & allrevs = rcsFile.AllRevs();
	std::vector<CRevFile>::const_iterator i;
	for(i = allrevs.begin(); i != allrevs.end(); ++i)
	{
		const CRevNumber & rev = i->RevNum();
		cout << '\t' << rev.c_str() << '\n';
	}

}

static void yygraph(void)
{
	std::vector<CRcsFile>::const_iterator i;
	for(i = gRcsFiles.begin(); i != gRcsFiles.end(); ++i)
	{
		const CRcsFile & rcs = *i;
		yygraph(rcs);
	}
}

static const char *const cmd_usage[] =
{
	"CVSTREE synopsis is: cvs [cvsoptions] log [logoptions] | cvstree [cvstree options]\n",
	"CVSTREE options are:\n",
	"        -v           Verbose the output similar to cvs log (default)\n",
	"        -f input     Input from a file\n",
	"        -o output    Output in a file\n",
	"        -g           Output a graph of the revisions\n",
#if qCvsDebug
	"        -1           Debug the lexer (flex output)\n",
	"        -2           Debug the parser (bison output)\n",
	"        -3           Debug both the parser and the lexer\n",
#endif
	"        --help,-h    Print this help\n",
	"        --version    Print the version number\n",
	NULL
};

static struct option long_options[] =
{
	{"help", 0, NULL, 'h'},
	{"version", 0, NULL, '*'},
	{0, 0, 0, 0}
};

static void usage (const char *const *cpp)
{
	for (; *cpp; cpp++)
		fprintf (stderr, *cpp);
	exit(1);
}

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	bool outGraph = false;
	string output;
	string input;

#if qCvsDebug
	yydebug = 0;
#endif
	yy_flex_debug = 0;
	
	while ((c = getopt_long
			(argc, argv, "+o:f:g123vh", long_options, &option_index))
		   != EOF)
	{
		switch (c)
		{
		case '1':
#if qCvsDebug
			yy_flex_debug = 1;
#endif
			break;
		case '2':
#if qCvsDebug
			yydebug = 1;
#endif
			break;
		case '3':
#if qCvsDebug
			yydebug = 1;
			yy_flex_debug = 1;
#endif
			break;
		case 'h':
			usage (cmd_usage);
			break;
		case 'g':
			outGraph = true;
			break;
		case 'v':
			outGraph = false;
			break;
		case 'o':
			output = optarg;
			break;
		case 'f':
			input = optarg;
			break;
		case '*':
			cerr << "Version of " << argv[0] << " is $Revision: 1.8 $\n";
			exit(1);
			break;
		case '?':
		default:
			usage (cmd_usage);
		}
	}

	argc -= optind;
	argv += optind;
	if (argc != 0)
		usage (cmd_usage);

	if(!input.empty())
	{
		yyin = fopen(input.c_str(), "r");
		if(yyin == 0L)
		{
			cerr << "Error while opening inputfile '" << input << "'\n";
			exit(errno);
		}
	}
	else
		yyin = stdin;
	yyreset();
	yyrestart(yyin);
	yyparse();

	if(outGraph)
	{
		yygraph();
	}
	else
	{
		if(!output.empty())
		  {
		    ofstream out( output.c_str() );
		    if(!out) {
		      cerr << "Error while opening outputfile '" << output << "'\n";
		      exit(errno);
		    }
		    yyoutput(out);
		  }
 		else
		  yyoutput(cout);
	}
	if(!input.empty())
		fclose(yyin);
}
