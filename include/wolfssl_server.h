#ifndef _WOLFSSL_SERVER_H_
#define _WOLFSSL_SERVER_H_

/* C stuff */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/limits.h>

/* C network stuff */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* C wolfSSL stuff */
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>

/* C++ stuff */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

/* C++ jsoncpp */
#include "json/json.h"
/* C++ base64 */
#include "base64.h"


/*
 *Server will not allow transfers greater
 *than 2MB by default.
*/
#define MAX_DATA_SIZE 2000000

/*
 *class for encrypting and decrypting JSON database file
 */
class KLCrypto
{
public:
    static byte* getRandBytes(int nbytes);
    static std::string dbEncrypt(std::string ptxt, std::string pass);
    static std::string dbDecrypt(std::string ctxt, std::string pass);
    static std::string genpwd(int len);
    static std::string sha256sum(const std::string &str);
    static std::string hexify(unsigned char c);
    static std::string pbkdf2hash(const std::string &str, const byte *salt,
                                  int saltlen);
};


#endif
