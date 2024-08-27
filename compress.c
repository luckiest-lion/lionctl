#include <string.h>
#include <zlib.h>
#include "compress.h"

int compress_data(const char *input, char *output, size_t *output_len) {
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    defstream.avail_in = (uInt)strlen(input) + 1; // size of input
    defstream.next_in = (Bytef *)input;
    defstream.avail_out = (uInt)*output_len;      // size of output
    defstream.next_out = (Bytef *)output;

    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

    *output_len = defstream.total_out;
    return 0;
}
