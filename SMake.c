/*

Zach Martin | 357-07 | PROJECT 2 | SMake.c | 4/19/2016 
   
*/
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
   
#include "SMake.h"
   
static Rule **curRule;

int main(int argc, char *argv[]) {
   FILE *makeFile = fopen("Smakefile", "r");
   Rule **rhead = calloc(sizeof(Rule *), MAX);
   curRule = rhead;
   Rule *temp;

   if (makeFile == NULL || argc > 2) {
      fprintf(stderr, "Cannot open/find Smakefile\n");
	  exit(EXIT_FAILURE);
   }

   CheckFile(makeFile);
   fclose(makeFile);
   
   if (argc == 2) {
      if ((temp = IsRule(rhead, argv[1])))
         Make(temp, rhead);
      else {
         fprintf(stderr, "%s is not a valid rule\n", argv[1]);
         exit(EXIT_FAILURE);
	  }
   }
   else if (*rhead)
      Make(*rhead, rhead);
   else {
      fprintf(stderr, "No rules found in Smakefile\n");
      exit(EXIT_FAILURE);
   }

   FreeAll(rhead);
   free(rhead);
   return 0;
}
   
/*********************************************** PARSING FILE ************************************************/
   
void CheckFile(FILE *file) {
   char *line = calloc(BUFF_SIZE, 1);
   char *temp;
   int c;
   int idx = 0;
   int num = 1;

   curRule--;		//Needed because newRule advances curRule pointer each call.
   while ((c = fgetc(file)) != EOF) {
      if (idx > BUFF_SIZE * num - 1) {
         num++;
         temp = calloc(BUFF_SIZE, num);
         strncpy(temp, line, BUFF_SIZE);
         free(line);
         line = temp;
      }

      if (c == '\n') { 
         if (strlen(line)) {
            if (*line == '\t') {
               if (*curRule)
                  AddAction(line);
               else {
                  fprintf(stderr, "Found action before first rule. Stop.\n");
                  exit(EXIT_FAILURE);
			   }
            }
            else 
               NewRule(line);
         }
         temp = calloc(BUFF_SIZE, num);
         free(line);
         line = temp;
         idx = 0;
      }
      else {
         *(line + idx) = c;
         idx++; 
      }
   }
   free(line);
}

void NewRule(char *line) {  
   char *target;
   char **curDep;
   char *newDep;

   target = strtok(line, ":");
   if (target) {
      curRule++;
      *curRule = calloc(1, sizeof(Rule));
	  (*curRule)->target = calloc(strlen(target) + 1, 1);
      (*curRule)->dependencies = calloc(MAX, sizeof(char *));
	  (*curRule)->actions = calloc(MAX, sizeof(char *));
	  (*curRule)->execute = 0;
      RemoveBlanks(target);
      strncpy((*curRule)->target, target, strlen(target));
	  curDep = (*curRule)->dependencies;
	  
      while ((target = strtok(NULL, "\t "))) {
         RemoveBlanks(target);
         ContainsColon(target);
         newDep = calloc(strlen(target) + 1, 1);
         strncpy(newDep, target, strlen(target));
         *curDep = newDep;
         curDep++; 
      } 
   }
   else {
      fprintf(stderr, "Missing target name\n");
      exit(EXIT_FAILURE);
   }
}

void AddAction(char *line) {
   char *action = strtok(line, "\t");
   char *cmd;
   char **tempAction = (*curRule)->actions;

   if (action) {
      while (*tempAction)
         tempAction++;
      cmd = calloc(strlen(action) + 1, 1);
      strncpy(cmd, action, strlen(action));
      *tempAction = cmd;
   }
}

/*********************************************** RUNNING PROGRAM ************************************************/

void Make(Rule *rule, Rule **rhead) {
   Rule *check = NULL;
   char **tempD = rule->dependencies;
   char *name;
   struct stat dStat, tStat;
   int file;

   if (*tempD == NULL)
      ExecuteRule(rule);
   else {
      while (*tempD) {
         name = *tempD;
         check = IsRule(rhead, name);
         file = stat(name, &dStat);
		 
         if (stat(rule->target, &tStat)) {
            if(!check && file) {
               fprintf(stderr, "No rule to make target %s\n", rule->target);
               exit(EXIT_FAILURE);
            }
         }
		 
         if (check) {
            Make(check, rhead);
            if (check->execute)
               ExecuteRule(rule); 
         }
         else if (file) {
            fprintf(stderr, "No rule to make target %s\n", rule->target);
            exit(EXIT_FAILURE);
		 }

         stat(rule->target, &tStat);
         if ((!file && (dStat.st_mtime > tStat.st_mtime)))
            ExecuteRule(rule);
         tempD++;
      }
   }
}

Rule *IsRule(Rule **rhead, char *target) {
   Rule **temp = rhead;

   while (*temp) {
      if (strcmp((*temp)->target, target) == 0) 
         return *temp;
      temp++;
   }
   return NULL;
}

void ExecuteRule(Rule *rule) {
   char **tempAction = rule->actions;

   while (*tempAction) {
      printf("%s\n", *tempAction);
      if (system(*tempAction)) {
         fprintf(stderr, "Cannot execute action '%s'\n", *tempAction);
         exit (EXIT_FAILURE);
	  }
      tempAction++;
   }
   rule->execute = 1;
}
   
void RemoveBlanks(char *str) { 
   char *temp = str; 

   while ((*str = *temp++)) {             
      if ((isspace(*str) == 0))          
         str++;                
   }
}

void ContainsColon(char *str) {
   char *temp = str;
   int size = strlen(str);
   while (size--) {
      if ((*temp++ == ':')) {
         fprintf(stderr, "Found extra : in dependency\n");
         exit(EXIT_FAILURE);
	  }
   }
}  

void FreeAll(Rule **rhead) {
   Rule *tempR;
   char **tempD, **tempA;
   char *tempChar;
   
   while (*rhead) {
      tempR = *rhead;
      free(tempR->target);
      tempD = tempR->dependencies;
      tempA = tempR->actions;
	  
      while (*tempD) {
         tempChar = *tempD;
         free(tempChar);
         tempD++;
	  }
      free(tempR->dependencies);
	  
	  while (*tempA) {
         tempChar = *tempA;
         free(tempChar);
         tempA++;
	  }
      free(tempR->actions);

      free(tempR);
      rhead++;	
   }
}
  
