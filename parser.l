%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define EXEC_LEN (PATH_MAX*2 + 30)

%}
%%
^.HEAD printf(".nr PS 12\n" ".nr PO 0.7i\n" ".nr LL 7i\n" ".nr LH 2i");
^.AUTOR printf(".AU\n");
^.TITLE printf(".TL\n");
^.COMPANY printf(".AI\n");
^.SECTION printf(".SH\n");
^.ITEM[ ]*.*$ printf(".IP \\[bu]\n%s\n.br", yytext+6);
^.INDENT printf(".RS");
^.FINDENT printf(".RE\n");
^.CODE[ ]*.*$ {
	int i = 0;
	char *lang = yytext+6, *filename = NULL, C2troffExec[EXEC_LEN];
	while (*lang == ' ') lang++;
	filename = lang+1;
	while (*filename == ' ') filename++;
	printf(".LP\n.LP\n.LP\n" ".fam C\n" ".ps 10\n" ".so %s.roff\n" ".fam P\n" ".LP\n.LP\n.LP\n", filename);
	sprintf(C2troffExec, "C2troff -h n -l %c %s %s.roff", *lang, filename, filename);
	system(C2troffExec);
}
\*\*.*\*\* {
	yytext[strlen(yytext+2)] = '\0';
	printf("\\fB%*s\\fP", strlen(yytext+2)-4, yytext+2);
}
\|\|.*\|\| {
	yytext[strlen(yytext+2)] = '\0';
	printf("\\fI%*s\\fP", strlen(yytext+2)-4, yytext+2);
}
\*\|\*\|.*\*\|\*\| {
	yytext[strlen(yytext+4)] = '\0';
	printf("\\f(BI%*s\\f(BI", strlen(yytext+4)-8, yytext+4);
}
\.$  printf(".\n.br");
^\n  printf(".LP\n");
\xe1 printf("\\('a");
\xe9 printf("\\('e");
\xed printf("\\('i");
\xf3 printf("\\('o");
\xfa printf("\\('u");
\xfc printf("\\(:u");
\xf1 printf("\\(~n");
\xbf printf("\\(r?");
\xa1 printf("\\(r!");
\$   printf("\\(Do");
\x80 printf("\\(eu");
=    printf("\\(eq");
`    printf("\\(oq");
%%
