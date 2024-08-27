#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <zlib.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cjson/cJSON.h>
#include <sqlite3.h>
#include <png.h>

// Buffer structure for libcurl
struct Buffer {
    char *data;
    size_t size;
};

// Callback function for libcurl to write data to a buffer
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Buffer *mem = (struct Buffer *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Not enough memory\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

// Function to download content from a URL using libcurl
int download_content(const char *url, struct Buffer *buffer) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }

    curl_global_cleanup();
    return 0;
}

// Function to compress data using zlib
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

// Function to encrypt data using libssl
int encrypt_data(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, int *ciphertext_len) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len_tmp;

    unsigned char key[32];
    unsigned char iv[16];

    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len_tmp, plaintext, plaintext_len);
    *ciphertext_len = len_tmp;

    EVP_EncryptFinal_ex(ctx, ciphertext + len_tmp, &len_tmp);
    *ciphertext_len += len_tmp;

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

// Function to parse and print XML using libxml2
void parse_xml(const char *xml_data) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadMemory(xml_data, strlen(xml_data), "noname.xml", NULL, 0);
    if (doc == NULL) {
        printf("Failed to parse XML\n");
        return;
    }

    root_element = xmlDocGetRootElement(doc);

    printf("Root element: %s\n", root_element->name);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

// Function to parse and print JSON using cJSON
void parse_json(const char *json_data) {
    cJSON *json = cJSON_Parse(json_data);

    if (json == NULL) {
        printf("Failed to parse JSON\n");
        return;
    }

    cJSON *name = cJSON_GetObjectItem(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Name: %s\n", name->valuestring);
    }

    cJSON_Delete(json);
}

// Function to write data to an SQLite database
void write_to_database(const char *db_name, const char *data) {
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(db_name, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = sqlite3_mprintf("INSERT INTO Transactions(Data) VALUES('%q');", data);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    sqlite3_free(sql);
    sqlite3_close(db);
}

// Function to generate a simple PNG image using libpng
void generate_png(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) return;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return;

    png_infop info = png_create_info_struct(png);
    if (!info) return;

    if (setjmp(png_jmpbuf(png))) return;

    png_init_io(png, fp);

    png_set_IHDR(png, info, 256, 256, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep row = (png_bytep) malloc(3 * 256 * sizeof(png_byte));

    for(int y = 0; y < 256; y++) {
        for(int x = 0; x < 256; x++) {
            row[x*3] = x;
            row[x*3 + 1] = y;
            row[x*3 + 2] = 128;
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);

    fclose(fp);
    png_free_data(png, info, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png, &info);
    free(row);
}

void show_help() {
    printf("Lucky Lion Control CLI\n");
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
