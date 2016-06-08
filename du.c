/*
 * Author: Patrick Cook
 * CSC 357
 * du.c
 * May 24, 2016
 */
#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "du.h"

#define WIDTH 10
/* Not needed however they are more
 * efficient than using pow(10, 3)
 */
#define POW_10_3 1000
#define POW_10_6 1000000
#define POW_10_9 1000000000
#define MAX_WORD 300
#define DEF_DEPTH 20
#define BASE_10 10

/*
 * Although frowned upon this is the only way to use
 * NFTW which is a library that traverses trees. Atleast 
 * I made it static!
 */
static DuInfo *du;

int main(int argc, char **argv) {
   int opt, error = 0;
   extern char *optarg;
   /* Setup struct to parsing flags */
   du = (DuInfo *)calloc(sizeof(DuInfo), 1);
   du->depth = DEF_DEPTH;
   du->lineEnd = '\n';
   du->fSymLnk = false;
   du->directory = (char *)calloc(sizeof(char), MAX_WORD);
   /* Default directory */
   strcpy(du->directory, ".");

   /* Read in commandline arguments */
   while ((opt = getopt(argc, argv, "abkmh0Scp:d:")) != -1) {
      switch (opt) {
      case 'a': du->displayAll = true;
         break;
      case 'b': du->mode = 'b';
         break;
      case 'k': du->mode = 'k';
         break;
      case 'm': du->mode = 'm';
         break;
      case 'h': du->mode = 'h';
         break;
      case '0': du->lineEnd = '\0';
         break;
      case 'c': du->showTotal = true;
         break;
      case 'd':
         du->depth = strtol(optarg, NULL, BASE_10);
         break;
      case 'p':
         strcpy(du->directory, optarg);
         break;
      case 'S': du->fSymLnk = true;
         break;
      case ':': error++;
         break;
      case '?': error++;
         break;
      default:
         break;
      }
      /* Exit if error occured */
      if (error) {
         exit(EXIT_FAILURE);
      }
   }
   WalkDirectory(du->directory);
   /* Print total size */
   if (du->showTotal == true) {
      printf("Total: ");
      PrintHuman(du->totalSize);
      printf("\n");
   }
   /* free memory before exiting */
   free(du->directory);
   free(du);
   return 0;
}
/* Walks through directory recusivley
 * and sends info to ShowInfo which
 * prints info depending on flags set
 */
int WalkDirectory(char *dirName) {
   int nftw_flags = du->fSymLnk ? FTW_ACTIONRETVAL : FTW_PHYS | FTW_ACTIONRETVAL;
   /* Start file tree traversal with a function pointer */
   if (nftw(dirName, ShowInfo, 20, nftw_flags) == -1) {
      exit(EXIT_FAILURE);
   }
   return 0;
}
/*
 * I am using file tree traversal libraries and I used
 * a function definition that was found on the documentation
 * page. Source: http://linux.die.net/man/3/nftw
 */
int ShowInfo(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
   if (ftwbuf->level <= du->depth) {
      /* Add block size of fpath to total */
      du->totalSize += sb->st_size > 0 ? sb->st_size : 0;
      /* Print and handle flags */
      if (S_ISDIR(sb->st_mode)) {
         fprintf(stdout, "%-8.0d %-s%c", sb->st_blocks, fpath, du->lineEnd);
      }
      /* Print files and directories */
      else if (du->displayAll == true) {
         PrintMode(sb->st_size);
         fprintf(stdout, "%-s%c", fpath, du->lineEnd);
      }
   }
   else {
      return FTW_SKIP_SIBLINGS;
   }
   return 0;
}
/* Prints the file/dir size base on flags given */
int PrintMode(double blocks) {
   switch (du->mode) {
   case 'b': fprintf(stdout, "%-8.0f", blocks);
      break;
   case 'k': fprintf(stdout,"%-8.0f", blocks / POW_10_3);
      break;
   case 'm': fprintf(stdout,"%-8.0f", blocks / POW_10_6);
      break;
   case 'h': PrintHuman(blocks);
      break;
   default: fprintf(stdout, "%-8.0f", blocks);
      break;
   }
   return 0;
}
/* Prints the sizes in B, KB, MB, GB etc */
int PrintHuman(double blocks) {
   char buf[MAX_WORD];
   /* If blocks is less than 1KB */
   if (blocks < POW_10_3) {
      sprintf(buf, "%.1fB", blocks);
      fprintf(stdout, "%-8s", buf);
   }
   /* If blocks is less than 1MB */
   else if (blocks < POW_10_6) {
      sprintf(buf, "%.1fKB", blocks / POW_10_3);
      fprintf(stdout, "%-8s", buf);
   }
   /* If blocks is less than 1GB */
   else if (blocks < POW_10_9) {
      sprintf(buf, "%.1fMB", blocks / POW_10_6);
      fprintf(stdout, "%-8s", buf);
   }
   /* If blocks are over 1GB */
   else {
      sprintf(buf, "%.1fGB", blocks / POW_10_9);
      fprintf(stdout, "%-8s", buf);
   }
   return 0;
}


