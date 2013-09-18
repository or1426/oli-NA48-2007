/********************************************************/
/* COmPACT user routine: user_init()                    */
/*                                                      */
/* User routine called upon program startup to allow    */
/* initialization of the user files, variables etc.     */
/*                                          RWM 20/6/97 */
/********************************************************/

#include "cmpio.h"
#include "user.h"
#include "reader.h"

char * dir = "/afs/cern.ch/user/o/oreardon/private/readerStuff/7foldDataOutput/";
#if ENABLE_OUTPUT

char * realFileName = "real-data.otxt";
char * realFullPath = NULL;
FILE * realFP;

char * ke3FileName = "ke3-data.otxt";
char * ke3FullPath = NULL;
FILE * ke3FP;

char * km3FileName = "km3-data.otxt";
char * km3FullPath = NULL;
FILE * km3FP;

char * k2piFileName = "k2pi-data.otxt";
char * k2piFullPath = NULL;
FILE * k2piFP;

#endif
int user_init() {
        
/* WARNING: do not alter things before this line */
/*---------- Add user C code here ----------*/
	fprt=fopen("compact.txt","w");
#if ENABLE_OUTPUT
	/*NOTE: asprintf isn't portable (if we stay with gcc all is good)*/
	/******I wish I knew why c doesn't have an asprintf natively******/
	
	asprintf(&realFullPath,"%s%s",dir,realFileName);
	realFP = fopen(realFullPath,"w");

	asprintf(&ke3FullPath,"%s%s",dir,ke3FileName);
	ke3FP = fopen(ke3FullPath,"w");

	asprintf(&km3FullPath,"%s%s",dir,km3FileName);
	km3FP = fopen(km3FullPath,"w");

	asprintf(&k2piFullPath,"%s%s",dir,k2piFileName);
	k2piFP = fopen(k2piFullPath,"w");
#endif
/*----------- End of user C code -----------*/
  return 0;
}
