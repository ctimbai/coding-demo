#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "process.h"

static void usage();

int main(int argc, char *argv[])
{
    const struct option options[] = {
        {"cpu-usage", no_argument, NULL, 'c'},
        {"help", no_argument, NULL, 'h'},
        {"sort", no_argument, NULL, 's'},
        {"time", no_argument, NULL, 't'},
        {"interval", required_argument, NULL, 'i'},
        {"pid", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    const char *opt = "chi:p:st";
    int arg;
    unsigned type = 0;
    struct arg_param param;
    memset(&param, 0, sizeof(param));

    while((arg = getopt_long(argc, argv, opt, options, NULL)) != -1) {
        switch(arg) {
            case 'c':
                type |= e_cpu_usage;
                break;
            case 'h':
                usage();
                return 0;
            case 'i':
                param.interval = (int)strtol(optarg, NULL, 10);
                if(param.interval <= 0) {
                    fprintf(stderr, "Invalid argument %d for option -i\n", param.interval);
                    return 1;
                }
                break;
            case 'p':
                param.pid = (int)strtol(optarg, NULL, 10);
                if(param.pid <= 0) {
                    fprintf(stderr, "Invaild argument for option -p\n");
                    return 1;
                }
                break;
            case 's':
                param.sort = 1;
                break;
            case 't':
                type |= e_process_time;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    if(optind < argc) {
        strncpy(param.pname, argv[optind], sizeof(param.pname) - 1);
    }

    collect_process_info(&param, type);

    return 0;
}

static void usage()
{
    fprintf(stdout, "Usage lps [options] [process name]\n\n"
            "A tool to display runtime process information.\n\n"
            "  -c, --cpu-usage\tprint process cpu usage at <interval> time, <interval> can specified with option i, default one second.\n"
            "  -h, --help\t\tprint this help message.\n"
            "  -i, --interval\tspecified interval time when print cpu usage.\n"
            "  -p, --pid\t\tspecified process id, if this value is given, process name will be omit.\n"
            "  -s, --sort\t\tsort output by thread id.\n"
            "  -t, --time\t\tprint process and thread cpu time from process startup.\n");
}
