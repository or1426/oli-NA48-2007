/* File to define variables to be used in usersrc routines */
#include "user.h"

FILE * fprt;
/* returns the dot product of two float 3-vectors */
float f3vdot(float *x,float *y) 
{
  return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
}

/* returns the square of a float 3-vector magnitude */
float f3vmag2(float *v)
{
  return ( _POWER2(v[0]) + _POWER2(v[1]) + _POWER2(v[2]) );
}

/* Returns the magnitude of a float 3-vector */
float f3vmag(float *v)
{
  return (float)sqrt( _POWER2(v[0]) + _POWER2(v[1]) + _POWER2(v[2]) );
}

/*Returns the (-+++) dot product of two four vectors*/
float f4vdot(float * v1, float * v2)
{
  return -v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
} 
         
int pointOfClosestApproach (float * point1, float * point2, float * v1, float * v2, float * dmin, float * vertex)
{

  //translate from track params to vector notation                                                  

  // printf("34 point1[0]: %f\n",point1[0]);
  int i;
  float r12[3];
  float det,q1,q2,t1,t2;
  float x;
  for(i=0;i<3;i++) 
  {
    r12[i]=point1[i]-point2[i];
  }
  //printf("43 point1[0]: %f\n",point1[0]);

 /*                                                                                                                          
  12-08-98 IWS change pow to a simple product to avoid                                                                                                                                                      
               segmentation fault...                                                                                                                                                                        
  det=pow(f3vdot(v1,v2),2)-f3vmag2(v1)*f3vmag2(v2);                                                                                                                                                         
  */
  x=f3vdot(v1,v2);
  det = _POWER2(x) - f3vmag2(v1)*f3vmag2(v2);
  //printf("Det: %f\n",det);
   if(det!=0.)
   {
      t1=(f3vmag2(v2)*f3vdot(r12,v1) - f3vdot(v1,v2) * f3vdot(r12,v2))/det;
      t2=(f3vdot(v1,v2)*f3vdot(r12,v1) - f3vmag2(v1) * f3vdot(r12,v2))/det;
      //printf("56 point1[0]: %f\n",point1[0]);
     for(i=0; i<3; i++) 
     {
       q1 = point1[i] + (t1*v1[i]);
       q2 = point2[i] + (t2*v2[i]);
       vertex[i]=(q1+q2)/2.0;
       r12[i]=q1-q2;
     }
    *dmin = f3vmag(r12);

    return 0; //good                                                                           
   }
   else
   {
     return 1; //indicating parallel lines                        
   }                                                                  
}


int mcEventType(superMcEvent * evt)
{
  /*
  if( (evt->Npart < 5) || (evt->part[0] != KAON) )
    {
      return UNKNOWN_EVENT;
    }
  */

  int pi0Count=0, piPlusCount=0, eCount=0, muCount=0,gammaCount=0,unknownCount=0,kCount=0;
  for(int i = 0;i<evt->Npart;++i)
    {
      switch(evt->part[i].type)
	{
	case 4:
	  ++pi0Count;
	  break;
	case 8:
	  ++piPlusCount;
	  break;
	case 16:
	  ++gammaCount;
	  break;
	case 32:
	  ++muCount;
	  break;
	case 64:
	  ++eCount;
	  break;
	case 512:
	  ++kCount;
	  break;
	default:
	  ++unknownCount;
	  break;
	}
    }

  
  if( (pi0Count==1) && (eCount==1) && (gammaCount >=2)  &&
      (unknownCount==0) && (piPlusCount==0) && (muCount==0) )
    {
      return KE3_EVENT;
    }

  /****************************************/
  /*apparently the things I've decided are*/
  /*probably electrons randomly turn up in*/
  /*events in the file labeled kµ3        */
  /*               wtf?                   */
  /****************************************/
  if( (gammaCount>=2) && (pi0Count==1) && (muCount==1) 
      /*&&(unknownCount==0) */&& (piPlusCount==0) /* && (eCount==0)*/ ) 
    {
      return KM3_EVENT;
    } 
  if( (gammaCount>=2) && (pi0Count==1) && (piPlusCount==1)/* &&
      (unknownCount==0) && (eCount==0) && (muCount==0)*/ )
    {
      //printf("k2pi\n");
      return K2P_EVENT;
    }

  printf("P0: %d\n",pi0Count);
  printf("P+: %d\n",piPlusCount);
  printf("e+: %d\n",eCount);
  printf("m+: %d\n",muCount);
  printf("ga: %d\n",gammaCount);
  printf("k+: %d\n",kCount);
  printf("un: %d\n",unknownCount);
  printf("\n");
  
  for(int i = 0;i<evt->Npart;++i)
    {
      printf("Type: %d\n",evt->part[i].type);
    } 
  printf("\n\n");
  //if we get here we are a bit clueless!
  return UNKNOWN_EVENT;
  
}
