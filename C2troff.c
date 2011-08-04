/* 
 * Coded by David Bruno Cortarello. Redistribute under the terms of the 
 * BSD-lite license. Bugs, suggests, nor projects: dcortarello@gmail.com.
 *
 * Program: C2troff
 * Version: 0.1
 *
 *
 * Copyright (c) 2011, David B. Cortarello
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice
 *     and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice
 *     and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *   * Neither the name of Kwort nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Fancy error macro */
#define pferror(f) { \
	fprintf(stderr, f " at %s:%d [%s]\n", __FILE__, __LINE__, strerror(errno)); \
	fflush(stderr); \
	return -1; \
}

/* Pretty long lines!!! */
#define MAX_LINE 65535
#define TABS_SIZE 4

const char *reserved_C[] = {
	"asm", "auto", "break", "case", "char", "const", "continue", "default", 
	"do", "double", "else", "entry", "enum", "extern", "float", "for", "goto", 
	"if", "int", "long", "register", "return", "short", "signed", "sizeof", 
	"static", "struct", "switch", "typedef", "union", "unsigned", "void", 
	"volatile", "while", "#define", "#else", "#endif", "#if", "#ifdef", 
	"#ifndef", "#include", "#undef", "#", "define", "endif", "ifdef", "ifndef", 
	"include", "undef", NULL
};

const char *reserved_SH[] = {
	"if", "fi", "then", "exit", "echo", "-ne", "-eq", "-gt", "-lt", "-ge", "-le" 
	"read", "do", "done", "for", "case", "esac", "break", "continue", "until", 
	"time", "select", "in", "function", "else", "elif", "[[", "]]", NULL
};

const char *delimiters_C =  " \t(){}.[]*&:;+-=\\\n";
const char *delimiters_SH = " \t(){}.[]*&:;+-=*|<>`\\\n";
const int max_reserved = 8;

typedef struct _mtok {
	char orig_token[10];
	char out_token[256];
	short drop;
} mtok;


int rtoken(char *str, const char *reserved[], const char *delimiters, mtok *dat)
{
	int i = 0, j = 0;

	for (i = 0; str[i] && i < max_reserved; i++) {
		for (j = 0; delimiters[j]; j++)
			if (str[i] == delimiters[j])
				goto gcopy;
	}

gcopy:
	if (i == 0)
		i = 1;
	memcpy(dat->orig_token, str, i);
	dat->orig_token[i] = '\0';
	strcpy(dat->out_token, dat->orig_token);
	dat->drop = i;
	if (i == 1) {
		if (*dat->out_token == '\\') {
			strcpy(dat->out_token, "\\e");
		}
		else if (*dat->out_token == '{')
			strcpy(dat->out_token, "\\fB{\\fP");
		else if (*dat->out_token == '}')
			strcpy(dat->out_token, "\\fB}\\fP");
	}

	for (i = 0; reserved[i]; i++)
		if (strlen(reserved[i]) == strlen(dat->orig_token) && 
			 !memcmp(dat->orig_token, reserved[i], strlen(dat->orig_token))) {
			sprintf(dat->out_token, "\\fB%s\\fP", dat->orig_token);
			break;
		}

	return dat->drop;
}


void doC(FILE *input, FILE *output)
{
	char line[MAX_LINE];
	int comment = 0, i = 0, tabs = 0, j = 0;
	mtok dat;

	while (fgets(line, MAX_LINE, input) != NULL) {

		/* Check for an empty line */
		if (*line == '\0' || *line == '\n') {
			fprintf(output, "\n.br\n");
			continue;
		}

		for (i = 0; i < strlen(line); i++) {

			/* Comment starting */
			if (line[i] == '/' && line[i+1] == '*') {
				fprintf(output, "\\fI/*");
				i += 1;
				comment = 1;
				continue;
			}

			/* We are inside a comment */
			if (comment) {
				if (line[i] == '*' && line[i+1] == '/') {
					fprintf(output, "*/\\fP");
					i += 1;
					comment = 0;
				}
				else {
					/* We can't miss tabs and carriage returns in comments */
					if (line[i] == '\t') {
						/* Count how many tabs we have */
						for (tabs = 1; line[i] && line[i+tabs] == '\t'; tabs++)
							;
						i += tabs - 1;
						fprintf(output, "\\h'|%dn'", TABS_SIZE * tabs);
					}
					else if (line[i] == '\n')
						fprintf(output, "\n.br\n");
					else
						fprintf(output, "%c", line[i]);
				}
				continue;
			}

			/* Check for tab */
			if (line[i] == '\t') {
				/* Count how many tabs we have */
				for (tabs = 1; line[i] && line[i+tabs] == '\t'; tabs++)
					;
				i += tabs - 1;
				fprintf(output, "\\h'|%dn'", TABS_SIZE * tabs);
			}

			/* Check for a pointer * */
			else if (line[i] == '*')
				fprintf(output, "\\fI*\\fP");

			/* Check for an inverted slash (\) */
			else if (line[i] == '\\')
				fprintf(output, "\\e");

			/* Check for an inverted slash (\) */
			else if (line[i] == '.')
				fprintf(output, "\\&.");

			/* Check for a opening braket ({) */
			else if (line[i] == '{')
				fprintf(output, "\\fB{\\fP");

			/* Check for a clossing braket (}) */
			else if (line[i] == '}')
				fprintf(output, "\\fB}\\fP");

			/* Check for a carriage return (\n) */
			else if (line[i] == '\n')
				fprintf(output, "\n.br\n");

			/* Otherwise, print the character taking care for reserved words */
			else {
				j = rtoken(line+i, reserved_C, delimiters_C, &dat);
				fprintf(output, "%s", dat.out_token);
				i += j-1;
			}
		}
	}
}


void doSH(FILE *input, FILE *output)
{
	char line[MAX_LINE];
	int comment = 0, i = 0, tabs = 0, j = 0;
	mtok dat;

	while (fgets(line, MAX_LINE, input) != NULL) {

		/* Check for an empty line */
		if (*line == '\0' || *line == '\n') {
			fprintf(output, "\n.br\n");
			continue;
		}

		for (i = 0; i < strlen(line); i++) {

			/* Comment, so print the rest of the line and jump to the next one */
			if (line[i] == '#' ) {
				fprintf(output, line+i);
            fprintf(output, ".br\n");
				break;
			}

			/* Check for tab */
			if (line[i] == '\t') {
				/* Count how many tabs we have */
				for (tabs = 1; line[i] && line[i+tabs] == '\t'; tabs++)
					;
				i += tabs - 1;
				fprintf(output, "\\h'|%dn'", TABS_SIZE * tabs);
			}

			/* Check for a pointer * */
			else if (line[i] == '*')
				fprintf(output, "\\fI*\\fP");

			/* Check for an inverted slash (\) */
			else if (line[i] == '\\')
				fprintf(output, "\\e");

			/* Check for an inverted slash (\) */
			else if (line[i] == '.')
				fprintf(output, "\\&.");

			/* Check for a opening braket ({) */
			else if (line[i] == '{')
				fprintf(output, "\\fB{\\fP");

			/* Check for a clossing braket (}) */
			else if (line[i] == '}')
				fprintf(output, "\\fB}\\fP");

			/* Check for a clossing braket (}) */
			else if (line[i] == '`')
				fprintf(output, "\\(oq");

			/* Check for a carriage return (\n) */
			else if (line[i] == '\n')
				fprintf(output, "\n.br\n");

			/* Otherwise, print the character taking care for reserved words */
			else {
				j = rtoken(line+i, reserved_SH, delimiters_SH, &dat);
				fprintf(output, "%s", dat.out_token);
				i += j-1;
			}
		}
	}
}

void help_exit() {
	fprintf(stderr, "Usage: C2troff <OPTIONS> [<INPUT FILE>] [<OUTPUT FILE>]\n"
						 "OPTIONS:\n"
						 "-l <LANGUAGE> - Supported languages: C and S (shell)\n"
						 "-h <y/n>      - Include ms macro head in the output\n"
						 "-m <y/n>      - Use monospace font\n\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	FILE *input = NULL, *output = NULL;
	char language = 'C', head = 'y';
	short int i = 0;

	if (argc < 5)
		help_exit();

	for (i = 1; i < 5; i++) {
		if (!strcmp(argv[i], "-l")) {
			language = argv[i+1][0];
			i += 2;
		}
		if (!strcmp(argv[i], "-h")) {
			head = argv[i+1][0];
			i += 2;
		}
	}

	switch (argc) {
		case 5:
			output = stdout;
			input = stdin;
			break;
		case 6:
			if (!(input = fopen(argv[5], "r")))
				pferror("fopen");
			output = stdout;
			break;
		case 7:
			if (!(input = fopen(argv[5], "r")))
				pferror("fopen");
			if (!(output = fopen(argv[6], "w")))
				pferror("fopen");
			break;
	}

	if (head == 'y') {
		fprintf(output, ".nr PS 12\n");
		fprintf(output, ".nr PO 1i\n");
		fprintf(output, ".nr LL 6.5i\n");
		fprintf(output, ".nr LH 2i\n");
	}

	switch (language) {
		case 'C':
			doC(input, output);
			break;
		case 'S':
			doSH(input, output);
			break;
	}

	fclose(input);
	fclose(output);

	return 0;
}

