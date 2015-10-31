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
    byte dectest[256];
    byte *iv_authtag_ctxt;
    byte *b64;
    word32 b64len;
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

    /*debugging*/
    if (wc_AesGcmDecrypt(&enc, dectest, ctxt, ptxtlen+pad, iv, 16, authTag,
                         sizeof(authTag), authIn, 1) != 0)
        std::cout<<"Decrypt test #1 failed!"<<std::endl;


    iv_authtag_ctxt = (byte*)calloc(16+(ptxtlen+pad)+sizeof(authTag),sizeof(byte));

    memcpy(iv_authtag_ctxt,iv,16);
    memcpy(iv_authtag_ctxt+16,authTag,sizeof(authTag));
    memcpy(iv_authtag_ctxt+16+sizeof(authTag),ctxt,ptxtlen+pad);

    /*debugging2*/
    if (wc_AesGcmDecrypt(&enc, dectest, iv_authtag_ctxt+16+16, 
                         ptxtlen+pad, iv_authtag_ctxt, 16, iv_authtag_ctxt+16,
                         16, authIn, 1) != 0)
        std::cout<<"Decrypt test #2 failed!"<<std::endl;

    //memset(key,0,sizeof(key));
    //memset(plain,0,ptxtlen+pad);
    /*memset(iv,0,16);
      memset(authTag,0,16);*/

    //free(plain);
    //free(iv);
    //free(ctxt);

    /*b64len = 2*(16+sizeof(authTag)+ptxtlen+pad+1);
    b64 = (byte*)calloc( b64len, sizeof(byte) );
    int b64retcode;
    if (b64 != NULL)
    {
        if ( (b64retcode=Base64_Encode((const byte*)iv_authtag_ctxt,
                                       16+sizeof(authTag)+ptxtlen+pad+1,b64,&b64len)) != 0)
            std::cout<<"Could not base64 encode ciphertext"<<std::endl;
        if (b64retcode == BAD_FUNC_ARG)
            std::cout<<"BAD_FUNC_ARG"<<std::endl;
        if (b64retcode == BUFFER_E)
            std::cout<<"BUFFER_E"<<std::endl;
    }
    if (b64 == NULL)
        std::cout<<"Failed to malloc b64 buffer"<<std::endl;
    
    free(iv_authtag_ctxt);

    std::string ret(reinterpret_cast< const char* >(b64));
    std::cout<<ret<<std::endl;*/
    for (int i=0; i<(16+16+ptxtlen+pad); ++i)
        std::cout<<(int)iv_authtag_ctxt[i]<<",";
    std::cout<<std::endl;

    std::string ret = base64_encode(iv_authtag_ctxt,16+16+ptxtlen+pad);
    std::cout<<ret<<std::endl;

    return ret;
}

std::string KLCrypto::dbDecrypt2(std::string ctxt, std::string pass)
{
    Aes dec;
    byte *iv_authtag_ctxt;
    byte iv[16];
    byte authTag[16];
    byte *ctxtbytes;
    byte *ptxtbytes;
    byte authIn[] = "A";
    //TODO: Replace with hash of pass//
    byte key[] = "YELLOWSUBMARINE";
    int b64debug;
    word32 outlen;
    
    std::cout<<ctxt<<std::endl;

    //iv_authtag_ctxt = (byte*)calloc(ctxt.size(),sizeof(byte));
    /*if ( (b64debug=Base64_Decode((const byte*)ctxt.c_str(),ctxt.size(),iv_authtag_ctxt,&outlen)) )
    {
        if (b64debug == ASN_INPUT_E)
            std::cout<<"ASN_INPUT_E"<<std::endl;
        else if (b64debug == BAD_FUNC_ARG)
            std::cout<<"BAD_FUNC_ARG"<<std::endl;
        else
            std::cout<<"Unknown error"<<std::endl;
            }*/
    std::string b64dec = base64_decode(ctxt);
    ptxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    ctxtbytes = (byte*)calloc(ctxt.size(),sizeof(byte));
    iv_authtag_ctxt = (byte*)b64dec.c_str();

    outlen = b64dec.size();

    for (int i=0; i<(int)outlen; ++i)
        std::cout<<(int)iv_authtag_ctxt[i]<<",";
    std::cout<<std::endl;

    memcpy(iv,iv_authtag_ctxt,16);
    memcpy(authTag,iv_authtag_ctxt+16,16);
    memcpy(ctxtbytes,iv_authtag_ctxt+32,outlen-32);

    /*word32 b64len;
    byte *b64iv = (byte*)calloc(32,sizeof(byte));
    Base64_Encode(iv,16,b64iv,&b64len);
    std::string ivstr(reinterpret_cast< const char* >(b64iv));
    std::cout<<ivstr<<std::endl;*/

    if (wc_AesGcmSetKey(&dec, key, sizeof(key)) != 0)
        std::cout<<"Failed to set decryption key"<<std::endl;
    
    std::cout<<"ctxt.size() is "<<ctxt.size()<<std::endl;
    std::cout<<"outlen: "<<outlen<<std::endl;

    if (wc_AesGcmDecrypt(&dec, ptxtbytes, ctxtbytes, outlen-32, iv, sizeof(iv), 
                         authTag, sizeof(authTag), authIn, 1) != 0)
        std::cout<<"Decrypt failed!"<<std::endl;

    memset(key,0,sizeof(key));
    //free(iv_authtag_ctxt);

    std::string ret(reinterpret_cast< const char* >(ptxtbytes));
    std::cout<<ret<<std::endl;

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

