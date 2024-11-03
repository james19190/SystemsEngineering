//--------------------------------------------------------------------------------------------------
// System Programming                         I/O Lab                                     Fall 2024
//
// @file
// @brief recursively traverse directory tree and list all entries
// @author James Kyoung Ho Kim
// @studid 2017-13897
//--------------------------------------------------------------------------------------------------

#define _GNU_SOURCE
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <grp.h>
#include <pwd.h>


#define MAX_DIR 64            ///< maximum number of supported directories

/// @brief output control flags
#define F_TREE      0x1       ///< enable tree view
#define F_SUMMARY   0x2       ///< enable summary
#define F_VERBOSE   0x4       ///< turn on verbose mode

/// @brief struct holding the summary
struct summary {
  unsigned int dirs;          ///< number of directories encountered
  unsigned int files;         ///< number of files
  unsigned int links;         ///< number of links
  unsigned int fifos;         ///< number of pipes
  unsigned int socks;         ///< number of sockets

  unsigned long long size;    ///< total size (in bytes)
  unsigned long long blocks;  ///< total number of blocks (512 byte blocks)
};

/// @brief file metadata stored in struct
struct metaData {
  char entryType;
  char *entryName;
  char *absPath;
  struct stat statusInfo;    
  int lstatResult;      
  int isDir;        
};

/// @brief abort the program with EXIT_FAILURE and an optional error message
///
/// @param msg optional error message or NULL
void panic(const char *msg)
{
  if (msg) fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

/// @brief read next directory entry from open directory 'dir'. Ignores '.' and '..' entries
///
/// @param dir open DIR* stream
/// @retval entry on success
/// @retval NULL on error or if there are no more entries
struct dirent *getNext(DIR *dir)
{
  struct dirent *next;
  int ignore;

  do {
    errno = 0;
    next = readdir(dir);
    if (errno != 0) perror(NULL);
    ignore = next && ((strcmp(next->d_name, ".") == 0) || (strcmp(next->d_name, "..") == 0));
  } while (next && ignore);

  return next;
}

/// @brief qsort comparator to sort directory entries. Sorted by name, directories first.
///
/// @param a pointer to first entry
/// @param b pointer to second entry
/// @retval -1 if a<b
/// @retval 0  if a==b
/// @retval 1  if a>b
static int dirent_compare(const void *a, const void *b) {
  
    struct dirent *e1 = (struct dirent *)a;
    struct dirent *e2 = (struct dirent *)b;

  // if one of the entries is a directory, it comes first
    if (e1->d_type != e2->d_type) {
        if (e1->d_type == DT_DIR) {
            return -1;
        } else {
            return 1;
        }
    }

    // If both entries are of the same type, compare their names
    return strcmp(e1->d_name, e2->d_name);
}

/// @brief recursively process directory @a dn and print its tree
///
/// @param dn absolute or relative path string
/// @param pstr prefix string printed in front of each entry
/// @param stats pointer to statistics
/// @param flags output control flags (F_*)
void processDir(const char *dn, const char *pstr, struct summary *stats, unsigned int flags)
{
  DIR *dirp = opendir(dn);
  if (!dirp) {
    // print error when directory does not exist or permission denied or any other error as standard output
    // NOTE: other errors do not need to be printed to standard output so I used fprintf for other errors
    printf("%sERROR: %s\n", pstr, strerror(errno));
    return;
  }

  struct dirent *dp;
  struct metaData **entries = NULL;
  size_t entry_count = 0;
  size_t entry_capacity = 0;

  while ((dp = getNext(dirp)) != NULL) {
    if (entry_count >= entry_capacity) {
      size_t new_capacity = entry_capacity == 0 ? 16 : entry_capacity * 2;
      struct metaData **new_entries = realloc(entries, new_capacity * sizeof(struct metaData *));
      // Handle OOM
      if (!new_entries) {
        fprintf(stderr, "Out of memory.\n");
        closedir(dirp);
        for (size_t i = 0; i < entry_count; i++) {
          free(entries[i]->entryName);
          free(entries[i]->absPath);
          free(entries[i]);
        }
        free(entries);
        exit(EXIT_FAILURE);
      }
      entries = new_entries;
      entry_capacity = new_capacity;
    }

    struct metaData *ei = malloc(sizeof(struct metaData));
    // Handle OOM
    if (!ei) {
      fprintf(stderr, "Out of memory.\n");
      closedir(dirp);
      for (size_t i = 0; i < entry_count; i++) {
        free(entries[i]->entryName);
        free(entries[i]->absPath);
        free(entries[i]);
      }
      free(entries);
      exit(EXIT_FAILURE);
    }
    ei->entryName = strdup(dp->d_name);
    // Handle OOM
    if (!ei->entryName) {
      fprintf(stderr, "Out of memory.\n");
      free(ei);
      closedir(dirp);
      exit(EXIT_FAILURE);
    }

    // Handle OOM
    if (asprintf(&ei->absPath, "%s/%s", dn, dp->d_name) == -1) {
      fprintf(stderr, "Out of memory.\n");
      free(ei->entryName);
      free(ei);
      closedir(dirp);
      exit(EXIT_FAILURE);
    }

    // Get stat info
    ei->lstatResult = lstat(ei->absPath, &ei->statusInfo);

    // Determine type character
    if (ei->lstatResult == -1) {
      ei->entryType = ' ';
      ei->isDir = 0;
    } else if (S_ISREG(ei->statusInfo.st_mode)) {
      ei->entryType = ' ';
      ei->isDir = 0;
    } else if (S_ISDIR(ei->statusInfo.st_mode)) {
      ei->entryType = 'd';
      ei->isDir = 1;
    } else if (S_ISLNK(ei->statusInfo.st_mode)) {
      ei->entryType = 'l';
      ei->isDir = 0;
    } else if (S_ISCHR(ei->statusInfo.st_mode)) {
      ei->entryType = 'c';
      ei->isDir = 0;
    } else if (S_ISBLK(ei->statusInfo.st_mode)) {
      ei->entryType = 'b';
      ei->isDir = 0;
    } else if (S_ISFIFO(ei->statusInfo.st_mode)) {
      ei->entryType = 'f';
      ei->isDir = 0;
    } else if (S_ISSOCK(ei->statusInfo.st_mode)) {
      ei->entryType = 's';
      ei->isDir = 0;
    } else {
      ei->entryType = '?';
      ei->isDir = 0;
    }

    entries[entry_count++] = ei;
  }

  // Sort the entries
  qsort(entries, entry_count, sizeof(struct metaData *), dirent_compare);

  for (size_t i = 0; i < entry_count; i++) {
    struct metaData *ei = entries[i];
    int is_last_entry = (i == entry_count - 1);

    char name_buf[1024];
    if (flags & F_TREE) {
      // -t option
      char connector[3];
      if (is_last_entry) {
        strcpy(connector, "`-");
      } else {
        strcpy(connector, "|-");
      }
      snprintf(name_buf, sizeof(name_buf), "%s%s%s", pstr, connector, ei->entryName);
    } else {
      // no -t option
      snprintf(name_buf, sizeof(name_buf), "%s%s", pstr, ei->entryName);
    }

    char display_name[1024];
    // Apply ... when '-v' option is present and name exceeds 54 characters
    if ((flags & F_VERBOSE) && (strlen(name_buf) > 54)) {
      strncpy(display_name, name_buf, 51);
      display_name[51] = '\0';
      strcat(display_name, "...");
    } else {
      strncpy(display_name, name_buf, sizeof(display_name) - 1);
      display_name[sizeof(display_name) - 1] = '\0';
    }

    if (ei->lstatResult == -1) {
      // Don't need to add to stats if lstat() failed
      char *error_msg = strerror(errno);
      printf("%-60s%s\n", display_name, error_msg);
    } else {
      if (S_ISREG(ei->statusInfo.st_mode)) {
        stats->files++;
        stats->size += ei->statusInfo.st_size;
        stats->blocks += ei->statusInfo.st_blocks;
      } else if (S_ISDIR(ei->statusInfo.st_mode)) {
        stats->dirs++;
        stats->size += ei->statusInfo.st_size;
        stats->blocks += ei->statusInfo.st_blocks;
      } else if (S_ISLNK(ei->statusInfo.st_mode)) {
        stats->links++;
        stats->size += ei->statusInfo.st_size;
        stats->blocks += ei->statusInfo.st_blocks;
      } else if (S_ISFIFO(ei->statusInfo.st_mode)) {
        stats->fifos++;
        stats->size += ei->statusInfo.st_size;
        stats->blocks += ei->statusInfo.st_blocks;
      } else if (S_ISSOCK(ei->statusInfo.st_mode)) {
        stats->socks++;
        stats->size += ei->statusInfo.st_size;
        stats->blocks += ei->statusInfo.st_blocks;
      }


      // Get user and group names
      struct passwd *pwd = getpwuid(ei->statusInfo.st_uid);
      const char *user_name = pwd ? pwd->pw_name : "";
      struct group *grp = getgrgid(ei->statusInfo.st_gid);
      const char *group_name = grp ? grp->gr_name : "";

      if (flags & F_VERBOSE) {
        // -v option
        printf("%-54s  %8.8s:%-8.8s  %10llu  %8llu  %c\n",
               display_name, user_name, group_name,
               (unsigned long long)ei->statusInfo.st_size,
               (unsigned long long)ei->statusInfo.st_blocks,
               ei->entryType);
      } else {
        // no -v option
        printf("%s\n", display_name);
      }
    }

    // Build prefix for child entries
    char *child_pstr = NULL;
    if (flags & F_TREE) {
      if (is_last_entry) {
        if (asprintf(&child_pstr, "%s  ", pstr) == -1) {
          fprintf(stderr, "Out of memory.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        if (asprintf(&child_pstr, "%s| ", pstr) == -1) {
          fprintf(stderr, "Out of memory.\n");
          exit(EXIT_FAILURE);
        }
      }
    } else {
      if (asprintf(&child_pstr, "%s  ", pstr) == -1) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
      }
    }

    // If it's a directory, recursively call processDir
    if (ei->isDir) {
      processDir(ei->absPath, child_pstr, stats, flags);
    }

    free(child_pstr);
  }

  // Clean up
  for (size_t i = 0; i < entry_count; i++) {
    free(entries[i]->entryName);
    free(entries[i]->absPath);
    free(entries[i]);
  }
  free(entries);
  closedir(dirp);
}

/// @brief print program syntax and an optional error message. Aborts the program with EXIT_FAILURE
///
/// @param argv0 command line argument 0 (executable)
/// @param error optional error (format) string (printf format) or NULL
/// @param ... parameter to the error format string
void syntax(const char *argv0, const char *error, ...)
{
  if (error) {
    va_list ap;

    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);

    printf("\n\n");
  }

  assert(argv0 != NULL);

  fprintf(stderr, "Usage %s [-t] [-s] [-v] [-h] [path...]\n"
                  "Gather information about directory trees. If no path is given, the current directory\n"
                  "is analyzed.\n"
                  "\n"
                  "Options:\n"
                  " -t        print the directory tree (default if no other option specified)\n"
                  " -s        print summary of directories (total number of files, total file size, etc)\n"
                  " -v        print detailed information for each file. Turns on tree view.\n"
                  " -h        print this help\n"
                  " path...   list of space-separated paths (max %d). Default is the current directory.\n",
                  basename((char*)argv0), MAX_DIR);

  exit(EXIT_FAILURE);
}

/// @brief Print function for -v option
void print_summary_header(unsigned int flags) {
    if (flags & F_SUMMARY) {
        if (flags & F_VERBOSE) {
            printf("%-54s  %8s:%-8s  %10s  %8s %s\n", "Name", "User", "Group", "Size", "Blocks", "Type");
            for (int i = 0; i < 100; i++) putchar('-');
            putchar('\n');
        } else {
            for (int i = 0; i < 100; i++) putchar('-');
            putchar('\n');
        }
    }
}

/// @brief Parse Arguments
void parse_arguments(int argc, char *argv[], const char *directories[], int *ndir, unsigned int *flags) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Handle flags
            if (strcmp(argv[i], "-t") == 0) {
                *flags |= F_TREE;
            } else if (strcmp(argv[i], "-s") == 0) {
                *flags |= F_SUMMARY;
            } else if (strcmp(argv[i], "-v") == 0) {
                *flags |= F_VERBOSE;
            } else if (strcmp(argv[i], "-h") == 0) {
                syntax(argv[0], NULL); // Show help
            } else {
                syntax(argv[0], "Unrecognized option '%s'.", argv[i]);
            }
        } else {
            // Recognized as a directory
            if (*ndir < MAX_DIR) {
                directories[(*ndir)++] = argv[i];
            } else {
                printf("Warning: Maximum number of directories exceeded, ignoring '%s'.\n", argv[i]);
            }
        }
    }
}

/// @brief Print function for -s option
void print_grand_totals(struct summary total_stats, int ndir, unsigned int flags) {
    if ((flags & F_SUMMARY) && (ndir > 1)) {
        printf("Analyzed %d directories:\n"
               "  Total # of files:        %16d\n"
               "  Total # of directories:  %16d\n"
               "  Total # of links:        %16d\n"
               "  Total # of pipes:        %16d\n"
               "  Total # of sockets:      %16d\n",
               ndir, total_stats.files, total_stats.dirs, total_stats.links, total_stats.fifos, total_stats.socks);

        if (flags & F_VERBOSE) {
            printf("  Total file size:         %16llu\n"
                   "  Total # of blocks:       %16llu\n",
                   total_stats.size, total_stats.blocks);
        }
    }
}


/// @brief Outputs the summary statistics for the -s option
void print_summary_statistics(struct summary stats, unsigned int options) {

    // separator line
    for (int i = 0; i < 100; i++) putchar('-');
    putchar('\n');

    char summary_output[256];
    unsigned int file_count = stats.files;
    unsigned int dir_count = stats.dirs;
    unsigned int link_count = stats.links;
    unsigned int fifo_count = stats.fifos;
    unsigned int socket_count = stats.socks;

    snprintf(summary_output, sizeof(summary_output), 
             "%u item%s, %u director%s, %u link%s, %u pipe%s, and %u socket%s",
             file_count, (file_count == 1) ? "" : "s",
             dir_count, (dir_count == 1) ? "y" : "ies",
             link_count, (link_count == 1) ? "" : "s",
             fifo_count, (fifo_count == 1) ? "" : "s",
             socket_count, (socket_count == 1) ? "" : "s");

    // Print the summary based on verbosity flag
    if (options & F_VERBOSE) {
        printf("%-68s   %14llu %9llu\n", summary_output, stats.size, stats.blocks);
    } else {
        printf("%-68s   \n", summary_output);
    }
    
    putchar('\n'); // Print a newline after the summary
}

// Program entry point
int main(int argc, char *argv[]) {
    //
    // default directory is the current directory (".")
    //
    const char CURRENT_DIRECTORY[] = "."; 
    const char *directories[MAX_DIR];
    int ndir = 0;

    struct summary tstat;
    unsigned int flags = 0;

    // Parse command line arguments
    parse_arguments(argc, argv, directories, &ndir, &flags);

    // If no directory was specified, use the current directory
    if (ndir == 0) {
        directories[ndir++] = CURRENT_DIRECTORY;
    }

    //
    // process each directory
    //
    // TODO
    //
    // Pseudo-code
    // - reset statistics (tstat)
    // - loop over all entries in 'directories' (number of entires stored in 'ndir')
    //   - reset statistics (dstat)
    //   - if F_SUMMARY flag set: print header
    //   - print directory name
    //   - call processDir() for the directory
    //   - if F_SUMMARY flag set: print summary & update statistics

    memset(&tstat, 0, sizeof(tstat));

    // Process each directory
    for (int i = 0; i < ndir; i++) {
        struct summary current_stats;
        memset(&current_stats, 0, sizeof(current_stats));

        print_summary_header(flags);

        printf("%s\n", directories[i]);

        const char *initial_prefix = (flags & F_TREE) ? "" : "  ";

        processDir(directories[i], initial_prefix, &current_stats, flags);

        if (flags & F_SUMMARY) {
            print_summary_statistics(current_stats, flags);
            tstat.files += current_stats.files;
            tstat.dirs += current_stats.dirs;
            tstat.links += current_stats.links;
            tstat.fifos += current_stats.fifos;
            tstat.socks += current_stats.socks;
            tstat.size += current_stats.size;
            tstat.blocks += current_stats.blocks;
        }
    }

    print_grand_totals(tstat, ndir, flags);

  //
  // that's all, folks!
  //

    return EXIT_SUCCESS;
}
