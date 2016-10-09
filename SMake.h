/* 

Zach Martin | 357-07 | PROJECT 2 | SMake.h | 4/19/2016

*/

#ifndef SMAKE_H
#define SMAKE_H

#define BUFF_SIZE 100
#define MAX 32

typedef struct Rule Rule;

struct Rule {
  char *target;
  char **dependencies;
  char **actions;
  int execute;
};

/*********** Parsing Smakefile ***********/
void CheckFile(FILE *file);

void NewRule(char *line);

void AddAction(char *line);

/*********** Running Smake ***********/

void Make(Rule *rule, Rule **rhead);

Rule *IsRule(Rule **rhead, char *target);

void ExecuteRule(Rule *rule);

void RemoveBlanks(char *str);

void ContainsColon(char *str);

void FreeAll(Rule **rhead);

#endif

/*
CheckFile
Parses through the given Smakefile character by character,
line buffer starts at 100, doubles if needed. 

NewRule
Once a new-line character is read and no tab is found at the beginning
a new rule is allocated, and the target and dependencies are 
allocated and added to the rule.

AddAction
Once a new-line character is read and a tab is found at the beginning
the entire line minus the tab is added as an action to the current rule.

Make
Checks dependencies to see if they are additional rules or a file,
and whether or not to execute the rule based on the guidelines in the README.

IsRule
Checks whether or not a rule exists with the given target name.

ExecuteRule
Echos and calls system() for each action of the given rule.

RemoveBlanks
Removes all blank characters from a given string.

ContainsColon
Checks a given string for any additional ':'. 
Only one ':' is permitted per new rule line.

FreeAll
Free's all remaning allocated memory.