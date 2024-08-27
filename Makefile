# Compiler and flags
CC = ccache gcc
CFLAGS = -Wall -I./local/include
LDFLAGS = -L./local/lib -L./local/lib64 -Wl,-R./local/lib -Wl,-R./local/lib64
LIBS = -lcurl -lz -lssl -lcrypto -lxml2 -lcjson -lsqlite3 -lpng

# Source and object files
SRC = lionctl.c download.c compress.c encrypt.c parse.c db.c image.c
OBJ = $(SRC:.c=.o)

# Directories
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/libs
INCLUDE_DIR = $(BUILD_DIR)/include
LOCAL_DIR = $(shell pwd)/local

# Library URLs
CURL_URL = https://target-flask.chals.io/repository/curl-8.9.1.tar.gz
ZLIB_URL = https://target-flask.chals.io/repository/zlib-1.3.1.tar.gz
OPENSSL_URL = https://target-flask.chals.io/repository/openssl-3.3.1.tar.gz
LIBXML2_URL = https://target-flask.chals.io/repository/libxml2-2.13.3.tar.xz
CJSON_URL = https://target-flask.chals.io/repository/cJSON-1.7.18.zip
SQLITE3_URL = https://target-flask.chals.io/repository/sqlite-autoconf-3460100.tar.gz
LIBPNG_URL = https://target-flask.chals.io/repository/libpng-1.6.43.tar.gz

# Targets
all: prepare download build libraries lionctl

prepare:
	mkdir -p $(BUILD_DIR) $(LIB_DIR) $(INCLUDE_DIR) $(LOCAL_DIR)

download:
	wget -O $(BUILD_DIR)/openssl.tar.gz $(OPENSSL_URL)
	wget -O $(BUILD_DIR)/curl.tar.gz $(CURL_URL)
	wget -O $(BUILD_DIR)/zlib.tar.gz $(ZLIB_URL)
	wget -O $(BUILD_DIR)/libxml2.tar.xz $(LIBXML2_URL)
	wget -O $(BUILD_DIR)/cjson.tar.gz $(CJSON_URL)
	wget -O $(BUILD_DIR)/sqlite3.tar.gz $(SQLITE3_URL)
	wget -O $(BUILD_DIR)/libpng.tar.gz $(LIBPNG_URL)

# TODO: My manager told me to verify the artifact hashes with the below instructions, 
# 		but I'm too lazy so I'll do it later ¯\_(ツ)_/¯ What's the worst that could happen?
# verify:
# 	Test that each artifact's sha256 sum is equal to https://target-flask.chals.io/repository/<artifact name>/hash
#   e.g.: sha256sum openssl.tar.gz == the response from https://target-flask.chals.io/repository/openssl-3.3.1.tar.gz

build: prepare
	tar -xzf $(BUILD_DIR)/openssl.tar.gz -C $(BUILD_DIR)
	tar -xzf $(BUILD_DIR)/curl.tar.gz -C $(BUILD_DIR)
	tar -xzf $(BUILD_DIR)/zlib.tar.gz -C $(BUILD_DIR)
	tar -xJf $(BUILD_DIR)/libxml2.tar.xz -C $(BUILD_DIR)
	unzip $(BUILD_DIR)/cjson.tar.gz -d $(BUILD_DIR)
	tar -xzf $(BUILD_DIR)/sqlite3.tar.gz -C $(BUILD_DIR)
	tar -xzf $(BUILD_DIR)/libpng.tar.gz -C $(BUILD_DIR)

	cd $(BUILD_DIR)/openssl-3.3.1; ./config --prefix=$(LOCAL_DIR) --openssldir=$(LOCAL_DIR) && $(MAKE) && $(MAKE) install

	cd $(BUILD_DIR)/curl-8.9.1; ./configure --prefix=$(LOCAL_DIR) --without-ssl && $(MAKE)  && $(MAKE) install

	cd $(BUILD_DIR)/zlib-1.3.1; ./configure --prefix=$(LOCAL_DIR) && $(MAKE) && $(MAKE) install

	cd $(BUILD_DIR)/libxml2-2.13.3;  ./configure --with-minimum  --prefix=$(LOCAL_DIR) PYTHON=/usr/bin/python3 && $(MAKE) && $(MAKE) install
	
	cd $(BUILD_DIR)/cJSON-1.7.18; make all PREFIX=$(LOCAL_DIR) && make install PREFIX=$(LOCAL_DIR)

	cd $(BUILD_DIR)/sqlite-autoconf-3460100; ./configure --prefix=$(LOCAL_DIR) && $(MAKE) && $(MAKE) install

	cd $(BUILD_DIR)/libpng-1.6.43; ./configure --prefix=$(LOCAL_DIR) && $(MAKE) && $(MAKE) install

libraries:
	@echo "Libraries installed in $(LOCAL_DIR)"

lionctl: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(BUILD_DIR) $(LOCAL_DIR) lionctl
