#include "server.h"

SSL *add_tls_srv(int clisockfd, char *cert, char *priv)
{
  SSL_CTX *tlsctx;
  SSL *tls;
  char loaderr;
  loaderr = 0;
  tlsctx = SSL_CTX_new(TLSv1_2_server_method());
  SSL_CTX_set_options(tlsctx,SSL_OP_SINGLE_DH_USE);
  /* try to use user-specified cert file */
  if (SSL_CTX_use_certificate_file(tlsctx,cert,SSL_FILETYPE_PEM) <= 0)
    {
      /* set fail flag */
      loaderr = 1;
      printf("[*] Could not load specified certificate file.. using default\n");
      /* try to load default cert file */
      if (SSL_CTX_use_certificate_file(tlsctx,PROXYCERT,SSL_FILETYPE_PEM) <= 0)
	panic("Could not load default certificate file.",10);
    }
  /* if we could not use user-specified cert file */
  if (loaderr)
    {
      /* try to load default private key */
      if (SSL_CTX_use_PrivateKey_file(tlsctx,PROXYPRIV,SSL_FILETYPE_PEM) <= 0)
	panic("Could not load default key file.",10);
    }
  /* if we were able to load user-specified cert file
   * try to use user-specified private key */
  else if (SSL_CTX_use_PrivateKey_file(tlsctx,priv,SSL_FILETYPE_PEM) <= 0)
    {
      panic("Could not load specified key file.",10);
    }

  tls = SSL_new(tlsctx);
  SSL_set_fd(tls,clisockfd);
  SSL_set_accept_state(tls);

  return tls;
}
