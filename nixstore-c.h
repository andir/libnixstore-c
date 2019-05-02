#ifndef _NIXSTORE_C_H_
#define _NIXSTORE_C_H_

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct nixstorec_instance;

struct nixstorec_instance* nixstorec_new_instance();
void nixstorec_free_instance(struct nixstorec_instance*);
int nixstorec_is_valid_path(const char* path);

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

void nixstorec_free_path_info(CPathInfo* path_info);
CPathInfo* nixstorec_query_path_info(struct nixstorec_instance*,
                                     const char* path);

char* nixstorec_query_path_from_hash_part(struct nixstorec_instance*,
                                          const char* hashPart);
char* nixstorec_query_path_from_nar_hash(struct nixstorec_instance*,
                                         const char* narHash);

void nixstorec_free(void* ptr);

#ifdef __cplusplus
};
#endif

#endif
