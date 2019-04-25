#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "nixstore-c.h"


int main(int argc, char* argv[]) {
	libnixstorec_init();

	if (argc > 1) {
		char* path = libnixstorec_query_path_from_nar_hash(argv[1]);
		if (path == NULL || path[0] == '\0') {
			fprintf(stderr, "not a valid hash?\n");
			return 0;
		}
		CPathInfo* path_info = libnixstorec_query_path_info(path);

		free(path);
		if (path_info != NULL) {
			fprintf(stderr, "narHash: %s\n", path_info->narHash);
			fprintf(stderr, "signatures: %s\n", path_info->signatures);
		}
		libnixstorec_free_path_info(path_info);
		libnixstorec_free_path_info(NULL);


	}
}
