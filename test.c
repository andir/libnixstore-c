#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "nixstore-c.h"


int main(int argc, char* argv[]) {

	struct nixstorec_instance *instance = nixstorec_new_instance();

	if (argc > 1) {
		char* path = nixstorec_query_path_from_nar_hash(instance, argv[1]);
		if (path == NULL || path[0] == '\0') {
			fprintf(stderr, "not a valid hash?\n");
			return 0;
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
	nixstorec_free_instance(instance);
}
