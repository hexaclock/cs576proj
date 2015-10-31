#include "keylocker.h"
#include "network.h"

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

std::string KLCrypto::dbEncrypt2(std::string ptxt, std::string pass)
{
    Aes enc;
    //TODO: replace with hash of pass//
    byte key[] = "YELLOWSUBMARINE";
    byte *plain;
    byte *ctxt;
    byte *iv_authtag_ctxt;
    byte *iv = KLCrypto::getRandBytes(16);
    byte authTag[16];
    byte authIn[] = "A";
    const char *ptxtcs = ptxt.c_str();
    int ptxtlen = ptxt.size()+1;
    int pad = 0;
    
    if (iv == NULL)
        std::cout<<"Could not get random bytes for IV"<<std::endl;

    while ( ((ptxtlen + pad) % 16) != 0 )
        ++pad;

    plain = (byte *)calloc((ptxtlen+pad),sizeof(byte));
    memcpy(plain,ptxtcs,strlen(ptxtcs));

    ctxt = (byte *)calloc((ptxtlen+pad),sizeof(byte));

    if (wc_AesGcmSetKey(&enc, key, sizeof(key)) != 0)
        std::cout<<"Could not set encryption key"<<std::endl;
    
    if (wc_AesGcmEncrypt(&enc, ctxt, plain, ptxtlen+pad, iv, 16, authTag,
                         sizeof(authTag), authIn, 1) != 0)
        std::cout<<"Encrypt failed!"<<std::endl;

    iv_authtag_ctxt = (byte*)calloc(16+(ptxtlen+pad)+sizeof(authTag),sizeof(byte));

    memcpy(iv_authtag_ctxt,iv,16);
    memcpy(iv_authtag_ctxt+16,authTag,sizeof(authTag));
    memcpy(iv_authtag_ctxt+16+sizeof(authTag),ctxt,ptxtlen+pad);

    memset(key,0,sizeof(key));
    memset(plain,0,ptxtlen+pad);
    memset(iv,0,16);
    memset(authTag,0,16);

    free(plain);
    free(iv);
    free(ctxt);

    std::string ret = base64_encode(iv_authtag_ctxt,16+16+ptxtlen+pad);
    free(iv_authtag_ctxt);

    return ret;
}

std::string KLCrypto::dbDecrypt2(std::string ctxt, std::string pass)
{
    Aes dec;
    const byte *iv_authtag_ctxt;
    byte iv[16];
    byte authTag[16];
    byte *ctxtbytes;
    byte *ptxtbytes;
    byte authIn[] = "A";
    //TODO: Replace with hash of pass//
    byte key[] = "YELLOWSUBMARINE";
    word32 outlen;
    std::string b64dec = base64_decode(ctxt);
    
    ptxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    ctxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    iv_authtag_ctxt = (const byte*)b64dec.c_str();

    outlen = b64dec.size();

    memcpy(iv,iv_authtag_ctxt,16);
    memcpy(authTag,iv_authtag_ctxt+16,16);
    memcpy(ctxtbytes,iv_authtag_ctxt+32,outlen-32);

    if (wc_AesGcmSetKey(&dec, key, sizeof(key)) != 0)
        std::cout<<"Failed to set decryption key"<<std::endl;

    if (wc_AesGcmDecrypt(&dec, ptxtbytes, ctxtbytes, outlen-32, iv, sizeof(iv), 
                         authTag, sizeof(authTag), authIn, 1) != 0)
        std::cout<<"Decrypt failed!"<<std::endl;

    std::string ret(reinterpret_cast< const char* >(ptxtbytes));

    memset(key,0,sizeof(key));
    memset(ptxtbytes,0,ret.size());
    free(ctxtbytes);
    free(ptxtbytes);

    return ret;    
}

bool KLCrypto::dbEncrypt(std::string filepath, std::string pass)
{
  int retcode;
  std::string cmd = "/usr/bin/openssl aes-256-cbc -salt -pass pass:"
    +pass+" -in "+filepath+" -out "+filepath+".enc"+" >/dev/null 2>&1";

  //std::cout<<cmd<<std::endl;

  retcode = system(cmd.c_str());

  std::remove(filepath.c_str());

  if (retcode == 0)
    return true;
  else
    return false;
}

bool KLCrypto::dbDecrypt(std::string filepath, std::string pass)
{
  int retcode;
  std::string cmd = "/usr/bin/openssl aes-256-cbc -d -pass pass:"
    +pass+" -in "+filepath+".enc -out "+filepath+" >/dev/null 2>&1";

  //std::cout<<cmd<<std::endl;

  retcode = system(cmd.c_str());

  if (retcode == 0)
    return true;
  else
    {
      std::remove(filepath.c_str());
      return false;
    }
}

