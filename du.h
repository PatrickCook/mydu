#ifndef DU_H
#define DU_H
#include <ftw.h>
/* Current supported flags:
 * -a write counts for all files
 * -b uses bytes for size displayed
 * -k uses kilobytes for size displayed
 * -m uses megabytes for size displayed
 * -h print sizes in human format (1K 234M 2G)
 * -0 end each output line with a 0 bytes instead of newline
 * -c produce a grand total size at end of program
 * -S follow symbolic links
 * -d depth of the search
 */
typedef struct DuInfo {
	bool displayAll; 
	bool fSymLnk;
	bool showTotal;	
	char mode;
	char lineEnd; 
	char *directory;
	double totalSize;
	int depth;
} DuInfo;

int PrintHuman(double blocks);
int PrintMode(double blocks);
int WalkDirectory(char *dirName);
int ShowInfo(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);
#endif
