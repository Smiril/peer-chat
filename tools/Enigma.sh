#!/bin/bash
#!/dev/tty1
ARGV=("$@")
ARGC=("$#")

echo " +****************************************************************************+"
echo " + Enigma 0.5                                    ###     Art-ifact      ###   +" 
echo " + CODENAME:                                    /   ##     #####      ##   \\  +"
echo " +           T.E.D.                           #\\     ##   #######   ##     /# +"
echo " +           The Enemy Dail                     #     ###############     #   +"
echo " +           KOENIG-MARTIN                    #/      ###############      \\# +"
echo " +                                             \\   #\\##    ##^##    ##/#   /  +"
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
    echo "${0} EKMFLGDQVZNTOWYHXUSPAIBRCJ AJDKSIRUXBLHWTMCQGZNPYFVOE BDFHJLCPRTXVZNYEIWGAKMUSQO ESOVPZJAYQUIRHXLNFTGKDCMWB VZBRGITYUPSDNHLXAWMJQOFECK YRUHQSLDPXNGOKMIEBFZCWVJAT QEVJZX 999999 A"
    exit 0
fi

echo " +****************************************************************************+ 
   enigma simulation , main and bombe, sonar@gmx.com, july 2018
   the encoding scheme uses code from harald schmidl's
   5 rotor German Enigma simulation."

echo " +****************************************************************************+
   Written by  Smiril
   7 lines Info , 20 lines License , 456 lines Code 
   @ https://github.com/Smiril/peer-chat/tree/master/tools/Enigma.sh"

echo " +****************************************************************************+
   LICENSE:MIT License"

echo "
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
SOFTWARE. " > ./LICENSE

echo "
#include <stdio.h>
#include <unistd.h> 
#include <string.h> 
#include <memory.h> 
#include <cstdlib> 
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <cstring> 
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
int Modulus(int iN, int iMod) {
	int iQ = (iN/iMod);
	return iN - (iQ*iMod);
}
char GetChar(int iGenerator, char cBase, int iRange) {
	return (cBase + Modulus(iGenerator, iRange));
}
int matrix() {
	char caRow[80];
	int j = 7;
	int k = 2;
	int l = 5;
	int m = 1;
	for (int ccc = 0;ccc <= 255;ccc++) {
		// Output a random row of characters
		for ( int i = 0; i < 80; ++i ) 
		{
			if (caRow[i] != ' ') {
				caRow[i] = GetChar(j + i*i, 33, 30);
				if (((i*i + k) % 71) == 0) {
					;
				} else {
					;
				}
			}
			std::cout << caRow[i];
			;
		}
		j = (j + 31);
		k = (k + 17);
		l = (l + 47);
		m = (m + 67);
		caRow[Modulus(j, 80)] = '-';
		caRow[Modulus(k, 80)] = ' ';
		caRow[Modulus(l, 80)] = '-';
		caRow[Modulus(m, 80)] = ' ';
		// Delay
		Sleep(10);
	} //end for
    return 0;
}
void logo(){
	printf(\"\\t\\t\\t\\t     (\\               /)\\n\");
	printf(\"\\t\\t\\t\\t    __)\\             /(__\\n\");
	printf(\"\\t\\t\\t\\t   __)_ \\  (\\!~!/)  / _(__\\n\");
	printf(\"\\t\\t\\t\\t  __)_ `.\\  )d b(  /.' _(__\\n\");
	printf(\"\\t\\t\\t\\t___)_ `. \\\(  _  )// .' _(___\\n\");
	printf(\"\\t\\t\\t\\t )_  `. \\  ((q_p))  / .'  _(_\\n\");
	printf(\"\\t\\t\\t\\t _)`.  \\  ,-)\\_/(-.  /  .'(_\\n\");
	printf(\"\\t\\t\\t\\t  _) ,-._/v/vvvvv\v\\_,-. (_\\n\");
	printf(\"\\t\\t\\t\\t  _)///(._/v(___)v\\_.)\\\(_\\n\");
	printf(\"\\t\\t\\t\\t   \\_ ___/v(_____)v\\___ _/\\n\");
	printf(\"\\t\\t\\t\\t     /vvv\\(_______)/vvv\\\\n\");
	printf(\"\\t\\t\\t\\t     \\vvv/v(_____)v\\vvv/  \\n\");
	printf(\"\\t\\t\\t\\t     _\\vv\\\v(___)v//vv/_\\n·\");
	printf(\"\\t\\t\\t\\t    '>_`  \\`-._.-'/  '_<`\\n\");
	printf(\"\\t\\t\\t\\t    ' >_,-'       `-._<`\\n\");
}
/* Rotor wirings */
std::string rotor[5]={/* CHANGE THIS BLOCK 1-5+ref+notch */ 
	/* Input \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\" */ 
	/* 1: */ \"${1}\", 
	/* 2: */ \"${2}\", 
	/* 3: */ \"${3}\", 
	/* 4: */ \"${4}\", 
	/* 5: */ \"${5}\"};
std::string ref=\"${6}\"; 
std::string notch=\"${7}\"; 
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
  char in[MSGLEN]; 
  int c, i = 0; 
  while((c = getchar()) != '\\n') 
  { 
    in[i] = toupper(c); 
    i++; 
  } 
  in[i] = '\0'; 
  
  printf(\"%s\\n%s\\n\", enigma(in, &p), in); 
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
	      
		(*ct)++; 
	        printf(\"\\x1B[33mWheels\\x1B[39m \\x1B[32m%d %d %d\x1B[39m \x1B[33mStart\\x1B[39m \\x1B[32m%c %c %c\\x1B[39m \\x1B[33mRings\\x1B[39m \\x1B[32m%c %c %c\\x1B[39m \\x1B[33mStecker\\x1B[39m \\\"\\x1B[32m%s\\x1B[39m\\\"\\n\", 
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
int test(int a, int b, int c, char *cyph, char *crib, int *ct) 
{ 
  char A, B, C, D; 
  int i = 0, cs; 
  char s[6]; 
  strcpy(s, \"\"); 
  printf(\"Checking wheels \\x1B[33m%d %d %d\\x1B[39m\\n\",  a, b, c); 
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
                printf(\" Progress: \\x1B[32m%d\\x1B[39m of Combinations done.\\n\",(int) progress1); 
              } 
            } 
          } 
	    } 
	  } 
	  else{ 
	    float progress2 = 1 * 1 * 1 * (float) strlen(cyph) * (float) strlen(crib) * 6 ; 
	    if(rotate(a, b, c, cyph, crib, s, ct)==0){ 
	     	printf(\" Progress: \\x1B[32m%d\\x1B[39m of Combinations done.\\n\",(int) progress2); 
        	} 
	  	  } 
	    } 
      } 
    } 
    else{ 
    float progress3 = 1 * 1 * 1 * (float) strlen(cyph) * (float) strlen(crib) * 6 ; 
    if(rotate(a, b, c, cyph, crib, s, ct)==0){ 
     	printf(\" Progress: \\x1B[32m%d\\x1B[39m of Combinations done.\\n\",(int) progress3); 
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
  for(int d = 1;d<=9;d++){ 
	  for(int e = 1;e<=9;e++){ 
		  for(int f = 1;f<=9;f++){ 
			  permute(d, e, f, cyph, crib, &ct); 
		  } 
	  } 
  } 
printf(\"\\nFound \\x1B[32m%d\\x1B[39m solutions.\\n\", ct); 
} 
/*once triples of five possible wheels*/
void permuteOnce(char *cyph, char *crib,int d,int e,int f) 
{ 
  int ct = 0; 
  permute(d, e, f, cyph, crib, &ct); 
  printf(\"\\nFound \\x1B[32m%d\\x1B[39m solutions.\\n\", (int)ct); 
} 
/*helper to read a character*/
char readCh() 
{ 
  char c, ret; 
  while((c = getchar()) != '\\n') 
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
      p->order[i] = i + i + 3;  
      p->rings[i] = 'D' + i + 1; 
      p->pos[i] = 'G' + i + 2; 
    } 
    strcpy(p->plug, \"TOPASKING\"); 
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
  printf(\"\\x1B[33mWheels\\x1B[39m \\x1B[32m %d %d %d \\x1B[39m \\x1B[33mStart\\x1B[39m \\x1B[32m %c %c %c \\x1B[39m \\x1B[33mRings\\x1B[39m \\x1B[32m %c %c %c \\x1B[39m Stecker \\\"\\x1B[32m%s\\x1B[39m\\\"\\n\", 
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
		printf("\x1b[32m");
		logo();
		matrix();
		printf("\x1b[0m");
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
	          if(strcmp(argv[x], \"--version\") == 0) 
	          { 
                  printf(\"\\tVersion\\n\\n\\t%s\\n\",Versionx()); 
              } 
	          if(strcmp(argv[x], \"--help\") == 0) 
	          { 
                  printf(\"\\tHelp\\n\\n\\t\\x1B[33m--option-1 PLUG MSG\\x1B[39m = Enigma Crack Algo from 1 1 1 to 9 9 9\\n\\t\\x1B[33m--option-2 PLUG MSG NUM NUM NUM\\x1B[39m = Enigma Crack Once Algo\\n\\t\\x1B[33m--option-3\\x1B[39m = Enigma 5 Rotor Calculator\\n\\t\\x1B[33m--version\\x1B[39m = Version\\n\\n\"); 
              } 
        } 
  return 0 ; 
} 
 " > ./Enigma.cc

g++ ./Enigma.cc -Wall -Wextra -pedantic -Wno-variadic-macros -DVERSION="\"Enigma 0.5 \\x1B[32mT.E.D.\\x1B[39m - \\x1B[33mThe Enemy Dail\\x1B[39m - KOENIG-MARTIN \"" -lstdc++ -o enigma 

mkdir -p bin

mv ./enigma bin

chmod +x bin/enigma

rm -rf ./Enigma.cc
	
echo " +****************************************************************************+"
echo " + Enigma 0.5                                    ###     Art-ifact      ###   +" 
echo " + CODENAME:                                    /   ##     #####      ##   \\  +"
echo " +           T.E.D.                           #\\     ##   #######   ##     /# +"
echo " +           The Enemy Dail                     #     ###############     #   +"
echo " +           KOENIG-MARTIN                    #/      ###############      \\# +"
echo " +                                             \\   #\\##    ##^##    ##/#   /  +"
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

exit 0
