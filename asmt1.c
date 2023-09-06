/* Program to perform multi-digit integer arithmetic.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2023, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.
   All included code is (c) Copyright University of Melbourne, 2023

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [Liam Zefu Li, 1463868]
   Dated:     [31/08/2023]

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

/* All necessary #defines provided as part of the initial skeleton */

#define INTSIZE	500	/* max number of digits per integer value */
#define LINELEN	999	/* maximum length of any input line */
#define NVARS	26	/* number of different variables */

#define CH_A     'a'    /* character 'a', first variable name */

#define ERROR	(-1)	/* error return value from some functions */
#define PROMPT	"> "	/* the prompt string for interactive input */

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define MULT	'*'	/* the multiplication operator */
#define POWR	'^'	/* the power-of operator */
#define DIVS	'/'	/* the division operator */
#define ALLOPS  "?=+*^/"

#define CH_ZERO  '0'    /* character zero */

#define CH_COM   ','    /* character ',' */
#define PUT_COMMAS 3    /* interval between commas in output values */

#define INT_ZERO 0	/* integer 0 */
#define INT_ONE  1  /* integer 1 */
#define INT_TEN  10	/* integer 10 */

typedef struct {
	int digits[INTSIZE + 1]; /* an array of designed length */
	int length; /* number of digits*/
} longint_t;

/* Debugging area */
#define DEBUG 0
#if DEBUG
#define DUMP_DBL(x) printf("line %d: %s = %.5f\n", __LINE__, #x, x)
#define DUMP_INT(x) printf("line %d: %s = %d\n", __LINE__, #x, x)
#define DUMP_STR(x) printf("line %d: %s = %s\n", __LINE__, #x, x)
#else
#define DUMP_DBL(x)
#define DUMP_INT(x)
#define DUMP_STR(x)
#endif

/****************************************************************/

/* A "magic" additional function needing explicit declaration */
int fileno(FILE *);

/* Skeleton program function prototypes */

void print_prompt(void);
void print_tadaa();
void print_error(char *message);

int  read_line(char *line, int maxlen);
void process_line(longint_t vars[], char *line);
int  get_second_value(longint_t vars[], char *rhsarg,
	longint_t *second_value);
int  to_varnum(char ident);

void do_print(int varnum, longint_t *var);
void do_assign(longint_t *var1, longint_t *var2);
void do_plus(longint_t *var1, longint_t *var2);
void do_mult(longint_t *var1, longint_t *var2);
void do_powr(longint_t *var1, longint_t *var2);
void zero_vars(longint_t vars[]);
longint_t parse_str(char *rhs);

void carry_over(longint_t *var1, int position);
void get_significant_length(longint_t *var);
int overflow(longint_t *var);
void overflow_print_exit(void);
void initialise(longint_t *var);


/****************************************************************/

/* Main program controls all the action
*/
int
main(int argc, char *argv[]) {
	char line[LINELEN+1] = {0};
	longint_t vars[NVARS];

	zero_vars(vars);
	print_prompt();
	while (read_line(line, LINELEN)) {
		if (strlen(line) > 0) {
			/* non empty line, so process it */
			process_line(vars, line);
		}
		print_prompt();
	}

	print_tadaa();
	return 0;
}

/****************************************************************/

/* Prints the prompt indicating ready for input, but only if it
   can be confirmed that the input is coming from a terminal.
   Plus, output might be going to a file, that's why the prompt,
   if required, is written to stderr and not stdout
*/
void
print_prompt(void) {
	if (isatty(fileno(stdin))) {
		fprintf(stderr, "> ");
		fflush(stderr);
	}
}

void
print_tadaa() {
	/* all done, so pack up bat and ball and head home,
	   getting the exact final lines right is a bit tedious,
	   because input might be coming from a file and output
	   might be going to a file */
	if (isatty(fileno(stdin)) && isatty(fileno(stdout))) {
		printf("\n");
	}
	printf("ta daa!!!\n");
	if (isatty(fileno(stdin)) && !isatty(fileno(stdout))) {
		fprintf(stderr, "\n");
	}
}

void
print_error(char *message) {
	/* need to write an error message to the right place(s)
	*/
	if (isatty(fileno(stdin)) || isatty(fileno(stdout))) {
		fprintf(stderr, "%s\n", message);
		fflush(stderr);
	}
	if (!isatty(fileno(stdout))) {
		printf("%s\n", message);
	}
}

/****************************************************************/

/* Reads a line of input into the array passed as argument,
   returns false if there is no input available.
   All whitespace characters are removed on the way through.
*/
int
read_line(char *line, int maxlen) {
	int i=0, c;
	while (((c=getchar())!=EOF) && (c!='\n')) {
		if (i<maxlen && !isspace(c)) {
			line[i++] = c;
		}
	}
	line[i] = '\0';
	/* then, if the input is coming from a file or the output
	   is going to a file, it is helpful to echo the input line
	   and record what the command was */
	if (!isatty(fileno(stdin)) || !isatty(fileno(stdout))) {
		printf("%s%s\n", PROMPT, line);
	}
	return ((i>0) || (c!=EOF));
}

/****************************************************************/

/* Process a command by parsing the input line into parts
*/
void
process_line(longint_t vars[], char *line) {
	int varnum, optype, status;
	longint_t second_value;

	/* determine the LHS variable, it
	   must be first character in compacted line
	*/
	varnum = to_varnum(line[0]); // varnum (0 ~ 25)
	if (varnum==ERROR) {
		print_error("invalid LHS variable");
		return;
	}

	/* more testing for validity 
	*/
	if (strlen(line)<2) {
		print_error("no operator supplied");
		return;
	}

	/* determine the operation to be performed, it
	   must be second character of compacted line
	*/
	optype = line[1];
	if (strchr(ALLOPS, optype) == NULL) {
		print_error("unknown operator");
		return;
	}

	/* determine the RHS argument (if one is required),
	   it must start in the third character of compacted line
	*/
	if (optype != PRINT) {
		if (strlen(line)<3) {
			print_error("no RHS supplied");
			return;
		}
		status = get_second_value(vars, line+2, &second_value);
		if (status==ERROR) {
			print_error("RHS argument is invalid");
			return;
		}
	}

	/* finally, do the actual operation
	*/
	if (optype == PRINT) {
		do_print(varnum, vars+varnum);
	} else if (optype == ASSIGN) {
		do_assign(vars+varnum, &second_value);
	} else if (optype == PLUS) {
		do_plus(vars+varnum, &second_value);
	} else if (optype == MULT) {
		do_mult(vars+varnum, &second_value);
	} else if (optype == POWR) {
		do_powr(vars+varnum, &second_value);
	} else {
		print_error("operation not available yet");
		return;
	}
	return;
}

/****************************************************************/

/* Convert a character variable identifier to a variable number
*/
int
to_varnum(char ident) {
	int varnum;
	varnum = ident - CH_A;
	if (0<=varnum && varnum<NVARS) {
		return varnum;
	} else {
		return ERROR;
	}
}

/****************************************************************/

/* Process the input line to extract the RHS argument, which
   should start at the pointer that is passed
*/
int
get_second_value(longint_t vars[], char *rhsarg,
			longint_t *second_value) {
	char *p;
	int varnum2;
	if (isdigit(*rhsarg)) {
		/* first character is a digit, so RHS is a number
		   now check the rest of RHS for validity */
		for (p=rhsarg+1; *p; p++) {
			if (!isdigit(*p)) {
				/* nope, found an illegal character */
				return ERROR;
			}
		}
		/* nothing wrong, ok to convert */
		*second_value = parse_str(rhsarg);
		return !ERROR;
	} else {
		/* argument is not a number, so should be a variable */
		varnum2 = to_varnum(*rhsarg);
		if (varnum2==ERROR || strlen(rhsarg)!=1) {
			/* nope, not a variable either */
			return ERROR;
		}
		/* and finally, get that variable's value */
		do_assign(second_value, vars+varnum2);
		return !ERROR;
	}
	return ERROR;
}

/* Set the vars array to all zero values
*/
void
zero_vars(longint_t vars[]) {
	for (int i=0; i<NVARS; i++) {
		initialise(vars+i);
	}
}

/*****************************************************************
******************************************************************

Your answer to the assignment should start here, using your new
typedef defined at the top of the program. The next few functions
will require modifications because of the change of structure
used for a long_int, and then you'll need to start adding whole
new functions after you get these first ones working properly.
Try and make the new functions fit the style and naming pattern
of the existing ones, ok?

******************************************************************
*****************************************************************/

/* Parse a string and return a longint_t object in reverse order
*/
longint_t 
parse_str(char *rhs) {
	longint_t num;
	num.length = strlen(rhs);
	for (int i = 0; i < num.length; i++) {
		num.digits[i] = rhs[num.length - 1 - i] - CH_ZERO;
	}
	return num;
}

/****************************************************************/

/* Print out a longint value and put commas
*/
void
do_print(int varnum, longint_t *var) {
	printf("register %c: ", varnum+CH_A);
	get_significant_length(var);

	/* print out the digits while putting commas every three digits */
	for (int i = var->length - 1; i >= 0; i--) {
		printf("%d", var->digits[i]);
		if (i > 0 && i % PUT_COMMAS == 0) {
			printf("%c", CH_COM);
		}
	}
	printf("\n");
}

/****************************************************************/

/* Assign a all digits from the input var2 array to the var1 array
*/
void
do_assign(longint_t *var1, longint_t *var2) {
	initialise(var1);
	for (int i = 0; i < var2->length; i++) {
		var1->digits[i] = var2->digits[i];
	}
}

/****************************************************************/

/* Adds var1 and var2 using array additions and checks overflow
*/
void
do_plus(longint_t *var1, longint_t *var2) {
	for (int i = 0; i < var2->length; i++) {
		var1->digits[i] += var2->digits[i];
		/* value added, now do the carry over */
		if (var1->digits[i] >= INT_TEN) {
			carry_over(var1, i);
		}
	}
	/* check overflow */
	if (overflow(var1)) {
		overflow_print_exit();
	}
}

/*****************************************************************
******************************************************************

Put your new functions below this line. Make sure you add suitable
prototypes at the top of the program.

******************************************************************
*****************************************************************/

/* Multiply var1 and var2 using array multiplications and checks overflow
*/
void 
do_mult(longint_t *var1, longint_t *var2) {
	/* set up another longint_t array to hold results and initialise it */
	longint_t final_result;
	initialise(&final_result);

	/* get the significant length of both arrays */
	get_significant_length(var1);
	get_significant_length(var2);

	/* multiply every digit in var1 with a single digit in var2, and add it
	   to the result array */
	for (int i = 0; i < var2->length; i++) {
		for (int j = 0; j < var1->length; j++) {
			int position = i + j;
			int temp_result = var1->digits[j] * var2->digits[i];
			final_result.digits[position] += temp_result;
			/* multiplied value added, now do the carry over */
			while (final_result.digits[position] >= INT_TEN) {
				carry_over(&final_result, position);
				position++;
			}
		}
		/* check if overflow */
		if (overflow(&final_result)) {
			overflow_print_exit();
		}
	}
	/* copy the result back to var1 */
	do_assign(var1, &final_result);
}

/****************************************************************/

/* Carry out exponentiation via repeated multiplication 
*/
void
do_powr(longint_t *var1, longint_t *var2) {
	/* if the base is zero or one, do nothing and exit function */
	get_significant_length(var1);
	if (var1->length == 1 && var1->digits[0] <= INT_ONE) {
		return;
	}
	/* copy the original value of var1 into a another array */
	longint_t var1_copy;
	do_assign(&var1_copy, var1);

	/* get the number of power */
	int power = 0;
	for (int i = 0; i < var2->length; i++) {
		int temp = var2->digits[i];
		for (int j = 0; j < i; j++) {
			temp *= INT_TEN;
		}
		power += temp;
	}

	/* if integer power overflows, terminate the program */
	if (power < 0) {
		overflow_print_exit();
	}

	/* now do the multiplication power - 1 times */
	for (int i = 0; i < power - 1; i++) {
		do_mult(var1, &var1_copy);
	}
}

/****************************************************************/

/* Do the carry over in addition and multiplication
*/
void
carry_over(longint_t *var, int position) {
	int carry = var->digits[position] / INT_TEN;
	var->digits[position + 1] += carry;
	var->digits[position] %= INT_TEN;
}

/****************************************************************/

/* Initialise the input array to all zero 
*/
void
initialise(longint_t *var) {
	for (int i = 0; i <= INTSIZE; i++) {
		var->digits[i] = INT_ZERO;
	}
	var->length = INTSIZE;
}

/****************************************************************/

/* Get the length of the part of an array that only contains input digits
*/
void
get_significant_length(longint_t *var) {
    int sig_len = var->length;
    while (sig_len > 1 && var->digits[sig_len - 1] == INT_ZERO) {
        sig_len--;
    }
    var->length = sig_len;
}

/****************************************************************/

/* Check if longint_t obejct overflows
*/
int 
overflow(longint_t *var) {
	if (var->digits[INTSIZE] > INT_ZERO) {
		return 1;
	}  
	return 0;
}

/****************************************************************/

/* Print error message and exit the program when encounter an overflow 
*/
void
overflow_print_exit(void) {
	printf("Integer overflow, program terminated\n");
	exit(EXIT_FAILURE);
}

/* algorithms are fun */