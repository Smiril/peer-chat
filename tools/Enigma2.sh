#!/bin/bash
#!/dev/tty1
ARGV=("$@")
ARGC=("$#")

echo " +****************************************************************************+"
echo " + Enigma II 0.6                                 ###     Art-ifact      ###   +" 
echo " + CODENAME:                                    /   ##     #####      ##   \\  +"
echo " +           T.E.D.                           #\\     ##   #######   ##     /# +"
echo " +           The Enemy Dail                     #     ###############     #   +"
echo " +           KOENIG-MARTIN                    #/      ###############      \\# +"
echo " +           TESLA                             \\   #\\##    ##^##    ##/#   /  +"
echo " +                                              ##/   \\#####/_\\#####/    \\##  +"
echo " +                                                      \\#########/           +"
echo " +                                                      / # # # # \\           +"
echo " +                                               /###__/   # # #   \\__###\\    +"
echo " +                                               #        /#####\\        #    +"
echo " +                                               \\######## R A T ########/    +"
echo " +                                                     Entertainment          +"
echo " +****************************************************************************+"

if [ "$#" -ne 9 ]; then
    echo "USAGE: ${0} rotor-1 rotor-2 rotor-3 rotor-4 rotor-5 ref-rotor notch-rotor msg-length fixing-letter"
    echo "USAGE: ${0} EKMFLGDQVZNTOWYHXUSPAIBRCJ AJDKSIRUXBLHWTMCQGZNPYFVOE BDFHJLCPRTXVZNYEIWGAKMUSQO ESOVPZJAYQUIRHXLNFTGKDCMWB VZBRGITYUPSDNHLXAWMJQOFECK YRUHQSLDPXNGOKMIEBFZCWVJAT QEVJZX 999999 A"
    exit 0
fi

sudo zypper install at-spi2-atk-common at-spi2-atk-gtk2 gtk2-data gtk2-devel gtk2-immodule-amharic gtk2-immodule-inuktitut gtk2-metatheme-adwaita gtk2-tools libopenssl-devel patterns-openSUSE-base patterns-openSUSE-devel_basis patterns-openSUSE-devel_C_C++ git

echo " +****************************************************************************+ 
   enigma simulation , main and bombe, sonar@gmx.com, september 2018
   the encoding scheme uses code from harald schmidl's
   5 rotor German Enigma simulation. The Calculating 
   scheme uses from smoes SHA1 CUDA Bruteforce Calculating"

echo " +****************************************************************************+
   Written by  Smiril
   8 lines Info , 20 lines License , 1039 lines Code 
   @ https://github.com/Smiril/peer-chat/tree/master/tools/Enigma2.sh"

echo " +****************************************************************************+
   LICENSE:MIT License"

echo "
MIT License

Copyright (c) 2018-* sonar@gmx.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the \"Software\"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. " > ./LICENSE

echo "
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <cuda.h>
#include <device_functions.h>
#define word const char

__device__ inline word f1( word x, word y, word z) { return ( ( x & y ) | ( ~x & z ) ); }
__device__ inline word f2( word x, word y, word z) { return ( x ^ y ^ z ); }
__device__ inline word f3( word x, word y, word z) { return ( ( x & y ) | ( x & z ) | ( y & z ) ); }
__device__ inline word f4( word x, word y, word z) { return ( x ^ y ^ z ); } 

__shared__ word * hash;

/* SHA256 init values */

__constant__ word I1 = 0x6a09e667;
__constant__ word I2 = 0xbb67ae85;
__constant__ word I3 = 0x3c6ef372;
__constant__ word I4 = 0xa54ff53a;
__constant__ word I5 = 0x510e527f;
__constant__ word I6 = 0x9b05688c;
__constant__ word I7 = 0x1f83d9ab;
__constant__ word I8 = 0x5be0cd19;

/* SHA256 constants */

__constant__ word C1 = 0x428a2f98;
__constant__ word C2 = 0x71374491;
__constant__ word C3 = 0xb5c0fbcf;
__constant__ word C4 = 0xe9b5dba5;
__constant__ word C5 = 0x3956c25b;
__constant__ word C6 = 0x59f111f1;
__constant__ word C7 = 0x923f82a4;
__constant__ word C8 = 0xab1c5ed5;
__constant__ word C9 = 0xd807aa98;
__constant__ word C10 = 0x12835b01;
__constant__ word C11 = 0x243185be;
__constant__ word C12 = 0x550c7dc3;
__constant__ word C13 = 0x72be5d74;
__constant__ word C14 = 0x80deb1fe;
__constant__ word C15 = 0x9bdc06a7;
__constant__ word C16 = 0xc19bf174;
__constant__ word C17 = 0xe49b69c1;
__constant__ word C18 = 0xefbe4786;
__constant__ word C19 = 0x0fc19dc6;
__constant__ word C20 = 0x240ca1cc;
__constant__ word C21 = 0x2de92c6f;
__constant__ word C22 = 0x4a7484aa;
__constant__ word C23 = 0x5cb0a9dc;
__constant__ word C24 = 0x76f988da;
__constant__ word C25 = 0x983e5152;
__constant__ word C26 = 0xa831c66d;
__constant__ word C27 = 0xb00327c8;
__constant__ word C28 = 0xbf597fc7;
__constant__ word C29 = 0xc6e00bf3;
__constant__ word C30 = 0xd5a79147;
__constant__ word C31 = 0x06ca6351;
__constant__ word C32 = 0x14292967;
__constant__ word C33 = 0x27b70a85;
__constant__ word C34 = 0x2e1b2138;
__constant__ word C35 = 0x4d2c6dfc;
__constant__ word C36 = 0x53380d13;
__constant__ word C37 = 0x650a7354;
__constant__ word C38 = 0x766a0abb;
__constant__ word C39 = 0x81c2c92e;
__constant__ word C40 = 0x92722c85;
__constant__ word C41 = 0xa2bfe8a1;
__constant__ word C42 = 0xa81a664b;
__constant__ word C43 = 0xc24b8b70;
__constant__ word C44 = 0xc76c51a3;
__constant__ word C45 = 0xd192e819;
__constant__ word C46 = 0xd6990624;
__constant__ word C47 = 0xf40e3585;
__constant__ word C48 = 0x106aa070;
__constant__ word C49 = 0x19a4c116;
__constant__ word C50 = 0x1e376c08;
__constant__ word C51 = 0x2748774c;
__constant__ word C52 = 0x34b0bcb5;
__constant__ word C53 = 0x391c0cb3;
__constant__ word C54 = 0x4ed8aa4a;
__constant__ word C55 = 0x5b9cca4f;
__constant__ word C56 = 0x682e6ff3;
__constant__ word C57 = 0x748f82ee;
__constant__ word C58 = 0x78a5636f;
__constant__ word C59 = 0x84c87814;
__constant__ word C60 = 0x8cc70208;
__constant__ word C61 = 0x90befffa;
__constant__ word C62 = 0xa4506ceb;
__constant__ word C63 = 0xbef9a3f7;
__constant__ word C64 = 0xc67178f2;

/* 32-bit rotate */

__device__ inline word ROT(word x,int n){ return ( ( x << n ) | ( x >> ( 32 - n ) ) ); }

#define CALC(n,i) temp =  ROT ( A , 5 ) + f##n( B , C, D ) +  W[i] + E + C##n  ; E = D; D = C; C = ROT ( B , 30 ); B = A; A = temp

#define CHUNK_SIZE 64
#define TOTAL_LEN_LEN 8

/*
 * ABOUT bool: this file does not use bool in order to be as pre-C99 compatible as possible.
 */

/*
 * Comments from pseudo-code at https://en.wikipedia.org/wiki/SHA-2 are reproduced here.
 * When useful for clarification, portions of the pseudo-code are reproduced here too.
 */

/*
 * Initialize array of round constants:
 * (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
 */
__constant__ static const uint32_t k[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

struct buffer_state {
	const uint8_t * p;
	size_t len;
	size_t total_len;
	int single_one_delivered; /* bool */
	int total_len_delivered; /* bool */
};

__device__ static inline uint32_t right_rot(uint32_t value, unsigned int count)
{
	/*
	 * Defined behaviour in standard C for all count where 0 < count < 32,
	 * which is what we need here.
	 */
	return value >> count | value << (32 - count);
}

static void init_buf_state(struct buffer_state * state, const uint8_t * input, size_t len)
{
	state->p = input;
	state->len = len;
	state->total_len = len;
	state->single_one_delivered = 0;
	state->total_len_delivered = 0;
}

/* Return value: bool */
__device__ static int calc_chunk(uint8_t chunk[CHUNK_SIZE], struct buffer_state * state)
{
	size_t space_in_chunk;

	if (state->total_len_delivered) {
		return 0;
	}

	if (state->len >= CHUNK_SIZE) {
		memcpy(chunk, state->p, CHUNK_SIZE);
		state->p += CHUNK_SIZE;
		state->len -= CHUNK_SIZE;
		return 1;
	}

	memcpy(chunk, state->p, state->len);
	chunk += state->len;
	space_in_chunk = CHUNK_SIZE - state->len;
	state->p += state->len;
	state->len = 0;

	/* If we are here, space_in_chunk is one at minimum. */
	if (!state->single_one_delivered) {
		*chunk++ = 0x80;
		space_in_chunk -= 1;
		state->single_one_delivered = 1;
	}

	/*
	 * Now:
	 * - either there is enough space left for the total length, and we can conclude,
	 * - or there is too little space left, and we have to pad the rest of this chunk with zeroes.
	 * In the latter case, we will conclude at the next invokation of this function.
	 */
	if (space_in_chunk >= TOTAL_LEN_LEN) {
		const size_t left = space_in_chunk - TOTAL_LEN_LEN;
		size_t len = state->total_len;
		int i;
		memset(chunk, 0x00, left);
		chunk += left;

		/* Storing of len * 8 as a big endian 64-bit without overflow. */
		chunk[7] = (uint8_t) (len << 3);
		len >>= 5;
		for (i = 6; i >= 0; i--) {
			chunk[i] = (uint8_t) len;
			len >>= 8;
		}
		state->total_len_delivered = 1;
	} else {
		memset(chunk, 0x00, space_in_chunk);
	}

	return 1;
}

#define MSGLEN ${8}
#define TO '${9}'

char s[MSGLEN];

using namespace std;

const char* Versionx() {
#ifdef VERSION
  return VERSION;
#else
  return \"WarGames 0.4 T.E.D. - The Enemy Dail - ENEMY-MODE\";
#endif
}


void sha256_hash_string (char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
    int i = 0;

    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), \"%02x\", hash[i]);
    }

    outputBuffer[64] = 0;
}

__device__ int barke_sha256 (const char* path, char output[65])
{
    char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, path, sizeof(path));
    SHA256_Final((unsigned char*)hash, (SHA256_CTX*)&sha256);

    sha256_hash_string(hash, output);
    
    return 0;
}

__device__ void calc_sha_256(uint8_t hash[32], const uint8_t * input, size_t len)
{
	/*
	 * Note 1: All integers (expect indexes) are 32-bit unsigned integers and addition is calculated modulo 2^32.
	 * Note 2: For each round, there is one round constant k[i] and one entry in the message schedule array w[i], 0 = i = 63
	 * Note 3: The compression function uses 8 working variables, a through h
	 * Note 4: Big-endian convention is used when expressing the constants in this pseudocode,
	 *     and when parsing message block data from bytes to words, for example,
	 *     the first word of the input message \"abc\" after padding is 0x61626380
	 */

	/*
	 * Initialize hash values:
	 * (first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
	 */
	uint32_t h[] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	int i, j;

	/* 512-bit chunks is what we will operate on. */
	uint8_t chunk[64];

	struct buffer_state state;

	init_buf_state(&state, input, len);

	while (calc_chunk(chunk, &state)) {
		uint32_t ah[8];
		
		/*
		 * create a 64-entry message schedule array w[0..63] of 32-bit words
		 * (The initial values in w[0..63] don't matter, so many implementations zero them here)
		 * copy chunk into first 16 words w[0..15] of the message schedule array
		 */
		uint32_t w[64];
		const uint8_t *p = chunk;

		memset(w, 0x00, sizeof w);
		for (i = 0; i < 16; i++) {
			w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
				(uint32_t) p[2] << 8 | (uint32_t) p[3];
			p += 4;
		}

		/* Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array: */
		for (i = 16; i < 64; i++) {
			const uint32_t s0 = right_rot(w[i - 15], 7) ^ right_rot(w[i - 15], 18) ^ (w[i - 15] >> 3);
			const uint32_t s1 = right_rot(w[i - 2], 17) ^ right_rot(w[i - 2], 19) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		
		/* Initialize working variables to current hash value: */
		for (i = 0; i < 8; i++)
			ah[i] = h[i];

		/* Compression function main loop: */
		for (i = 0; i < 64; i++) {
			const uint32_t s1 = right_rot(ah[4], 6) ^ right_rot(ah[4], 11) ^ right_rot(ah[4], 25);
			const uint32_t ch = (ah[4] & ah[5]) ^ (~ah[4] & ah[6]);
			const uint32_t temp1 = ah[7] + s1 + ch + k[i] + w[i];
			const uint32_t s0 = right_rot(ah[0], 2) ^ right_rot(ah[0], 13) ^ right_rot(ah[0], 22);
			const uint32_t maj = (ah[0] & ah[1]) ^ (ah[0] & ah[2]) ^ (ah[1] & ah[2]);
			const uint32_t temp2 = s0 + maj;

			ah[7] = ah[6];
			ah[6] = ah[5];
			ah[5] = ah[4];
			ah[4] = ah[3] + temp1;
			ah[3] = ah[2];
			ah[2] = ah[1];
			ah[1] = ah[0];
			ah[0] = temp1 + temp2;
		}

		/* Add the compressed chunk to the current hash value: */
		for (i = 0; i < 8; i++)
			h[i] += ah[i];
	}

	/* Produce the final hash value (big-endian): */
	for (i = 0, j = 0; i < 8; i++)
	{
		hash[j++] = (uint8_t) (h[i] >> 24);
		hash[j++] = (uint8_t) (h[i] >> 16);
		hash[j++] = (uint8_t) (h[i] >> 8);
		hash[j++] = (uint8_t) h[i];
	}
}

/* Rotor wirings */
__constant__ std::string rotor[5]={/* CHANGE THIS BLOCK 1-5+ref+notch */
	/* 1: */ \"${1}\", 
	/* 2: */ \"${2}\", 
	/* 3: */ \"${3}\", 
	/* 4: */ \"${4}\", 
	/* 5: */ \"${5}\"};
__constant__ std::string ref=\"${6}\"; 
__constant__ std::string notch=\"${7}\"; 

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*=\"AMTE\";*/
} Params;

/*take a char and return its encoded version according to the 
  encryption params, update params, i.e. advance wheels
  this part uses Fauzan Mirza's code*/
__device__ char scramble(char c, Params *p)
{
  int i, j, flag = 0;

		c=toupper(c);
		if (!isalpha(c))
			return -1;
		
		/* Step up first rotor */
		p->pos[0]++;
		if (p->pos[0]>'Z')
			p->pos[0] -= 26;

		/* Check if second rotor reached notch last time */
		if (flag)
		{
			/* Step up both second and third rotors */
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			p->pos[2]++;
			if (p->pos[2]>'Z')
				p->pos[2] -= 26;
			flag=0;
		}

		/*  Step up second rotor if first rotor reached notch */
		if (p->pos[0]==notch[p->order[0]-1])
		{
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			/* Set flag if second rotor reached notch */
			if (p->pos[1]==notch[p->order[1]-1])
				flag=1;
		}

		/*  Swap pairs of letters on the plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

		/*  Rotors (forward) */
		for (i=0; i<3; i++)
		{
			c += p->pos[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i]-'A';
			if (c<'A')
				c += 26;

			c=rotor[p->order[i]-1][c-'A'];

			c += p->rings[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i]-'A';
			if (c<'A')
				c += 26;
		}

		/*  Reflecting rotor */
		c=ref[c-'A'];

		/*  Rotors (reverse) */
		for (i=3; i; i--)
		{
			c += p->pos[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i-1]-'A';
			if (c<'A')
				c += 26;

			for (j=0; j<26; j++)
				if (rotor[p->order[i-1]-1][j]==c)
					break;
			c=j+'A';

			c += p->rings[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i-1]-'A';
			if (c<'A')
				c += 26;
		}
		
		/*  Plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

  return c;
}

/*take a string, return encoded string*/
char *enigma(char *in, Params *p)
{
  unsigned int j;
  for(j = 0; j < strlen(in); j++)
  s[j] = scramble(in[j], p);
  s[j] = '\0';
  return s;
}

/*read in a string, and pass it through enigma*/
void cypher(Params p)
{
  char in[MSGLEN];//, s[MSGLEN];
  int c, i = 0;

  while((c = getchar()) != '\n')
  {
    in[i] = toupper(c);
    i++;
  }
  in[i] = '\0';
  //strcpy(s, enigma(in, &p));
  printf(\"%s\\n%s\\n\", enigma(in, &p), in);
}

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
__device__ int rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct)
{
  Params p;

  p.order[0] = a;
  p.order[1] = b;
  p.order[2] = c;
  p.rings[0] = p.rings[1] = p.rings[2] = 'A';
  strcpy(p.plug, plug);
  for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
  {
    for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
    {
      for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
      {
       for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++)
        {
          for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
          {
            for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
            {
	      Params cp = p;
	      unsigned int i = 0;

	      while(sizeof(crib) > i)
	      {
		if(cyph[i] != scramble(crib[i], &cp)){
		break;
		}
		else{
			i++;
			}
	      }
	      if(sizeof(crib) == i)
	      {
		//char s[MSGLEN];

		(*ct)++;
	        printf(\"Wheels %d %d %d Start %c %c %c Rings %c %c %c Stecker \\\"%s\\\"\\n\",
                        p.order[0], p.order[1], p.order[2], 
                        p.pos[0], p.pos[1], p.pos[2],
                        p.rings[0], p.rings[1], p.rings[2], p.plug);
	        cp = p;
	        
            	printf(\"%s decoded -> %s\\n\", (char *)cyph, enigma(cyph, &cp));
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

/*do the whole check including steckering of up to two pairs of letters*/
__device__ int test(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  char A, B, C, D;
  int i = 0, cs;
  char s[5];

  strcpy(s, cyph);
  printf(\"Checking wheels %d %d %d\\n\",  a, b, c);
  for(cs = 0; cs < 3; cs++)
  {
    if(cs > 0)
    {
      for(A = 'A'; A <= TO; A++)
      {
        for(B = A + 1; B <= TO; B++)
        {
	  s[0] = A;
	  s[1] = B;
	  s[2] = '\0';
	  if(cs > 1)
	  {
	    for(C = A + 1; C <= TO; C++)
	    {
	      if(C == B)
	      continue;
	      for(D = C + 1; D <= TO; D++)
              {
	        if(D == A || D == B){
		continue;
		i++;
		s[2] = C;
		s[3] = D;
		s[4] = '\0';
             float progress1 = 1 * 1 * 1 * (float) strlen(cyph) * (float) strlen(crib) * 6 ;

             if(rotate(a, b, c, cyph, crib, s, ct)==0){
                printf(\" Progress: %d of Combinations done.\\n\",(int) progress1);
              }
            }
          }
	    }
	  }
	  else{
	    float progress2 = 1 * 1 * 1 * (float) strlen(cyph) * (float) strlen(crib) * 6 ;

	    if(rotate(a, b, c, cyph, crib, s, ct)==0){
	     	printf(\" Progress: %d of Combinations done.\\n\",(int) progress2);
        	}
	  	  }
	    }
      }
    }
    else{
    float progress3 = 1 * 1 * 1 * (float) strlen(cyph) * (float) strlen(crib) * 6 ;

    if(rotate(a, b, c, cyph, crib, s, ct)==0){
     	printf(\" Progress: %d of Combinations done.\\n\",(int) progress3);
     	}
    }
  }
  return 0;
}


/*run on all permutations of wheels a, b, c*/
__device__ void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  test(a, b, c, cyph, crib, ct);
  test(a, c, b, cyph, crib, ct);
  test(b, a, c, cyph, crib, ct);
  test(b, c, a, cyph, crib, ct);
  test(c, a, b, cyph, crib, ct);
  test(c, b, a, cyph, crib, ct);
}

/*all triples of five possible wheels*/
__device__ void permuteAll(char *cyph, char *crib)
{
  int ct = 0;
  std::string d,e,f;


  for(int d = 1;d<=9;d++){
	  for(int e = 1;e<=9;e++){
		  for(int f = 1;f<=9;f++){
			  permute(d, e, f, cyph, crib, &ct);
		  }
	  }
  }
printf(\"\\nFound %d solutions.\\n\", ct);
}

/*once triples of five possible wheels*/
__device__ void permuteOnce(char *cyph, char *crib,int d,int e,int f)
{
  int ct = 0;
  permute(d, e, f, cyph, crib, &ct);
  printf(\"\\nFound %d solutions.\\n\", (int)ct);
}

/*helper to read a character*/
char readCh()
{
  char c, ret;

  while((c = getchar()) != '\n')
  ret = c;
  return ret;
}

/*init the starting position*/
void initParams(Params *p)
{
  int i;
  char c;

  printf(\"d)efault or u)ser: \");
  c = readCh();
  if(c != 'u')
  {
    for(i = 0; i < 3; i++)
    {
      p->order[i] = i + 1;
      p->rings[i] = 'A';
      p->pos[i] = 'A';
    }
    strcpy(p->plug, " ");
  }
  else
  {
      for(i = 0; i < 3; i++)
      {
        printf(\"Wheel %d: \", i + 1);
        p->order[i] = readCh() - 48;
      }
      for(i = 0; i < 3; i++)
      {
        printf(\"Ring  %d: \", i + 1);
        p->rings[i] = readCh();
      }
      for(i = 0; i < 3; i++)
      {
        printf(\"Start %d: \", i + 1);
        p->pos[i] = readCh();
      }
      printf(\"Stecker: \");
      i = 0;
      while((c = getchar()) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }
  printf(\"Wheels %d %d %d Start %c %c %c Rings %c %c %c Stecker \\\"%s\\\"\\n\",
         p->order[0], p->order[1], p->order[2], 
         p->pos[0], p->pos[1], p->pos[2],
         p->rings[0], p->rings[1], p->rings[2], p->plug);
}

void start(word * hash_tmp,  word * length, const char * res)
{
    unsigned char * buffer = 0;
    unsigned char * buffer_fill = 0x0;
    cudaMalloc((void**)&buffer,10 * sizeof(buffer));
    cudaMalloc((void**)&hash,5 * sizeof(hash));
    

    for(int i = 0; i < 10; i++)
        buffer_fill[i] = 0x0; 
    
    
    cudaMemcpy ((void*)hash, hash_tmp,sizeof(hash_tmp),cudaMemcpyHostToDevice);
    cudaMemcpy (buffer, buffer_fill,sizeof(buffer_fill),cudaMemcpyHostToDevice);
    
    // Call actual brute force kernel-function with 
    // - blocks: count of possible chars squared
    // - threads: possible chars
   
    smash<<<9025,95>>>(length, buffer, hash);

    cudaMemcpy((void*)res, (const void*)buffer, sizeof(buffer),cudaMemcpyDeviceToHost);
    //cudaMemcpy(debug, hash, 5 * sizeof(word), cudaMemcpyDeviceToHost);

    cudaError_t err1 = cudaGetLastError();
    if( cudaSuccess != err1) 
        printf( \"Cuda error: %s.\\n\",  cudaGetErrorString(err1) );

    cudaFree((void*)buffer);
    cudaFree((void*)hash);
}

__global__ void smash(int length, char * buffer, word * hash)
{
    word h0,h1,h2,h3,h4,h5,h6,h7;
    int higher = 126;
    int lower = 32;
    char *input_cpy = 0;
    int carry = 1;

    // load into register
    h0 = hash[0];
    h1 = hash[1];
    h2 = hash[2];
    h3 = hash[3];
    h4 = hash[4];
    h5 = hash[5];
    h6 = hash[6];
    h7 = hash[7];

    if(length > 3)
       for(int i = 3; i < 10; i++)
           input_cpy[i] = lower;
       

    // init input_cpy
    input_cpy[0] = threadIdx.x + lower;
    if(length > 1)
        input_cpy[1] = (blockIdx.x / 95) + lower;
            if(length > 2)
	    		input_cpy[2] = (blockIdx.x % 95) + lower;

    // Length for carry flag (break) if length < 3
    short int s = length < 3 ? length : 3;

    // value @length as a flag.
    //if (s != nullptr)
    for(int i = length; i < 10; i++)
            input_cpy[i] = 0;
        
    
    // Init words for SHA
    word W[80],A,B,C,D,E,F,G,H,temp;
 
    // calculate all possible charsets with the
    // given threadId, blockId and length
    while(input_cpy[length] == 0 && buffer[0] == 0) //@TODO || flag) 
    {
        // Calculate sha256 for given input.
        // DO THE SHA256 ------------------------------------------------------
        //calc_sha256(hash, input_cpy, length);
        
        memInit(W, input_cpy, length);
        for (i = 0; i < 16; i++) {
			w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
				(uint32_t) p[2] << 8 | (uint32_t) p[3];
			p += 4;
		}

		/* Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array: */
		for (i = 16; i < 64; i++) {
			const uint32_t s0 = right_rot(w[i - 15], 7) ^ right_rot(w[i - 15], 18) ^ (w[i - 15] >> 3);
			const uint32_t s1 = right_rot(w[i - 2], 17) ^ right_rot(w[i - 2], 19) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		
		/* Initialize working variables to current hash value: */
		for (i = 0; i < 8; i++)
			ah[i] = h[i];

		/* Compression function main loop: */
		for (i = 0; i < 64; i++) {
			const uint32_t s1 = right_rot(ah[4], 6) ^ right_rot(ah[4], 11) ^ right_rot(ah[4], 25);
			const uint32_t ch = (ah[4] & ah[5]) ^ (~ah[4] & ah[6]);
			const uint32_t temp1 = ah[7] + s1 + ch + k[i] + w[i];
			const uint32_t s0 = right_rot(ah[0], 2) ^ right_rot(ah[0], 13) ^ right_rot(ah[0], 22);
			const uint32_t maj = (ah[0] & ah[1]) ^ (ah[0] & ah[2]) ^ (ah[1] & ah[2]);
			const uint32_t temp2 = s0 + maj;

			ah[7] = ah[6];
			ah[6] = ah[5];
			ah[5] = ah[4];
			ah[4] = ah[3] + temp1;
			ah[3] = ah[2];
			ah[2] = ah[1];
			ah[1] = ah[0];
			ah[0] = temp1 + temp2;
		}

		/* Add the compressed chunk to the current hash value: */
		for (i = 0; i < 8; i++)
			h[i] += ah[i];
	}

	/* Produce the final hash value (big-endian): */
	for (i = 0, j = 0; i < 8; i++)
	{
		hash[j++] = (uint8_t) (h[i] >> 24);
		hash[j++] = (uint8_t) (h[i] >> 16);
		hash[j++] = (uint8_t) (h[i] >> 8);
		hash[j++] = (uint8_t) h[i];
	}
        
        for(int i = 16; i < 80; i++)
            W[i] = ROT( ( W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16] ) , 1 ); 
        
        
        A = I1;    B = I2;    C = I3;    D = I4;    E = I5;

        CALC(1,0);  CALC(1,1);  CALC(1,2);  CALC(1,3);  CALC(1,4);
        CALC(1,5);  CALC(1,6);  CALC(1,7);  CALC(1,8);  CALC(1,9);
        CALC(1,10); CALC(1,11); CALC(1,12); CALC(1,13); CALC(1,14);
        CALC(1,15); CALC(1,16); CALC(1,17); CALC(1,18); CALC(1,19);
        CALC(2,20); CALC(2,21); CALC(2,22); CALC(2,23); CALC(2,24);
        CALC(2,25); CALC(2,26); CALC(2,27); CALC(2,28); CALC(2,29);
        CALC(2,30); CALC(2,31); CALC(2,32); CALC(2,33); CALC(2,34);
        CALC(2,35); CALC(2,36); CALC(2,37); CALC(2,38); CALC(2,39);
        CALC(3,40); CALC(3,41); CALC(3,42); CALC(3,43); CALC(3,44);
        CALC(3,45); CALC(3,46); CALC(3,47); CALC(3,48); CALC(3,49);
        CALC(3,50); CALC(3,51); CALC(3,52); CALC(3,53); CALC(3,54);
        CALC(3,55); CALC(3,56); CALC(3,57); CALC(3,58); CALC(3,59);
        CALC(4,60); CALC(4,61); CALC(4,62); CALC(4,63); CALC(4,64);
        CALC(4,65); CALC(4,66); CALC(4,67); CALC(4,68); CALC(4,69);
        CALC(4,70); CALC(4,71); CALC(4,72); CALC(4,73); CALC(4,74);
        CALC(4,75); CALC(4,76); CALC(4,77); CALC(4,78); CALC(4,79);
    
        // That needs to be done, == with like (A + I1) =0 hash[0] 
        // is wrong all the time?!
        word tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;   
 
        tmp1 = A + I1;
        tmp2 = B + I2;
        tmp3 = C + I3;
        tmp4 = D + I4;
        tmp5 = E + I5;
        tmp6 = F + I6;
        tmp7 = G + I7;
        tmp8 = H + I8;

        // if result was found, cpy to buffer
        if( tmp1 == h0 && 
            tmp2 == h1 &&
            tmp3 == h2 &&
            tmp4 == h3 &&
            tmp5 == h4 &&
            tmp6 == h5 &&
            tmp7 == h6 &&
            tmp8 == h7 )
        { 
            buffer[0] = input_cpy[0];   
            buffer[1] = input_cpy[1];   
            buffer[2] = input_cpy[2];   
            buffer[3] = input_cpy[3];   
            buffer[4] = input_cpy[4];   
            buffer[5] = input_cpy[5];   
            buffer[6] = input_cpy[6];   
            buffer[7] = input_cpy[7];   
            buffer[8] = input_cpy[8];   
            buffer[9] = input_cpy[9];   
            
            break;
        }
        
        // adding new value
        // DO THE ADDITION ----------------------------------------------
    
        for(int i = s; i < 10; i++)
        {
            if(carry)
            {
                input_cpy[i] = input_cpy[i]+ 1;
                if(input_cpy[i] > higher)
                {
                    input_cpy[i] = lower;
                    carry = 1;
                } else 
                    carry = 0;
            } else 
                break;
        }

        carry = 1;

    }

}


/*
 * device function __device__ void memInit(uint, uchar, int)
 * 
 * Prepare word for sha-256 (expand, add length etc)
*/


__device__ void memInit(word * tmp, unsigned char input[], int length)
{

    int stop = 0;
    // reseting tmp
    for(int i = 0; i < 80; i++) tmp[i] = 0;

    // fill tmp like: message char c0,c1,c2,...,cn,10000000,00...000
    for(int i = 0; i < length; i+=4)
    {
        for(int j = 0; j < 4; j++)
            if(i + j < length)
                tmp[i/4] |= input[i+j] << (24-j * 8);
            else 
            {
                stop = 1;
                break;
            }
        if(stop)
            break;
    }
    tmp[length/4] |= 0x80 << (24-(length%4) * 8);     // Append 1 then zeros
    // Adding length as last value
    tmp[15] |= length * 8;
}

/********************************************MAIN*********************************************/
int main(int argc, char *argv[])
{
  Params p;
  int x;
  const char *resx;

	if(argc == 1){ /*main case*/
		printf(\"Option usage: %s --help\\n\",argv[0]);
		exit(1);
	}

	for(x=0; x<argc; x++) /*bombe case*/
	{
	          if(strcmp(argv[x], \"--option-1\") == 0)
	          {
                  printf(\"Option 1\\n\");
                  permuteAll(argv[x + 1], argv[x + 2]);
              }
		if(strcmp(argv[x], \"--option-2\") == 0)
	          {
                  printf(\"Option 2\\n\");
                  permuteOnce(argv[x + 1], argv[x + 2], atoi(argv[x + 3]), atoi(argv[x + 4]), atoi(argv[x + 5]));
              }
	          if(strcmp(argv[x], \"--option-3\") == 0)
	          {
                  printf(\"Option 3\\n\");
                  initParams(&p);
                  cypher(p);
              }
              if(strcmp(argv[x], \"--option-4\") == 0)
	          {
                  printf(\"Option 4\\n\");
                  start(argv[x + 1], argv[x + 2],resx);
              }
	          if(strcmp(argv[x], \"--barke\") == 0)
	          {
                    char calc_hash[65];
                    std::string name1;
                    puts (\"Please, enter a Message: \");
                    getline(cin,name1);
                    printf("");
                    barke_sha256(name1.c_str(), calc_hash);
                    printf (\"%s - %s \\n\",name1.c_str(),calc_hash);
              }
	          if(strcmp(argv[x], \"--version\") == 0)
	          {
                  printf(\"Version\\n\\n\\t%s\\n\",Versionx());
              }
	          if(strcmp(argv[x], \"--help\") == 0)
	          {
                  printf(\"Help\\n\\n\\t--option-1 PLUG MSG = Enigma Crack Algo\\n\\t--option-2 PLUG MSG NUM NUM NUM = Enigma Crack Once Algo\\n\\t--option-3 = Enigma 5 Rotor Calculator\\n\\t--option-4 MSGHASH MSGLEN = Enigma Bruteforce MSG Calculator\\n\\t--barke = SHA256 MSGHASH\\n\\t--version = Version\\n\\n\");
              }
        }
  return 0 ;
}

 " > ./main.cu

/usr/local/cuda-9.2/bin/nvcc main.cu -x cu -DVERSION="\"Enigma II 0.6 CUDA T.E.D. - The Enemy Dail - KOENIG-MARTIN - TESLA\"" -lssl -lcrypto -lpthread -lstdc++ -o enigma-cuda -I/usr/local/cuda-9.2/include

mkdir -p bin

mv ./enigma-cuda bin

chmod +x bin/enigma-cuda

rm -rf ./main.cu
	
echo " +****************************************************************************+"
echo " + Enigma II 0.6                                 ###     Art-ifact      ###   +" 
echo " + CODENAME:                                    /   ##     #####      ##   \\  +"
echo " +           T.E.D.                           #\\     ##   #######   ##     /# +"
echo " +           The Enemy Dail                     #     ###############     #   +"
echo " +           KOENIG-MARTIN                    #/      ###############      \\# +"
echo " +           TESLA                             \\   #\\##    ##^##    ##/#   /  +"
echo " +                                              ##/   \\#####/_\\#####/    \\##  +"
echo " +                                                      \\#########/           +"
echo " +                                                      / # # # # \\           +"
echo " +                                               /###__/   # # #   \\__###\\    +"
echo " +                                               #        /#####\\        #    +"
echo " +                                               \\######## R A T ########/    +"
echo " +                                                     Entertainment          +"
echo " +****************************************************************************+"
 
cat ./LICENSE

rm -rf ./LICENSE

enigma-cuda --version

exit 0
