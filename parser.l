%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define EXEC_LEN (PATH_MAX*2 + 30)

int bl, it, blit;

%}
%%
^.HEAD {
	bl = 0; it = 0; blit = 0;
	printf(".nr PO 0.7i\n" ".nr LL 6i\n" ".nr LH 2i");
	/* printf(".nr FN T\n" ".nr PS 10\n" ".nr PO 0.7i\n" ".nr LL 7i\n" ".nr LH 2i"); */
}
^.AUTOR printf(".AU\n");
^.TITLE printf(".TL\n");
^.COMPANY printf(".AI\n");
^.SECTION printf(".SH\n");
^.ITEM[ ]* printf(".IP \\[bu]\n");
^.INDENT printf(".RS");
^.FINDENT printf(".RE\n");
^.CODE[ ]*.*$ {
	int i = 0;
	char *lang = yytext+6, *filename = NULL, C2troffExec[EXEC_LEN];
	while (*lang == ' ' || *lang == '\t') lang++;
	filename = lang+1;
	while (*filename == ' ' || *lang == '\t') filename++;
	printf(".LP\n.LP\n.LP\n" ".fam C\n" ".ps 10\n" ".so %s.roff\n" ".fam\n" ".ps\n", filename);
	sprintf(C2troffExec, "C2troff -h n -l %c %s %s.roff", *lang, filename, filename);
	system(C2troffExec);
}
\*\* {
	if (!bl) {
		printf("\\fB");
		bl = 1;
	}
	else {
		printf("\\fP");
		bl = 0;
	}
}
\|\| {
	if (!it) {
		printf("\\fI");
		it = 1;
	}
	else {
		printf("\\fP");
		it = 0;
	}
}
\*\|\*\| {
	if (!blit) {
		printf("\\f(BI");
		blit = 1;
	}
	else {
		printf("\\fP");
		blit = 0;
	}
}
\.$  printf(".\n.br");
\:$  printf(":\n.br");
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
