/*
 * Quick demo of how to use libjwt using a RS256.
 * You must have created a private/public key pair like so:
 *
 *  openssl genpkey -algorithm RSA -out myprivatekey.pem -pkeyopt rsa_keygen_bits:2048
 *  openssl rsa -in myprivatekey.pem -pubout > mykey.pub
 *
 * @author gback, CS 3214, Spring 2018, updated Spring 2021
 */
#include <jwt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

unsigned char private_key[16384];
unsigned char public_key[16384];

static void 
die(const char *msg, int error)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(error));
    exit(EXIT_FAILURE);
}

static void 
read_key(unsigned char *key, const char *name)
{
    FILE *f = fopen(name, "r");
    if (f == NULL)
        perror("fopen"), exit(-1);
    size_t len = fread(key, 1, 8192, f);
    key[len] = '\0';
    fclose(f);
}

int
main()
{
    read_key(private_key, "myprivatekey.pem");
    read_key(public_key, "mykey.pub");

    int rc;
    jwt_t *mytoken;

    rc = jwt_new(&mytoken);
    if (rc)
        die("jwt_new", rc);

    rc = jwt_add_grant(mytoken, "sub", "user0");
    if (rc)
        die("jwt_add_grant sub", rc);

    time_t now = time(NULL);
    rc = jwt_add_grant_int(mytoken, "iat", now);
    if (rc)
        die("jwt_add_grant iat", rc);

    rc = jwt_add_grant_int(mytoken, "exp", now + 3600 * 24);
    if (rc)
        die("jwt_add_grant exp", rc);

    rc = jwt_set_alg(mytoken, JWT_ALG_RS256, 
            private_key, strlen((char *)private_key));
    if (rc)
        die("jwt_set_alg", rc);

    printf("dump:\n");
    rc = jwt_dump_fp(mytoken, stdout, 1);
    if (rc)
        die("jwt_dump_fp", rc);

    char *encoded = jwt_encode_str(mytoken);
    if (encoded == NULL)
        die("jwt_encode_str", ENOMEM);

    printf("encoded as %s\nTry entering this at jwt.io\n", encoded);

    jwt_t *ymtoken;
    rc = jwt_decode(&ymtoken, encoded, public_key, strlen((char *)public_key));
    if (rc)
        die("jwt_decode", rc);

    char *grants = jwt_get_grants_json(ymtoken, NULL); // NULL means all
    if (grants == NULL)
        die("jwt_get_grants_json", ENOMEM);

    printf("redecoded: %s\n", grants);
}
