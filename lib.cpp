#include "nixstore-c.h"
#include "util.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <nix/config.h>
#include <nix/globals.hh>
#include <nix/store-api.hh>
#include <nix/value-to-json.hh>

#include <pthread.h>

using namespace nix;

#define FREEZ(P)            \
  {                         \
    if (P != NULL) free(P); \
    P = NULL;               \
  }

struct nixstorec_instance {
  std::shared_ptr<Store> store;
};

template <typename T>
T* mallocz() {
  const auto size = sizeof(T);
  void* p = malloc(size);
  if (p) memset(p, 0, size);
  return (T*)p;
}

inline int is_valid_instance(struct nixstorec_instance* p) {
  if (p == NULL || p->store == nullptr) return 0;
  return 1;
}

static pthread_mutex_t config_init_mutex;

__attribute__((constructor)) void nixstore_load(void) {
  pthread_mutex_init(&config_init_mutex, NULL);
}

__attribute__((destructor)) void nixstore_unload(void) {
  pthread_mutex_destroy(&config_init_mutex);
}

struct nixstorec_instance* nixstorec_new_instance() {
  static int config_loaded = 0;

  pthread_mutex_lock(&config_init_mutex);

  initGC();

  if (!config_loaded) {
    // FIXME: mutex
    try {
      loadConfFile();
      settings.lockCPU = false;
    } catch (Error& e) {
      fprintf(stderr, "%s\n", e.what());
      pthread_mutex_unlock(&config_init_mutex);
      return NULL;
    }
    config_loaded = 1;
  }

  pthread_mutex_unlock(&config_init_mutex);

  struct nixstorec_instance* p = mallocz<struct nixstorec_instance>();

  if (p == NULL) {
    return NULL;
  }

  try {
    p->store = openStore();
  } catch (Error& e) {
    FREEZ(p);
    return NULL;
  }

  if (p->store == nullptr) {
    FREEZ(p);
    return NULL;
  }

  return p;
}

void nixstorec_free_instance(struct nixstorec_instance* ptr) {
  if (ptr == NULL) return;

  ptr->store = nullptr;  // release the shared pointer
  FREEZ(ptr);
}

int nixstorec_is_valid_path(struct nixstorec_instance* instp,
                            const char* path) {
  if (!is_valid_instance(instp)) {
    fprintf(stderr, "Not a valid nixstorec instance.");
    return 0;
  }

  try {
    auto ret = instp->store->isValidPath(path);
    return ret;
  } catch (Error& e) {
    fprintf(stderr, "%s\n", e.what());
    return 0;
  }
}

void nixstorec_free_path_info(CPathInfo* path_info) {
  if (path_info == NULL) return;

  FREEZ(path_info->path);
  FREEZ(path_info->deriver);
  FREEZ(path_info->narHash);
  FREEZ(path_info->references);
  FREEZ(path_info->signatures);
  FREEZ(path_info->ca);
  FREEZ(path_info);
}

CPathInfo* nixstorec_query_path_info(struct nixstorec_instance* instp,
                                     const char* path) {
  CPathInfo* path_info = NULL;
  if (!is_valid_instance(instp)) {
    fprintf(stderr, "Not a valid nixstorec instance.");
    return NULL;
  }

  try {
    auto p = instp->store->queryPathInfo(path);
    if (p->deriver == "") return NULL;

    path_info = mallocz<CPathInfo>();
    if (path_info == NULL) {
      fprintf(stderr, "failed to allocate memory for the CPathInfo struct");
      return NULL;
    }

    path_info->registrationTime = p->registrationTime;
    path_info->narSize = p->narSize;
    path_info->ultimate = p->ultimate;

    path_info->path = strdup(p->path.c_str());
    if (path_info->path == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr, "failed to duplicate PathInfo path string");
      return NULL;
    }

    path_info->deriver = strdup(p->deriver.c_str());
    if (path_info->deriver == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr, "failed to duplicate PathInfo deriver string");
      return NULL;
    }

    path_info->narHash = strdup(p->narHash.to_string().c_str());
    if (path_info->narHash == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr, "failed to duplicate PathInfo narHash string");
      return NULL;
    }

    {
      std::string references = util::join(p->references, " ");
      path_info->references = strdup(references.c_str());
    }

    if (path_info->references == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr,
              "failed to duplicate PathInfo references string after joining "
              "the parts");
      return NULL;
    }

    {
      std::string signatures = util::join(p->sigs, " ");
      path_info->signatures = strdup(signatures.c_str());
    }

    if (path_info->signatures == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr,
              "failed to duplicate PathInfo references string after joining "
              "the parts");
      return NULL;
    }

    path_info->ca = strdup(p->ca.c_str());
    if (path_info->ca == NULL) {
      nixstorec_free_path_info(path_info);
      fprintf(stderr, "failed to duplicate PathInfo ca string");
      return NULL;
    }

    return path_info;
  } catch (Error& e) {
    nixstorec_free_path_info(path_info);
    fprintf(stderr, "%s\n", e.what());
    return NULL;
  }

  return NULL;
}

char* nixstorec_query_path_from_hash_part(struct nixstorec_instance* instp,
                                          const char* hashPart) {
  if (!is_valid_instance(instp)) {
    fprintf(stderr, "Not a valid nixstorec instance.");
    return NULL;
  }

  try {
    auto p = instp->store->queryPathFromHashPart(hashPart);
    return strdup(p.c_str());
  } catch (Error& e) {
    fprintf(stderr, "%s\n", e.what());
    return NULL;
  }
}

char* nixstorec_query_path_from_file_hash(struct nixstorec_instance* instp,
                                         const char* narHash) {
  if (!is_valid_instance(instp)) {
    fprintf(stderr, "Not a valid nixstorec instance.");
    return 0;
  }

  try {
    auto p = instp->store->queryPathFromFileHash(narHash);
    return strdup(p.c_str());
  } catch (Error& e) {
    fprintf(stderr, "%s\n", e.what());
    return NULL;
  }
}

void nixstorec_free(void* ptr) { FREEZ(ptr); }

void nixstorec_free_eval_result(EvalResult* r) {
	if (r == NULL) return;

	FREEZ(r->result);
	FREEZ(r->error);
	FREEZ(r);
}

EvalResult* nixstorec_eval_cstr(struct nixstorec_instance* instp,
		const char* expr) {
	if (!is_valid_instance(instp)) {
		fprintf(stderr, "Not a valid nixstorec instance.");
		return NULL;
	}

	if (expr == NULL) {
		fprintf(stderr, "Not a valid expression.");
		return NULL;
	}

	EvalResult* res = mallocz<EvalResult>();
	if (res == NULL) {
		fprintf(stderr, "Failed to alloc memory for EvalResult");
		return NULL;
	}

	PathSet context;
	// FIXME: support custom search Paths
	EvalState state({""}, ref<Store>(instp->store));

	auto v = state.allocValue(); // FIXME: Will this throw on OOM?
	if (v == NULL) {
		res->success = 0;
		res->result = NULL;
		return res;
	}

	util::OnScopeExit cleanupAlloc([&v]() {
		GC_free(v);
	});

	try {
		state.eval(state.parseExprFromString(std::string(expr), ""), *v);
	} catch (Error& e) {
		res->success = 0;
		res->result = NULL;
		res->error = strdup(e.what());
		return res;
	}

	std::stringstream ss;

	try {
		printValueAsJSON(state, true, *v, ss, context);
	} catch(Error& e) {
		res->success = 0;
		res->result = NULL;
		res->error = strdup(e.what());
		return res;
	}

	const std::string s = ss.str();
	res->result = strdup(s.c_str());
	res->success = 1;

	return res;
}

EvalResult* nixstorec_eval_file(struct nixstorec_instance* instp, const char* path) {
	if (!is_valid_instance(instp)) {
		fprintf(stderr, "Not a valid nixstorec instance.");
		return NULL;
	}

	if (path == NULL) {
		fprintf(stderr, "Not a valid path.");
		return NULL;
	}

	EvalResult* res = mallocz<EvalResult>();
	if (res == NULL) {
		fprintf(stderr, "Failed to alloc memory for EvalResult");
		return NULL;
	}

	PathSet context;
	// FIXME: support custom search Paths
	EvalState state({""}, ref<Store>(instp->store));

	auto v = state.allocValue(); // FIXME: Will this throw on OOM?

	try {
		state.eval(state.parseExprFromFile(Path(path)), *v);
	} catch (Error& e) {
		res->success = 0;
		res->result = NULL;
		res->error = strdup(e.what());
		return res;
	}

	std::stringstream ss;

	try {
		printValueAsJSON(state, true, *v, ss, context);
	} catch(Error& e) {
		res->success = 0;
		res->result = NULL;
		res->error = strdup(e.what());
		return res;
	}

	const std::string s = ss.str();
	res->result = strdup(s.c_str());
	res->success = 1;

	return res;
}

const char* nixstorec_eval_result_get_error(EvalResult* r) {
	if (r == NULL) return NULL;
	if (r->success) return NULL;
	return r->error;
}

const char* nixstorec_eval_result_get_result(EvalResult* r) {
	if (r == NULL) return NULL;
	if (!r->success) return NULL;
	return r->result;
}

int nixstorec_eval_result_get_success(EvalResult* r) {
	if (r == NULL) return -1;
	return r->success;
}
