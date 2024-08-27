#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "download.h"
#include "compress.h"
#include "encrypt.h"
#include "parse.h"
#include "db.h"
#include "image.h"
#include "utils.h"

void show_help() {
    printf("Lucky Lion Control Tool\n");
    printf("Usage:\n");
    printf("  lionctl [command] [options]\n");
    printf("Commands:\n");
    printf("  download <url>         Download content from a URL\n");
    printf("  parsexml <data>        Parse XML data\n");
    printf("  parsejson <data>       Parse JSON data\n");
    printf("  compress <data>        Compress data using zlib\n");
    printf("  encrypt <data>         Encrypt data using OpenSSL\n");
    printf("  dbwrite <data>         Write data to SQLite database\n");
    printf("  generatepng <file>     Generate a PNG file\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }

    struct Buffer buffer = {0};
    char compressed[256];
    size_t compressed_len = sizeof(compressed);
    unsigned char encrypted[256];
    int encrypted_len;

    const char *command = argv[1];

    if (strcmp(command, "download") == 0 && argc == 3) {
        if (download_content(argv[2], &buffer) == 0) {
            printf("Downloaded content:\n%s\n", buffer.data);
        }
    } else if (strcmp(command, "parsexml") == 0 && argc == 3) {
        parse_xml(argv[2]);
    } else if (strcmp(command, "parsejson") == 0 && argc == 3) {
        parse_json(argv[2]);
    } else if (strcmp(command, "compress") == 0 && argc == 3) {
        compress_data(argv[2], compressed, &compressed_len);
        printf("Compressed data length: %zu bytes\n", compressed_len);
    } else if (strcmp(command, "encrypt") == 0 && argc == 3) {
        encrypt_data((unsigned char *)argv[2], (int)strlen(argv[2]), encrypted, &encrypted_len);
        printf("Encrypted data length: %d bytes\n", encrypted_len);
    } else if (strcmp(command, "dbwrite") == 0 && argc == 3) {
        write_to_database("casino.db", argv[2]);
        printf("Data written to database.\n");
    } else if (strcmp(command, "generatepng") == 0 && argc == 3) {
        generate_png(argv[2]);
        printf("Generated PNG image: %s\n", argv[2]);
    } else {
        show_help();
    }

    free(buffer.data);

    return 0;
}
