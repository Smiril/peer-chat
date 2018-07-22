/* enigma simulation , main and bombe, sonar@gmx.com, july 2018
   the encoding scheme uses code from harald schmidl's
   5 rotor German Enigma simulation.

   Written by  Smiril
   8 lines Info , 23 lines License , 62 lines Makefile , 439 lines Code 
   @ https://github.com/Smiril/peer-chat/tree/master/tools/Enigma.cc

   LICENSE:

MIT License

Copyright (c) 2018-* sonar@gmx.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
   MAKEFILE:

NAME := Enigma
MAJOR := 0
MINOR := 5
CODENAME := \x1B[32mT.E.D.\x1B[39m - \x1B[33mThe Enemy Dail\x1B[39m - KOENIG-MARTIN
LIBNAME := $(NAME)
VERSION := $(LIBNAME) $(MAJOR).$(MINOR) $(CODENAME)
LIBS := -lstdc++
PREFIX := bin
CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic \
        -Wno-variadic-macros \
        -DVERSION="\"$(VERSION)\""

.PHONY: all enigma install uninstall clean version

all: enigma
enigma: Enigma.cc
	$(CXX) Enigma.cc $(CXXFLAGS) $(LIBS) -o enigma
debug-enigma: CXXFLAGS += -g -DDEBUG
debug-enigma: Enigma.cc
	$(CXX) $(CXXFLAGS) Enigma.cc $(LIBS) -o debug-enigma
version:
	@echo " +****************************************************************************+"
	@echo " + Enigma 0.4                                     ###      Art-ifact      ###   +" 
	@echo " + CODENAME: T.E.D - KOENIG-MARTIN              /   ##      #####      ##   \\  +"
	@echo " +                                            #\\     ##   #######   ##     /# +"
	@echo " +                                              #     ###############     #   +"
	@echo " +                                            #/      ###############      \\# +"
	@echo " +                                             \\   #\\##    ##^##    ##/#   /  +"
	@echo " +                                              ##/   \\#####/_\\#####/    \\##  +"
	@echo " +                                                      \\#########/           +"
	@echo " +                                                      / # # # # \\           +"
	@echo " +                                               /###__/   # # #   \\__###\\    +"
	@echo " +                                               #        /#####\\        #    +"
	@echo " +                                               \\######## R A T ########/    +"
	@echo " +                                                     Entertainment          +"
	@echo " +****************************************************************************+"
	@cat ./LICENSE
	

install: enigma
	mkdir -p $(PREFIX)
	cp enigma $(PREFIX)
	chmod +x $(PREFIX)/enigma

uninstall:
	rm -rf $(PREFIX)/enigma
clean:
	rm -rf enigma

*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>
#include <iostream>
#include <cstring>

#define MSGLEN 80
#define TO 'E'

char s[MSGLEN];

using namespace std;

const char* Versionx() {
#ifdef VERSION
  return VERSION;
#else
  return "WarGames 0.4 T.E.D. - The Enemy Dail - ENEMY-MODE";
#endif
}

/* Rotor wirings */
std::string rotor[5]={/* CHANGE THIS BLOCK 1-5+ref+notch */
	/* Input "ABCDEFGHIJKLMNOPQRSTUVWXYZ" */
	/* 1: */ "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	/* 2: */ "AJDKSIRUXBLHWTMCQGZNPYFVOE",
	/* 3: */ "BDFHJLCPRTXVZNYEIWGAKMUSQO",
	/* 4: */ "ESOVPZJAYQUIRHXLNFTGKDCMWB",
	/* 5: */ "VZBRGITYUPSDNHLXAWMJQOFECK" };

std::string ref="YRUHQSLDPXNGOKMIEBFZCWVJAT";

std::string notch="QEVJZ";

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*="AMTE";*/
} Params;

/*take a char and return its encoded version according to the 
  encryption params, update params, i.e. advance wheels
  this part uses Fauzan Mirza's code*/
char scramble(char c, Params *p)
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
  printf("%s\n%s\n", enigma(in, &p), in);
}

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
int rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct)
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
		char s[MSGLEN];

		(*ct)++;
	        printf("\x1B[33mWheels\x1B[39m \x1B[32m%d %d %d\x1B[39m \x1B[33mStart\x1B[39m \x1B[32m%c %c %c\x1B[39m \x1B[33mRings\x1B[39m \x1B[32m%c %c %c\x1B[39m \x1B[33mStecker\x1B[39m \"\x1B[32m%s\x1B[39m\"\n",
                        p.order[0], p.order[1], p.order[2], 
                        p.pos[0], p.pos[1], p.pos[2],
                        p.rings[0], p.rings[1], p.rings[2], p.plug);
	        cp = p;
	        
            	printf("%s decoded -> %s\n", (char *)cyph, enigma(cyph, &cp));
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
int test(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  char A, B, C, D;
  int i = 0, cs;
  char s[5];

  strcpy(s, "");
  printf("Checking wheels \x1B[33m%d %d %d\x1B[39m\n",  a, b, c);
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
             float progress1 = (float) a * (float) b * (float) c * (float) strlen(cyph)  ;

             if(rotate(a, b, c, cyph, crib, s, ct)==0){
                printf(" Progress: \x1B[32m%d\x1B[39m of 1000%% done.\n",(int) progress1);
              }
            }
          }
	    }
	  }
	  else{
	    float progress2 = (float) a * (float) b * (float) c * (float) strlen(cyph) ;

	    if(rotate(a, b, c, cyph, crib, s, ct)==0){
	     	printf(" Progress: \x1B[32m%d\x1B[39m of 1000%% done.\n",(int) progress2);
        	}
	  	  }
	    }
      }
    }
    else{
    float progress3 = (float) a * (float) b * (float) c * (float) strlen(cyph) ;

    if(rotate(a, b, c, cyph, crib, s, ct)==0){
     	printf(" Progress: \x1B[32m%d\x1B[39m of 1000%% done.\n",(int) progress3);
     	}
    }
  }
  return 0;
}


/*run on all permutations of wheels a, b, c*/
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  test(a, b, c, cyph, crib, ct);
  test(a, c, b, cyph, crib, ct);
  test(b, a, c, cyph, crib, ct);
  test(b, c, a, cyph, crib, ct);
  test(c, a, b, cyph, crib, ct);
  test(c, b, a, cyph, crib, ct);
}

/*all triples of five possible wheels*/
void permuteAll(char *cyph, char *crib)
{
  int ct = 0;
  std::string d,e,f;


  for(int d = 1;d<=999;d++){
	  for(int e = 1;e<=999;e++){
		  for(int f = 1;f<=999;f++){
			  permute(d, e, f, cyph, crib, &ct);
		  }
	  }
  }
printf("\nFound \x1B[32m%d\x1B[39m solutions.\n", ct);
}

/*once triples of five possible wheels*/
void permuteOnce(char *cyph, char *crib,char *d,char *e,char *f)
{
  int ct = 0;
  permute(d, e, f, cyph, crib, &ct);
  printf("\nFound \x1B[32m%d\x1B[39m solutions.\n", ct);
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

  printf("d)efault or u)ser: ");
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
        printf("Wheel %d: ", i + 1);
        p->order[i] = readCh() - 48;
      }
      for(i = 0; i < 3; i++)
      {
        printf("Ring  %d: ", i + 1);
        p->rings[i] = readCh();
      }
      for(i = 0; i < 3; i++)
      {
        printf("Start %d: ", i + 1);
        p->pos[i] = readCh();
      }
      printf("Stecker: ");
      i = 0;
      while((c = getchar()) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }
  printf("\x1B[33mWheels\x1B[39m \x1B[32m%d %d %d\x1B[39m \x1B[33mStart\x1B[39m \x1B[32m%c %c %c\x1B[39m \x1B[33mRings\x1B[39m \x1B[32m%c %c %c\x1B[39m Stecker \"\x1B[32m%s\x1B[39m\"\n",
         p->order[0], p->order[1], p->order[2], 
         p->pos[0], p->pos[1], p->pos[2],
         p->rings[0], p->rings[1], p->rings[2], p->plug);
}


/********************************************MAIN*********************************************/
int main(int argc, char *argv[])
{
  Params p;
  int x;

	if(argc == 1){ /*main case*/
		printf("Option usage: %s --help\n",argv[0]);
		exit(1);
	}

	for(x=0; x<argc; x++) /*bombe case*/
	{
	          if(strcmp(argv[x], "--option-1") == 0)
	          {
                  printf("Option 1\n");
                  permuteAll(argv[x + 1], argv[x + 2]);
              }
		if(strcmp(argv[x], "--option-2") == 0)
	          {
                  printf("Option 2\n");
                  permuteOnce(argv[x + 1], argv[x + 2], argv[x + 3], argv[x + 4], argv[x + 5]);
              }
	          if(strcmp(argv[x], "--option-3") == 0)
	          {
                  printf("Option 3\n");
                  initParams(&p);
                  cypher(p);
              }
	          if(strcmp(argv[x], "--version") == 0)
	          {
                  printf("\tVersion\n\n\t%s\n",Versionx());
              }
	          if(strcmp(argv[x], "--help") == 0)
	          {
                  printf("\tHelp\n\n\t\x1B[33m--option-1 PLUG MSG\x1B[39m = Enigma Crack Algo\n\t\x1B[33m--option-2 PLUG MSG NUM NUM NUM\x1B[39m = Enigma Crack Once Algo\n\t\x1B[33m--option-3\x1B[39m = Enigma 5 Rotor Calculator\n\t\x1B[33m--version\x1B[39m = Version\n\n");
              }
        }
  return 0 ;
}
