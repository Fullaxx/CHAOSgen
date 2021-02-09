#ifndef __MY_GCRYPT_HELPERS__
#define __MY_GCRYPT_HELPERS__

#include <gcrypt.h>

void my_libgcrypt_init(char *vers);
void my_error(const char *what, gcry_error_t err, int exitcode);

#endif
