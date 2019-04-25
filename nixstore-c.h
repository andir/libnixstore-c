#ifndef _NIXSTORE_C_H_
#define _NIXSTORE_C_H_

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void libnixstorec_init();
int libnixstorec_is_valid_path(const char* path);

typedef struct {
	char* path;
	char* deriver;
	char* narHash;
	char* references;
	time_t registrationTime;
	uint64_t narSize;
	/* uint64_t id; */
	int ultimate;
	char* signatures;
	char* ca;
} CPathInfo;

void libnixstorec_free_path_info(CPathInfo* path_info);
CPathInfo* libnixstorec_query_path_info(const char* path);

char* libnixstorec_query_path_from_hash_part(const char* hashPart);
char* libnixstorec_query_path_from_nar_hash(const char* narHash);

void libnixstorec_free(void* ptr);

#ifdef __cplusplus
};
#endif

#endif
