#pragma once
#include <string.h>

struct md5_ctx
{
	unsigned int	buf[4];
	unsigned int	bits[2];
	unsigned char	in[64];
};

void MD5_Init(md5_ctx *ctx);
void MD5_Update(md5_ctx *ctx, const unsigned char *buf, unsigned int length);
void MD5_Final(md5_ctx *ctx, unsigned char digest[16]);

unsigned int MD5_PseudoRandom(unsigned int seed);