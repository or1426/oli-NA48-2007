/*****************************************************************/
/* COmPAC user routine: user_superCmpEvent(superCmpEvent *sevt) */
/*                                                               */
/* User routine called everytime an event `*sevt' is             */
/* loaded. A return value of greater than zero denotes           */
/* an error condition has occured.                               */
/*                                     BH 13/2/98    RWM 20/6/97 */
/*****************************************************************/

/***********************************************/
/* This file is rapidly becoming very very bad */
/***********************************************/

#include "user.h"

int user_superCmpEvent(superBurst *sbur,superCmpEvent *sevt) {
  /* WARNING: do not alter things before this line */
  /*---------- Add user C code here ----------*/
  static int nuserevt=0;
  //printf("(%f,%f,%f)\nthickness: %f\n",Geom->Magnet.x,Geom->Magnet.y,Geom->Magnet.z,Geom->Magnet.thickness);
  if(nuserevt<20)
    {
      printSuperCmpEvent(sevt,fprt);
    }

  float totalNonElectronEnergy = 0;

  int cutWhichKilledEvent = SURVIVED;

  int numUntrackedClusters = 0;

  //we use this to store the indeces of the clusters that don't have a track
  //the NULL is *really* important otherwise we
  //realloc against a random piece of memory and shit goes down
  int * tracklessClusters = NULL;

  //extract 2 untracked clusters
  for(size_t i = 0; (i < sevt->Ncluster);++i)
    {
      if(sevt->cluster[i].iTrack == -1)
        {
          ++numUntrackedClusters;
          totalNonElectronEnergy += sevt->cluster[i].energy;
          tracklessClusters = (int *)realloc(tracklessClusters,
                                             numUntrackedClusters * sizeof(int));
          tracklessClusters[numUntrackedClusters - 1] = i;
        }

    }

  //If we haven't got one track and two untracked clusters we may as well give up now
  //in theory it'd be nice to allow for more esoteric stuff like multiple tracks
  //unfortunately life is too short....
  if( (numUntrackedClusters != 2) || ( sevt->Ntrack != 1) )
    {
      return -1;
    }
  
  //this makes sure the data has the "minimum bias" flag set
  //it only rejects a small fraction and I have no idea what it does
  //but the proper scientists seem to use it...
#if ENABLE_MIN_BIAS_CUT
  if( !((sevt->trigWord >> 11) & 1) )
    {
      cutWhichKilledEvent = MIN_BIAS_CUT;
#if BREAK_ON_FAILED_CUT
      return -1;
#endif
    }
#endif

  //index of track we pick the first and only one now but we could use this to allow more
  //and select the best one if we can be bothered
  //we only want one track so we can do without all the pointer bullshit
  int iTrack = 0;

  //also get the index of the cluster associated with this track (-1 if none)
  int iTrackedCluster = sevt->track[iTrack].iClus;


#if ENABLE_BASIC_QUALITY_CUTS
  for(int i = 0;i<numUntrackedClusters;++i)
    {
      //this became a bit convoluted. sorry.
      if( ((sevt->cluster[tracklessClusters[i]].status > 4) || 
	   (sevt->cluster[tracklessClusters[i]].dDeadCell < 2)) && 
	  (cutWhichKilledEvent == SURVIVED))
	{
	  cutWhichKilledEvent = BASIC_QUALITY_CUTS;
#if BREAK_ON_FAILED_CUT
	  return -1;
#endif
	}
    }
#endif

#if ENABLE_BASIC_QUALITY_CUTS

  if( (sevt->track[iTrack].quality < 0.6) && (cutWhichKilledEvent == SURVIVED) )
    {
      cutWhichKilledEvent = BASIC_QUALITY_CUTS;
#if BREAK_ON_FAILED_CUT
      return 0;
#endif
    }
#endif

  float dtg1g2 = fabs(sevt->cluster[ tracklessClusters[0] ].time - sevt->cluster[ tracklessClusters[1] ].time);
#if ENABLE_TIMING_CUTS
  if( !(dtg1g2 < 2) && (cutWhichKilledEvent == SURVIVED) )
    {
      cutWhichKilledEvent = TIMING_CUTS;
#if BREAK_ON_FAILED_CUT
      return -1;
#endif
    }
#endif
  /****************Apparently this is wrong (in sim)*****************/
  float meanPi0Time = (sevt->cluster[tracklessClusters[0]].time + sevt->cluster[tracklessClusters[1]].time)/2.0;

  float tTrack = sevt->track[iTrack].time;
  float dtTrackPi0Mean = fabs(meanPi0Time - tTrack);
#if ENABLE_TIMING_CUTS
  if( !(dtTrackPi0Mean < 12) )
    {
      cutWhichKilledEvent = TIMING_CUTS;
#if BREAK_ON_FAILED_CUT
      return -1;
#endif
    }
#endif

  if(iTrackedCluster > -1)
    {
      float dtTrackCluster = sevt->cluster[iTrackedCluster].time -sevt->track[iTrack].time;

      /****************Apparently this is wrong in sim*****************/
#if ENABLE_TIMING_CUTS
      if( ((dtTrackCluster < -4 ) || (dtTrackCluster > 4)) && (cutWhichKilledEvent == SURVIVED) )
        {
          cutWhichKilledEvent = TIMING_CUTS;
#if BREAK_ON_FAILED_CUT
          return -1;
#endif
        }
#endif
    }

  //Evgueni knows what this does. I think I understand it...
#if ENABLE_CRAZY_LKR_ACC_CUT
  //here we are working with info from after the magnetic field so we can get the Lkr hit pos
  float dzLkrDCH = Geom->Lkr.z - Geom->DCH.z;
  float lkrPlaneX = sevt->track[iTrack].x + dzLkrDCH*sevt->track[iTrack].dxdz;
  float lkrPlaneY = sevt->track[iTrack].y + dzLkrDCH*sevt->track[iTrack].dydz;
  
  //basically if there is something bad from the lkr this'll return 1
  //if its good then it returns 0
  if (LKr_acc (sbur->nrun, lkrPlaneX, lkrPlaneY, 8)) 
    {
      cutWhichKilledEvent = CRAZY_LKR_ACC_CUT;
#if BREAK_ON_FAILED_CUT
      return -1;
#endif
    }
#endif

  /*FIRST WE WORK OUT THE LOCATION OF THE DECAY USING LKR DATA*/
  //here goes nothing...
  //I miss operator overloading and proper classes
  float chargedPartPoint[3],chargedPartVel[3];
  //for the charged particle here we work with the before magnetic field data so we can get 
  //the vertex location
  chargedPartPoint[0] = sevt->track[iTrack].bx;
  chargedPartPoint[1] = sevt->track[iTrack].by;
  chargedPartPoint[2] = Geom->DCH.bz;

  chargedPartVel[0] = sevt->track[iTrack].bdxdz;
  chargedPartVel[1] = sevt->track[iTrack].bdydz;
  chargedPartVel[2] = 1.0;

  float chargedPartVelMag = f3vmag(chargedPartVel);

  float gamma1LkrVertex[3],gamma2LkrVertex[3];
  gamma1LkrVertex[0] = sevt->cluster[tracklessClusters[0]].x;
  gamma1LkrVertex[1] = sevt->cluster[tracklessClusters[0]].y;
  gamma1LkrVertex[2] = Geom->Lkr.z; //This seems to be correct (fucking magic)!!

  gamma2LkrVertex[0] = sevt->cluster[tracklessClusters[1]].x;
  gamma2LkrVertex[1] = sevt->cluster[tracklessClusters[1]].y;
  gamma2LkrVertex[2] = Geom->Lkr.z;

  float dispGamma1Gamma2[3];
  for(int i = 0; i < 3; ++i)
    {
      dispGamma1Gamma2[i] = gamma1LkrVertex[i] - gamma2LkrVertex[i];
    }

  float gamma1Gamma2Distance = f3vmag(dispGamma1Gamma2);

#if ENABLE_BASIC_QUALITY_CUTS
  if( !((gamma1Gamma2Distance > 10)) && (cutWhichKilledEvent == SURVIVED) )
    {
      cutWhichKilledEvent = BASIC_QUALITY_CUTS;
#if BREAK_ON_FAILED_CUT
      return -1;
#endif
    }
#endif

  float gamma1Energy = sevt->cluster[tracklessClusters[0]].energy;
  float gamma2Energy = sevt->cluster[tracklessClusters[1]].energy;

  //distance in the z direction between the Lkr and the pi0 decay location
  //the pi0 decays really really fast so we can take this as the kaon decay location
  //it can be shown that...
  float dz = gamma1Gamma2Distance*sqrt(gamma1Energy * gamma2Energy)/PI0_MASS;

  //The decay vertex as calculated from lkr data
  //Should be more precise than the methods relying on assumptions about the beam geometry
  //It does rely on the assumption that the photons came from a pi0 though
  float decayVertex[3];

  //I miss operator overloading
  //this would be soooooooo much nicer in a sensible language
  decayVertex[2] = Geom->Lkr.z - dz;
  decayVertex[0] = chargedPartPoint[0]-(chargedPartPoint[2]-decayVertex[2])*chargedPartVel[0];
  decayVertex[1] = chargedPartPoint[1]-(chargedPartPoint[2]-decayVertex[2])*chargedPartVel[1];

#if ENABLE_Z_COORD_CUT
  if( ((decayVertex[2] < -2500) || (decayVertex[2] > 7600)) && (cutWhichKilledEvent == SURVIVED)  )
    {
      cutWhichKilledEvent = Z_COORD_CUT;
#if BREAK_ON_FAILED_CUT
	return -1;
#endif
    }
#endif


  float dispDecayVertexGamma1Vertex[3],dispDecayVertexGamma2Vertex[3];
  for(int i = 0; i < 3;++i)
    {
      dispDecayVertexGamma1Vertex[i] = gamma1LkrVertex[i] - decayVertex[i];
      dispDecayVertexGamma2Vertex[i] = gamma2LkrVertex[i] - decayVertex[i];
    }
  float distDecayVertexGamma1Vertex = f3vmag(dispDecayVertexGamma1Vertex);
  float distDecayVertexGamma2Vertex = f3vmag(dispDecayVertexGamma2Vertex);

  float gamma1VectMom[3],gamma2VectMom[3];
  for(int i = 0; i < 3; ++i)
    {
      //normalise then multiply by momentum magnitude (same as energy for a photon)
      gamma1VectMom[i] = gamma1Energy*(dispDecayVertexGamma1Vertex[i]/distDecayVertexGamma1Vertex);
      gamma2VectMom[i] = gamma2Energy*(dispDecayVertexGamma2Vertex[i]/distDecayVertexGamma2Vertex);
    }

  /*HERE WE RECONSTRUCT THE PI0 INVARIANT MASS*/

  float pi0VectMom[3];
  for(int i = 0; i < 3; ++i)
    {
      pi0VectMom[i] = gamma1VectMom[i] + gamma2VectMom[i];
    }

  float pi0MomMag = f3vmag(pi0VectMom);
  float pi0ReconstructedMass = sqrt( _POWER2(totalNonElectronEnergy) - _POWER2(pi0MomMag));

  //sometimes reconstruction makes the mass imaginary
  //(this is bad so we ignore it and pretend it doesnt happen)
  //In c an imaginary number becomes "-nan" which has the property than -nan != -nan
  if( pi0ReconstructedMass != pi0ReconstructedMass)
    {
      return -1;
    }

  /*HERE WE CALCULATE THE MISSING MOMENTUM (DUE TO AN INVISIBLE PHOTON OR  NEUTRINO STEALING IT)*/

  //this is really dodgy because we've already shown that abcog_params lies to us!
  float beamVelocity[3];
  //assuming +ve kaons may be wrong but it seems to have worked so far
  beamVelocity[0] = abcog_params.pkdxdzp;
  beamVelocity[1] = abcog_params.pkdydzp;
  beamVelocity[2] = 1;

  float beamVelMag = f3vmag(beamVelocity);
  float beamMomMag = abcog_params.pkp;
  float chargedPartMomMag = sevt->track[iTrack].p;

  float chargedPartVectMom[3], beamVectMom[3];
  //why do I write my comments in the first person plural?
  for(int i = 0;i<3;++i)
    {
      chargedPartVectMom[i] = chargedPartMomMag*chargedPartVel[i]/chargedPartVelMag;
      beamVectMom[i] = beamMomMag * beamVelocity[i] / beamVelMag;
    }

  float detectedMom[3];
  for(int i = 0;i<3;++i)
    {
      detectedMom[i] = pi0VectMom[i] + chargedPartVectMom[i];
    }

  float missingMom[3];
  for( int i = 0;i<3;++i)
    {
      missingMom[i] = beamVectMom[i] - detectedMom[i];
    }

  float missingMomMag = f3vmag(missingMom);

  /*HERE WE ASSUME THE CHARGED PARTICLE IS A PI+ AND CALCULATE THE KAON MASS*/
  //It seemed like a good idea at the time
  //see the note in user.h about naming the pi+ Pi
  float pi0Energy = sqrt( _POWER2(PI0_MASS) + _POWER2(pi0MomMag));
  float pi1Energy = sqrt(_POWER2(PI1_MASS) + _POWER2(chargedPartMomMag));
  float kaonMass = sqrt(_POWER2(pi0Energy + pi1Energy) - f3vmag2(detectedMom));

  /*HERE WE CALCULATE THE E/P RATIO TO CUT AGAINST KMU3*/
  float chargedPartEnergy;
  if ( iTrackedCluster != -1)
    {
      chargedPartEnergy = sevt->cluster[iTrackedCluster].energy;
    }
  else
    {
      //according to evgueni if this is -1 it means that the energy was below Lkr threshold
      //what if they miss the Lkr completly?
      //according to evgueni that isn't too common but a bigger problem is them
      //hitting near the edge so some energy is missed...
      chargedPartEnergy = 0;
    }
  float chargedPartEPRatio = chargedPartEnergy / sevt->track[iTrack].p;


  /*POSSIBLY WE CAN USE THE MUON FLAG TO REJECT KMU3*/
  //apparently this is dodgy (we don't really know how they decided to raise this flag)
  //This is -1 if there is no muon, apparently with high reliability
  int iMuon = sevt->track[iTrack].iMuon;
  int muon;
  if(iMuon == -1)
    {
      muon = 0;
    }
  else
    {
      muon = 1;
    }
  float kaonEnergy = sqrt(f3vmag2(beamVectMom) + _POWER2(abcog_params.mkp) );
  /*HERE WE DO THE MISSING MASS CALCULATION*/
  float missingEnergy = kaonEnergy - pi0Energy - chargedPartEnergy;
  //we worked out the missing momentum above

  float missingMass2 = _POWER2(missingEnergy) - _POWER2(missingMomMag);

  /*Try to calculate the pi0 transverse momentum*/
  float beamDirection[3];
  for(int i = 0;i<3;++i)
    {
      beamDirection[i] = beamVelocity[i]/beamVelMag;
    }

  float pi0BeamDirMomMag = f3vdot(pi0VectMom,beamDirection);
  float pi0TransMom[3];
  for(int i = 0;i<3;++i)
    {
      pi0TransMom[i] = pi0VectMom[i] - pi0BeamDirMomMag*beamDirection[i];
    }
  float pi0TransMomMag = f3vmag(pi0TransMom);

#if ENABLE_OUTPUT

  fprintf(realFP,"%f %f %f %f %f %d %f\n",pi0ReconstructedMass, kaonMass, missingMomMag, chargedPartEPRatio,missingMass2,muon,pi0TransMomMag);
  //  fprintf(realFP,"%f %f %f\n",decayVertex[0],decayVertex[1],decayVertex[2]);
#endif


  nuserevt++;

  /*----------- End of user C code -----------*/
  return 0;
}

