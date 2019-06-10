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
int nixstorec_is_valid_path(struct nixstorec_instance*, const char* path);

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
char* nixstorec_query_path_from_file_hash(struct nixstorec_instance*,
                                         const char* narHash);

void nixstorec_free(void* ptr);

typedef struct {
	// 1 == success, <> 1 => error
	int success;

	// NULL if success, otherwise optionally contains exception message
	char* error;

	// NULL if error otherwise the response JSON as character array
	char* result;
} EvalResult;

// Frees the given EvalResult
void nixstorec_free_eval_result(EvalResult*);

// Returns the success field value of the given EvalResult
int nixstorec_eval_result_get_success(EvalResult*);

// Returns the error string of the given EvalResult
const char* nixstorec_eval_result_get_error(EvalResult*);

// Returns the response string of the given EvalResult
const char* nixstorec_eval_result_get_result(EvalResult*);

// Eval the given expression with the given store instance
// Returns a pointer to the result structure. The caller must free the
// structure when done with processing.
EvalResult* nixstorec_eval_cstr(struct nixstorec_instance*, const char* expr);

// Eval the expression in the given file.
// Returns a pointer to the result structure. The caller must free the
// structure when done with processing.
EvalResult* nixstorec_eval_file(struct nixstorec_instance*, const char* path);

#ifdef __cplusplus
};
#endif

#endif
