#include "keylocker.h"
#include "network.h"

std::string KLCrypto::hexify(unsigned char c)
{
    std::stringstream ss;
    ss << std::hex << (int)c;

    return ss.str();    
}

/*std::string KLCrypto::getHexStr(const char *s, unsigned int slen)
{
    unsigned int i;
    std::string ret = "";

    for (i=0; i<slen; ++i)
        ret += hexify(s[i]);

    return ret;
    }*/

std::string KLCrypto::pbkdf2hash(const std::string &str, const byte *salt,
                                 int saltlen)
{
    int iter = 131073;
    byte hash[SHA256_DIGEST_SIZE];
    const byte *strbytes = (const byte*)str.c_str();

    if ( wc_PBKDF2(hash, strbytes, str.size(), salt,
                   saltlen, iter, sizeof(hash), SHA256) != 0 )
    {
        return "";
    }

    std::string ret = base64_encode(hash,sizeof(hash));

    return ret;
}

std::string KLCrypto::sha256sum(const std::string &str)
{
    byte shasum[SHA256_DIGEST_SIZE];
    const char *strc = str.c_str();
    std::string ret;
    
    wc_Sha256Hash((const byte*)strc, strlen(strc), shasum);

    for (int i=0; i<(int)sizeof(shasum); ++i)
        ret += hexify(shasum[i]);

    //ret = base64_encode(shasum,SHA256_DIGEST_SIZE);

    return ret;
}

byte* KLCrypto::getRandBytes(int nbytes)
{
    RNG rng;
    byte *randret;

    randret = (byte *)calloc(nbytes,sizeof(byte));

    if (wc_InitRng(&rng) != 0)
        return NULL;
    if (wc_RNG_GenerateBlock(&rng, randret, nbytes) != 0)
        return NULL;

    wc_FreeRng(&rng);

    return randret;
}

std::string KLCrypto::dbEncrypt(std::string ptxt, std::string pass)
{
    Aes enc;
    //TODO: replace with hash of pass//
    //byte key[] = "YELLOWSUBMARINE";
    byte *plain;
    byte *ctxt;
    byte *iv_authtag_ctxt;
    byte *iv = KLCrypto::getRandBytes(16);
    byte authTag[16];
    byte authIn[] = "A";
    const char *ptxtcs = ptxt.c_str();
    int ptxtlen = ptxt.size()+1;
    int pad = 0;
    /*password stuff*/
    byte key[16];
    int saltlen = 16;
    byte *salt = KLCrypto::getRandBytes(saltlen);
    const byte *passbytes = (const byte*)pass.c_str();
    int passlen = pass.size();
    int iter = 131072;
    int keylen = 16;

    if ( wc_PBKDF2(key, passbytes, passlen, salt,
                   saltlen, iter, keylen, SHA256) != 0)
        std::cout<<"Could not derive key from password"<<std::endl;

    if (iv == NULL)
        std::cout<<"Could not get random bytes for IV"<<std::endl;

    while ( ((ptxtlen + pad) % 16) != 0 )
        ++pad;

    plain = (byte *)calloc((ptxtlen+pad),sizeof(byte));
    memcpy(plain,ptxtcs,strlen(ptxtcs));

    ctxt = (byte *)calloc((ptxtlen+pad),sizeof(byte));

    if (wc_AesGcmSetKey(&enc, key, sizeof(key)) != 0)
    {
        std::cout<<"Could not set encryption key"<<std::endl;
        return std::string();
    }

    if (wc_AesGcmEncrypt(&enc, ctxt, plain, ptxtlen+pad, iv, 16, authTag,
                         sizeof(authTag), authIn, 1) != 0)
    {
        std::cout<<"Encrypt failed!"<<std::endl;
        return std::string();
    }

    iv_authtag_ctxt = (byte*)calloc(saltlen+16+sizeof(authTag)+ptxtlen+pad,sizeof(byte));
    memcpy(iv_authtag_ctxt,salt,saltlen);
    memcpy(iv_authtag_ctxt+saltlen,iv,16);
    memcpy(iv_authtag_ctxt+saltlen+16,authTag,sizeof(authTag));
    memcpy(iv_authtag_ctxt+saltlen+16+sizeof(authTag),
           ctxt,ptxtlen+pad);

    memset(key,0,sizeof(key));
    memset(plain,0,ptxtlen+pad);
    memset(iv,0,16);
    memset(authTag,0,16);

    free(plain);
    free(iv);
    free(salt);
    free(ctxt);

    std::string ret = base64_encode(iv_authtag_ctxt,saltlen+16+sizeof(authTag)+ptxtlen+pad);
    free(iv_authtag_ctxt);

    return ret;
}

std::string KLCrypto::dbDecrypt(std::string ctxt, std::string pass)
{
    Aes dec;
    const byte *iv_authtag_ctxt;
    byte iv[16];
    byte authTag[16];
    byte *ctxtbytes;
    byte *ptxtbytes;
    byte authIn[] = "A";
    //TODO: Replace with hash of pass//
    //byte key[] = "YELLOWSUBMARINE";
    word32 outlen;
    std::string b64dec = base64_decode(ctxt);
    /*password stuff*/
    byte key[16];
    int saltlen = 16;
    byte salt[16];
    const byte *passbytes = (const byte*)pass.c_str();
    int passlen = pass.size();
    int iter = 131072;
    int keylen = 16;

    ptxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    ctxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    iv_authtag_ctxt = (const byte*)b64dec.c_str();

    outlen = b64dec.size();

    memcpy(salt,iv_authtag_ctxt,saltlen);
    memcpy(iv,iv_authtag_ctxt+saltlen,16);
    memcpy(authTag,iv_authtag_ctxt+saltlen+16,16);
    memcpy(ctxtbytes,iv_authtag_ctxt+saltlen+16+16,outlen-48);

    if ( wc_PBKDF2(key, passbytes, passlen, salt,
                   saltlen, iter, keylen, SHA256) != 0)
        std::cout<<"Could not derive key from password"<<std::endl;

    if (wc_AesGcmSetKey(&dec, key, sizeof(key)) != 0)
    {
        std::cout<<"Failed to set decryption key"<<std::endl;
        return std::string();
    }

    if (wc_AesGcmDecrypt(&dec, ptxtbytes, ctxtbytes, outlen-48, iv, sizeof(iv),
                         authTag, sizeof(authTag), authIn, 1) != 0)
    {
        std::cout<<"Decrypt failed!"<<std::endl;
        return std::string();
    }

    std::string ret(reinterpret_cast< const char* >(ptxtbytes));

    memset(key,0,sizeof(key));
    memset(ptxtbytes,0,ret.size());
    free(ctxtbytes);
    free(ptxtbytes);

    return ret;
}

std::string KLCrypto::genpwd(int pwdlen)
{
    if ( pwdlen <= 0 || pwdlen > 1024 )
    {
        std::cout << "Invalid password length!" << std::endl;
        exit(123);
    }

    int bytesin;

    if (pwdlen % 4) // if passwordlength is not a multiple of 4
        bytesin = ((pwdlen * 3) / 4) + 1; // 3/4 the bytes needed as output are needed as input (+1)
    else
        bytesin = (pwdlen / 4) * 3; // 3/4 of the bytes needed as output are needed as input

    byte *rand = KLCrypto::getRandBytes(bytesin);
    std::string ret = base64_encode(rand, bytesin);
    memset(rand,0,bytesin);
    ret.resize(pwdlen);

    return ret;
}
