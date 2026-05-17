#ifndef __OPENMP_COMPAT_H__
#define __OPENMP_COMPAT_H__

#ifdef _OPENMP
#include <omp.h>
#else
inline int omp_get_max_threads() {
	return 1;
}

inline void omp_set_num_threads(int) {
}
#endif

#endif
