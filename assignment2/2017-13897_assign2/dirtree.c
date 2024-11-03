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


#define MAX_DIR 64            

/// @brief output control flags
#define F_TREE      0x1       ///< tree view
#define F_SUMMARY   0x2       ///< summary
#define F_VERBOSE   0x4       ///< verbose mode

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
  char entryType;                ///< type of the entry (e.g., file, directory, link)
  char *entryName;               ///< name of the entry
  char *absPath;                 ///< absolute path of the entry
  struct stat statusInfo;        ///< status information retrieved by lstat
  int lstatResult;               ///< result of the lstat operation
  int isDir;                     ///< flag indicating if the entry is a directory (1 for true, 0 for false)
};

//function prototypes
void panic(const char *msg);
struct dirent *getNext(DIR *dir);
static int dirent_compare(const void *a, const void *b);
void print_directory_error(const char *prefix);
struct metaData* allocate_entry(struct dirent *entryInfo, const char *entryName);
void update_entry_info(struct metaData *entryInfo);
void print_entry_details(struct metaData *entryInfo, struct summary *stats, const char *display_name, unsigned int flags);
char* buildChildPrefix(unsigned int flags, int is_last_entry, const char *pstr);
void processDir(const char *dn, const char *pstr, struct summary *stats, unsigned int flags);
void syntax(const char *argv0, const char *error, ...);
void print_summary_header(unsigned int flags);
void parse_arguments(int argc, char *argv[], const char *directories[], int *ndir, unsigned int *flags);
void print_grand_totals(struct summary total_stats, int ndir, unsigned int flags);
void print_summary_statistics(struct summary stats, unsigned int options);

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
  
    struct metaData *entry1 = *(struct metaData **)a;
    struct metaData *entry2 = *(struct metaData **)b;

    // Check if one entry is a directory and the other is not
    int isDir1 = entry1->isDir;
    int isDir2 = entry2->isDir;

    if (isDir1 != isDir2) {
        if (isDir1) {
            return -1; // entry1 is a directory, comes first
        } else {
            return 1; // entry2 is a directory, comes first
        }
    }

    // Both entries are of the same type, sort by entryName
    return strcmp(entry1->entryName, entry2->entryName);
}

/// @brief Outputs an error message when the directory cannot be accessed
/// @param prefix A string to display before the error message
void print_directory_error(const char *prefix) {
    char error_message[256];
    snprintf(error_message, sizeof(error_message), "%sERROR: %s\n", prefix, strerror(errno));
    fputs(error_message, stderr);
}

/// @brief Allocate a new metaData entry
/// @param entryInfo Pointer to the directory entry
/// @param entryName Directory name
/// @return Pointer to the allocated metaData structure
struct metaData* allocate_entry(struct dirent *entryInfo, const char *entryName) {

    // Ensure that the directory entry and name are valid
    if (entryInfo == NULL || entryName == NULL) {
        fputs("Error: Invalid directory entry or directory name.\n", stderr);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for a new metaData entry
    struct metaData *entry_info = malloc(sizeof(struct metaData));
    if (!entry_info) {
        fputs("Error: Unable to allocate memory for metaData entry.\n", stderr);
        exit(EXIT_FAILURE);
    }

    // Check if entry name duplication is successful
    entry_info->entryName = strdup(entryInfo->d_name);
    if (!entry_info->entryName) {
        free(entry_info); // Clean up before exit
        fputs("Error: Unable to duplicate entry name.\n", stderr);
        exit(EXIT_FAILURE);
    }

    // Check if absolute path construction is successful
    if (asprintf(&entry_info->absPath, "%s/%s", entryName, entryInfo->d_name) == -1) {
        free(entry_info->entryName); // Clean up before exit
        free(entry_info);
        fputs("Error: Unable to create absolute path.\n", stderr);
        exit(EXIT_FAILURE);
    }

    return entry_info;
}

/// @brief Update entry metadata (type and isDir)
/// @param entryInfo Pointer to the metaData entry
void update_entry_info(struct metaData *entryInfo) {

    entryInfo->lstatResult = lstat(entryInfo->absPath, &entryInfo->statusInfo);

    // Initialize entryType and isDir
    entryInfo->entryType = ' ';
    entryInfo->isDir = 0;

    if (entryInfo->lstatResult == -1) {
        return;
    }

    switch (entryInfo->statusInfo.st_mode & S_IFMT) { // Use the file type mask
        case S_IFREG:  // Regular file
            entryInfo->entryType = ' ';
            entryInfo->isDir = 0;
            break;
        case S_IFDIR:  // Directory
            entryInfo->entryType = 'd';
            entryInfo->isDir = 1;
            break;
        case S_IFLNK:  // Symbolic link
            entryInfo->entryType = 'l';
            entryInfo->isDir = 0;
            break;
        case S_IFCHR:  // Character device
            entryInfo->entryType = 'c';
            entryInfo->isDir = 0;
            break;
        case S_IFBLK:  // Block device
            entryInfo->entryType = 'b';
            entryInfo->isDir = 0;
            break;
        case S_IFIFO:  // FIFO (named pipe)
            entryInfo->entryType = 'f';
            entryInfo->isDir = 0;
            break;
        case S_IFSOCK: // Socket
            entryInfo->entryType = 's';
            entryInfo->isDir = 0;
            break;
        default:       // Unknown type
            entryInfo->entryType = '?';
            entryInfo->isDir = 0;
            break;
    }
}

/// @brief Print entry details and update statistics
/// @param entryInfo Pointer to the metaData entry
/// @param stats Pointer to summary statistics
/// @param display_name The formatted display name
/// @param flags Output control flags
void print_entry_details(struct metaData *entryInfo, struct summary *stats, const char *display_name, unsigned int flags) {
    // Check for errors in lstat result
    if (entryInfo->lstatResult == -1) {
        printf("%-60s%s\n", display_name, strerror(errno)); // Print error message
    } else {
        mode_t file_type = entryInfo->statusInfo.st_mode & S_IFMT;

        // Use switch to increment statistics based on the entry type
        switch (file_type) {
            case S_IFREG:  // Regular file
                stats->files++;
                stats->size += entryInfo->statusInfo.st_size;
                stats->blocks += entryInfo->statusInfo.st_blocks;
                break;
            case S_IFDIR:  // Directory
                stats->dirs++;
                stats->size += entryInfo->statusInfo.st_size;
                stats->blocks += entryInfo->statusInfo.st_blocks;
                break;
            case S_IFLNK:  // Symbolic link
                stats->links++;
                stats->size += entryInfo->statusInfo.st_size;
                stats->blocks += entryInfo->statusInfo.st_blocks;
                break;
            case S_IFIFO:  // FIFO (pipe)
                stats->fifos++;
                stats->size += entryInfo->statusInfo.st_size;
                stats->blocks += entryInfo->statusInfo.st_blocks;
                break;
            case S_IFSOCK: // Socket
                stats->socks++;
                stats->size += entryInfo->statusInfo.st_size;
                stats->blocks += entryInfo->statusInfo.st_blocks;
                break;
            default: 
                break;
        }

        // Retrieve user and group names for detailed output
        struct passwd *user_info = getpwuid(entryInfo->statusInfo.st_uid);
        struct group *group_info = getgrgid(entryInfo->statusInfo.st_gid);

        const char *user_name;
        const char *group_name;

        if (user_info != NULL) {
            user_name = user_info->pw_name;
        } else {
            user_name = "Unknown"; 
        }

        if (group_info != NULL) {
            group_name = group_info->gr_name;
        } else {
            group_name = "Unknown"; 
        }

        // Print details based on verbosity flag
        if (flags & F_VERBOSE) {
            printf("%-54s  %8.8s:%-8.8s  %10llu  %8llu  %c\n",
                   display_name, user_name, group_name,
                   (unsigned long long)entryInfo->statusInfo.st_size,
                   (unsigned long long)entryInfo->statusInfo.st_blocks,
                   entryInfo->entryType);
        } else {
            printf("%s\n", display_name); // Print just the display name
        }
    }
}

/// @brief Build prefix for  sub-entries based on flags
/// @param flags Output control flags
/// @param is_last_entry Indicates if this is the last entry
/// @param pstr Current prefix string
/// @return Newly created child prefix string
char* buildChildPrefix(unsigned int flags, int is_last_entry, const char *pstr) {
    char *child_pstr = NULL;
    if (flags & F_TREE) {
        if (is_last_entry) {
            asprintf(&child_pstr, "%s  ", pstr);
        } else {
            asprintf(&child_pstr, "%s| ", pstr);
        }
    } else {
        asprintf(&child_pstr, "%s  ", pstr);
    }
    return child_pstr;
}

/// @brief Recursively process directory @a dn and print its tree
///
/// @param dn Absolute or relative path string
/// @param pstr Prefix string printed in front of each entry
/// @param stats Pointer to statistics
/// @param flags Output control flags (F_*)
void processDir(const char *dn, const char *pstr, struct summary *stats, unsigned int flags) {
    
    DIR *presentDir = opendir(dn);
    if (!presentDir) {
        print_directory_error(pstr);
        return;
    }

    struct dirent *dirEntry;
    struct metaData **entries = NULL;
    size_t currCount = 0, currCapacity = 0;
    

    do {
        dirEntry = getNext(presentDir);
        
        // If end of directory reached, break loop
        if (dirEntry == NULL) {
            break;
        }

        // Check if the entries array needs to be resized
        if (currCount >= currCapacity) {

            size_t newCapacity;
            // Determine the new capacity
            if (currCapacity == 0) {
                newCapacity = 16; // initial 
            } else {
                newCapacity = currCapacity * 2; // increase allocated memory
            }

            // reallocate memory for entries

            size_t temp_size = newCapacity * sizeof(struct metaData *);
            struct metaData **temp_entries = realloc(entries, temp_size);

            if (temp_entries == NULL) {

                fprintf(stderr, "Memory allocation error.\n");
                closedir(presentDir); 

                size_t i = 0; 
                while (i < currCount) {

                    // Free the memory allocated for the entry name
                    if (entries[i]->entryName != NULL) {
                        free(entries[i]->entryName);
                        entries[i]->entryName = NULL;
                    }

                    // Free the memory allocated for the absolute path
                    if (entries[i]->absPath != NULL) {
                        free(entries[i]->absPath);
                        entries[i]->absPath = NULL;
                    }

                    free(entries[i++]); // Free the metaData structure
                }

                free(entries); // Free the entries array
                exit(EXIT_FAILURE); // Exit on failure
            }

            entries = temp_entries; 
            currCapacity = newCapacity; 
        }

        // Allocate a new entry for the current directory entry
        struct metaData *entryInfo = allocate_entry(dirEntry, dn);
        update_entry_info(entryInfo); 
        entries[currCount++] = entryInfo; 

    } while (1);


    // Sort the entries
    qsort(entries, currCount, sizeof(struct metaData *), dirent_compare);

    for (size_t i = 0; i < currCount; i++) {

      struct metaData *entryInfo = entries[i];
      int lastEntry = (currCount - i == 1);

        
      char nameBuffer[1024];
      char connector[3]; 
      size_t sizeBuf = sizeof(nameBuffer);

      switch (flags & (F_TREE | F_VERBOSE)) {

          case F_TREE: // Only tree view
              if (lastEntry) {
                  strcpy(connector, "`-"); 
              } else {
                  strcpy(connector, "|-"); 
              }
              snprintf(nameBuffer, sizeBuf, "%s%s%s", pstr, connector, entryInfo->entryName);
              break;

          case F_VERBOSE: // Verbose mode only

          case (F_TREE | F_VERBOSE): // Both options enabled
              if (lastEntry) {
                  strcpy(connector, "`-"); 
              } else {
                  strcpy(connector, "|-"); 
              }
              snprintf(nameBuffer, sizeBuf, "%s%s%s", pstr, connector, entryInfo->entryName);
              break;

          default: // No special flags
              snprintf(nameBuffer, sizeBuf, "%s%s", pstr, entryInfo->entryName);
              break;
      }

      char fileName[1024];
      size_t nameLength = strlen(nameBuffer);

      if (flags & F_VERBOSE) {
          if (nameLength > 54) {
              strncpy(fileName, nameBuffer, 51);
              fileName[51] = '\0';
              strcat(fileName, "...");
          } else {
              strncpy(fileName, nameBuffer, sizeof(fileName) - 1);
              fileName[sizeof(fileName) - 1] = '\0';
          }
      } else {
          // If not verbose, copy nameBuffer normally
          strncpy(fileName, nameBuffer, sizeof(fileName) - 1);
          fileName[sizeof(fileName) - 1] = '\0'; 
      }

      print_entry_details(entryInfo, stats, fileName, flags);

      char *child_pstr = buildChildPrefix(flags, lastEntry, pstr);

      if (entryInfo->isDir != 0) {
          processDir(entryInfo->absPath, child_pstr, stats, flags);
      }

      free(child_pstr);
  }

  // Free allocated resources and close the directory

  size_t index = 0;
  while (index < currCount) {

    struct metaData *currentEntry = entries[index];

    if (currentEntry->entryName != NULL) {
        free(currentEntry->entryName); 
        currentEntry->entryName = NULL; 
    }

    
    if (currentEntry->absPath != NULL) {
        free(currentEntry->absPath); 
        currentEntry->absPath = NULL; 
    }

    free(currentEntry);
    index++; 
  }

  free(entries); 
  closedir(presentDir);
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