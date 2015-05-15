#pragma once
#include <string.h>

struct md5_ctx
{
	unsigned int	buf[4];
	unsigned int	bits[2];
	unsigned char	in[64];
};

void md5_init(md5_ctx *ctx);
void md5_update(md5_ctx *ctx, const unsigned char *buf, unsigned int length);
void md5_final(md5_ctx *ctx, unsigned char digest[16]);

unsigned int md5_random(unsigned int seed);