/* functions to handle zlib-compressed data read from socket
*/
#include <zlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <winsock.h>
#endif


/* included functions */
void *prepare_sock_gz_r(FILE *sockr);
int z_getc(void *v);
char *z_gets(void *v, char *line, size_t len);
char *z_read_sock(void *v);
int close_sock_gz_r(void *v);



#define ZBSIZE 100000
typedef struct {
	z_stream stream;
	char z_buffer[ZBSIZE]; /* compressed input buffer */
	char text_buffer[4 * ZBSIZE]; /* decompressed buffer */
	char *pos, *endbuf;
#ifdef WIN32
	SOCKET fd;
#else
	int fd;
#endif
	} sock_gz_r;



void *prepare_sock_gz_r(FILE *sockr)
{
int err;
sock_gz_r *big;

big = (sock_gz_r *)malloc(sizeof(sock_gz_r));
if(big == NULL) return NULL;
big->stream.next_in = Z_NULL;
big->stream.avail_in = 0;
big->stream.avail_out = 0;
big->stream.zalloc = Z_NULL;
big->stream.zfree = Z_NULL;
big->stream.opaque = NULL;
big->pos = big->text_buffer;
big->endbuf = big->pos;
#ifdef WIN32
big->fd = (SOCKET)sockr;
#else
big->fd = fileno(sockr);
#endif
err = inflateInit(&big->stream);
return err == Z_OK ? (void *)big : NULL;
}


int z_getc(void *v)
{
int q;
sock_gz_r *big = (sock_gz_r *)v;
z_streamp zs;

if(big->pos < big->endbuf) {
	return *(big->pos++);
	}
zs = &(big->stream);
zs->next_out = (Bytef *)big->text_buffer;
zs->avail_out = sizeof(big->text_buffer);
big->pos = (char *)zs->next_out;
do	{
	if(zs->avail_in == 0) {
		int lu;
#ifdef WIN32
		lu = recv( big->fd , big->z_buffer, ZBSIZE, 0 );
#else
		lu = read( big->fd , big->z_buffer, ZBSIZE );
#endif
		if(lu == -1) return EOF;
		zs->next_in = (Bytef *)big->z_buffer;
		zs->avail_in = lu;
		}
	q = inflate(zs, Z_NO_FLUSH);	
	if(q == Z_STREAM_END) break;	
	if(q != Z_OK) {
		break;
		}
	}
while ( (char *)zs->next_out == big->pos);
big->endbuf = (char *)zs->next_out;
if(big->pos < big->endbuf) return *(big->pos++);
else 
	return EOF;
}


char *z_gets(void *v, char *line, size_t len)
{
int c;
char *p;

p = line;
while(len > 1) {
	c = z_getc( v );
	if(c == EOF) {
		if(p == line) return NULL;
		break;
		}
	*(p++) = c;
	if(c == '\n') break;
	len--;
	}
*p = 0;
return line;
}


char *z_read_sock(void *v)
{
static char line[500];
char *p;
int l;

p = z_gets(v, line, sizeof(line));
if(p == NULL) return NULL;
l = strlen(line);
if(l > 0 && line[l-1] == '\n') line[l-1] = 0;
return line;
}


int close_sock_gz_r(void *v)
{
sock_gz_r *big = (sock_gz_r *)v;
int val;

val = inflateEnd(&(big->stream));
free(big);
return val;
}

