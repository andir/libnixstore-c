#include "nixstore-c.h"
#include "util.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nix/config.h>
#include <nix/globals.hh>
#include <nix/store-api.hh>
#include <nix/globals.hh>


#define FREEZ(P) { if (P != NULL) free(P); P = NULL; }

void* mallocz(size_t size) {
	void* p = malloc(size);
	if (p)
		memset(p, 0, size);
	return p;
}

using namespace nix;

static ref<Store> store() {
    static std::shared_ptr<Store> _store = nullptr;
    if (!_store) {
        try {
            loadConfFile();
            settings.lockCPU = false;
            _store = openStore();
        } catch (Error & e) {
            fprintf(stderr, "%s\n", e.what());
	    exit(1);
        }
    }
    return ref<Store>(_store);
}

void libnixstorec_init() {
	store();
}

int libnixstorec_is_valid_path(const char* path) {
	try {
	    auto ret = store()->isValidPath(path);
	    return ret;
	} catch (Error & e) {
            fprintf(stderr, "%s\n", e.what());
	    return 0;
	}
}

void libnixstorec_free_path_info(CPathInfo* path_info) {
	if (path_info == NULL) return;

	FREEZ(path_info->path);
	FREEZ(path_info->deriver);
	FREEZ(path_info->narHash);
	FREEZ(path_info->references);
	FREEZ(path_info->signatures);
	FREEZ(path_info->ca);
	FREEZ(path_info);
}

CPathInfo* libnixstorec_query_path_info(const char* path) {
	CPathInfo* path_info = NULL;
	try {
		auto p = store()->queryPathInfo(path);
		if (p->deriver == "") return NULL;



		path_info = (CPathInfo*)mallocz(sizeof(CPathInfo));
		if (path_info == NULL) {
			fprintf(stderr, "failed to allocate memory for the CPathInfo struct");
			return NULL;
		}

		path_info->registrationTime = p->registrationTime;
		path_info->narSize = p->narSize;
		path_info->ultimate = p->ultimate;

		path_info->path = strdup(p->path.c_str());
		if (path_info->path == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo path string");
			return NULL;
		}

		path_info->deriver = strdup(p->deriver.c_str());
		if (path_info->deriver == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo deriver string");
			return NULL;
		}

		path_info->narHash = strdup(p->narHash.to_string().c_str());
		if (path_info->narHash == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo narHash string");
			return NULL;
		}


		{
			std::string references = util::join(p->references, " ");
			path_info->references = strdup(references.c_str());
		}

		if (path_info->references == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo references string after joining the parts");
			return NULL;
		}


		{
			std::string signatures = util::join(p->sigs, " ");
			path_info->signatures = strdup(signatures.c_str());
		}

		if (path_info->signatures == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo references string after joining the parts");
			return NULL;
		}

		path_info->ca = strdup(p->ca.c_str());
		if (path_info->ca == NULL) {
			libnixstorec_free_path_info(path_info);
			fprintf(stderr, "failed to duplicate PathInfo ca string");
			return NULL;
		}

		return path_info;
	} catch (Error & e) {
		libnixstorec_free_path_info(path_info);
		fprintf(stderr, "%s\n", e.what());
		return NULL;
	}

	return NULL;
}

char* libnixstorec_query_path_from_hash_part(const char* hashPart) {
	try {
		auto p = store()->queryPathFromHashPart(hashPart);
		return strdup(p.c_str());
	} catch (Error & e) {
		fprintf(stderr, "%s\n", e.what());
		return NULL;
	}
}

char* libnixstorec_query_path_from_nar_hash(const char* narHash) {
	try {
		auto p = store()->queryPathFromNarHash(narHash);
		return strdup(p.c_str());
	} catch (Error & e) {
		fprintf(stderr, "%s\n", e.what());
		return NULL;
	}
}
