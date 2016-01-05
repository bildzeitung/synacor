/* Synacor Machine Utility
 *
 */
#include <argp.h>
#include <stdio.h>
#include <stdbool.h>

#include "vm.h"

const char *argp_program_version = "main 0.1";
const char *argp_program_bug_address = "bildzeitung@gmail.com";

static char doc[] = "Synacor Machine Client";
static char args_doc[] = "image"; 
static struct argp_option options[] = {
    {"debug", 'd', 0, OPTION_ARG_OPTIONAL, "Output memory after each tick", 0},
    {0, 0, 0, 0, 0, 0}
};

struct arguments {
    FILE* image;
    FILE* script;
    bool debug;
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'd':
            arguments->debug = true;
            break;
        case ARGP_KEY_ARG:
            switch (state->arg_num) {
                case 0:
                    arguments->image = fopen(arg, "rb");
                    if (!arguments->image) {
                        perror("Could not open image");
                        return errno;
                    }
                    break;
                case 1:
                    arguments->script = fopen(arg, "rb");
                    if (!arguments->script) {
                        perror("Could not open script file");
                        return errno;
                    }
                    break;
                default:
                    argp_usage(state);
                    break;
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char** argv) {
    struct arguments arguments;

    // defaults
    arguments.debug = false;
    arguments.script = NULL;

    if (argp_parse(&argp, argc, argv, 0, 0, &arguments)) return 1;

    if (init_vm(arguments.debug)) {
        printf("Could not initialize VM\n");
        return 1;
    }

    if (load_memory(arguments.image)) {
        return 1;
    }

    fclose(arguments.image);

    return start_vm(arguments.script);
}
