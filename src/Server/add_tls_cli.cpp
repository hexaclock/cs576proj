#include "server.h"

/*
 *given a socketfd, tries to create TLS ctx
 *returns a SSL connection on success
*/
SSL *add_tls_cli(int sockfd)
{
  /* SSL/TLS stuff */
  SSL_CTX *tls;
  SSL *ctls;
  tls = SSL_CTX_new(TLSv1_2_client_method());
  ctls = SSL_new(tls);
  SSL_set_fd(ctls,sockfd);
  /* end SSL/TLS stuff */
  return ctls;
  
}

