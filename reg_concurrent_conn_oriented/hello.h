/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _HELLO_H_RPCGEN
#define _HELLO_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


#define HELLO 0x20000001
#define HELLO_V1 1

#if defined(__STDC__) || defined(__cplusplus)
#define HELLO 1
extern  char ** hello_1(char **, CLIENT *);
extern  char ** hello_1_svc(char **, struct svc_req *);
extern int hello_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define HELLO 1
extern  char ** hello_1();
extern  char ** hello_1_svc();
extern int hello_1_freeresult ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_HELLO_H_RPCGEN */