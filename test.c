#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nixstore-c.h"

int main(int argc, char* argv[]) {

	struct nixstorec_instance *instance = nixstorec_new_instance();

	if (argc > 1) {
		char* path = nixstorec_query_path_from_file_hash(instance, argv[1]);
		if (path == NULL || path[0] == '\0') {
			fprintf(stderr, "not a valid hash?\n");
			return 1;
		}

		if (!nixstorec_is_valid_path(instance, path)) {
			fprintf(stderr, "the path returned from nix-store isn't valid o.O\n");
		}

		CPathInfo* path_info = nixstorec_query_path_info(instance, path);

		free(path);
		if (path_info != NULL) {
			fprintf(stderr, "narHash: %s\n", path_info->narHash);
			fprintf(stderr, "signatures: %s\n", path_info->signatures);
		}

		nixstorec_free_path_info(path_info);
		nixstorec_free_path_info(NULL);
	}


	{
		const char* expr = "let f = x: { a = 10 + x; }; in f 100";
		EvalResult* r = nixstorec_eval_cstr(instance, expr);
		if (r == NULL) {
			fprintf(stderr, "Failed to eval, returned NULL");
			return 1;
		}

		if (nixstorec_eval_result_get_success(r)) {
			fprintf(stdout, "Result: %s\n", nixstorec_eval_result_get_result(r));
		} else {
			fprintf(stderr, "Eval Error: %s", nixstorec_eval_result_get_error(r));
		}

		nixstorec_free_eval_result(r);
		nixstorec_free_eval_result(NULL);
	}

	{
		const char* abspath= realpath("./test.nix", NULL);
		if (abspath == NULL) {
			perror("Failed to realpath");
		} else {
			EvalResult* r = nixstorec_eval_file(instance, abspath);
			free(abspath);
			if (r == NULL) {
				fprintf(stderr, "Failed to eval, returned NULL");
				return 1;
			}

			if (nixstorec_eval_result_get_success(r)) {
				fprintf(stdout, "Result: %s\n", nixstorec_eval_result_get_result(r));
			} else {
				fprintf(stderr, "Eval Error: %s", nixstorec_eval_result_get_error(r));
			}

			nixstorec_free_eval_result(r);
		}
	}

	nixstorec_free_instance(instance);
}
