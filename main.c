#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

const int kilobyte = 1024 * 1024;
const int blocksize = 1024;

void usage(const char* prgm) {
        printf(
                "\nUsage: %s [options]\n\n"
                "-- Options ----\n"
                "\t-h              -- Show this help\n"
                "\t-t [time]       -- Runtime in seconds (default: 10)\n"
                "\t-m [memory]     -- Memory to use in Megabytes (default: 0)\n"
                "\t-f [forks]      -- Forks to start (default: 0)\n"
                "\t-b [busy forks] -- Busy Forks to start (default: 0)\n\n"
                "-- Example ----\n"
                "%s -t 60 -m 1024 -f 30 -b 1\n"
                ,prgm, prgm);
}


int fork_loop(int busy) {
        while (1) {
                if (!busy) {
                        sleep(1);
                }
        }
        return 0;
}

void* allocate(int size) {
        void* r = malloc(kilobyte * size);
        memset(r, 0, kilobyte * size);
        return r;
}

void** array_allocate(int size) {
        int full_blocks = size / blocksize;
        int rest_bytes = size % blocksize;
        int rest = 0;
        if (rest_bytes) {
                rest = 1;
        }

        void** r = malloc((full_blocks + rest) * sizeof(void*));
        for (int i = 0; i < full_blocks; i++) {
                r[i] = allocate(blocksize);
        }

        if (rest) {
                r[full_blocks] = malloc(rest_bytes);
        }

        return r;
}

int main(int argc, char **argv) {
        unsigned int memory = 0;
        unsigned int forks = 0;
        unsigned int run_time = 10;
        unsigned int busy_forks = 0;
        int c = 0;

        while ((c = getopt (argc, argv, "ht:b:m:f:")) != -1) {
                switch (c) {
                case 'm':
                        memory = (unsigned int)(atoi(optarg));
                        break;
                case 'b':
                        busy_forks = (unsigned int)(abs(atoi(optarg)));
                        break;
                case 't':
                        run_time = (unsigned int)(atoi(optarg));
                        break;
                case 'f':
                        forks = (unsigned int)(abs(atoi(optarg)));
                        break;
                case 'h':
                        usage(*argv);
                        return 0;
                case '?':
                        fprintf (stderr, "Use -h for help.\n");
                        return 1;
                }
        }
        printf(
                "Memory     : %uMB\n"
                "Forks      : %u\n"
                "Busy Forks : %u\n"
                "Runtime    : %us\n"
                , memory, forks, busy_forks, run_time);

        unsigned int counter = forks + busy_forks;
        int pids[counter];
        for (size_t i = 0; i < counter; i++) {
                pids[i] = 0;
        }
        int break_loop = 0;
        for (size_t i = 0; i < counter; i++) {
                int pid = fork();
                switch (pid) {
                case -1:
                        printf("Failed to fork: %s\n", strerror(errno));
                        int count_forks = 0;
                        for (size_t i = 0; i < sizeof(pids) / sizeof(int); i++) {
                                if (pids[i] > 0) {
                                        count_forks++;
                                }
                        }
                        printf("Maximum of %d Forks reached\n", count_forks + 1);
                        break_loop = 1;
                        break;
                case 0:
                        if (i < busy_forks) {
                                fork_loop(1);
                        } else {
                                fork_loop(0);
                        }
                        return 0;
                default:
                        pids[i] = pid;
                }
                if (break_loop) {

                        break;
                }
        }

        void ** ref = array_allocate(memory);
        sleep(run_time);
        for (size_t i = 0; i <= sizeof(ref) / sizeof(void*); i++) {
                free(*(ref + i));
        }
        free(ref);

        for (size_t i = 0; i < counter; i++) {
                if (pids[i]) {
                        kill(pids[i], SIGTERM);
                }
        }
        wait(NULL);
}
