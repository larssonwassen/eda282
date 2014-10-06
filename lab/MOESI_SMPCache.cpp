#include "MESI_SMPCache.h"

//extern void computeOpt(std::vector<uint64_t> &candidates, uint64_t &repl_addr);

MESI_SMPCache::MESI_SMPCache(int cpuid, std::vector<SMPCache * > * cacheVector,
                 int csize, int cassoc, int cbsize, int caddressable, const char * repPol, bool cskew) : 
               SMPCache(cpuid,cacheVector){
  fprintf(stderr,"Making a MESI cache with cpuid %d size %d assoc %d bsize %d\n",cpuid, csize, cassoc, cbsize);
  CacheGeneric<MESI_SMPCacheState> *c = CacheGeneric<MESI_SMPCacheState>::create(csize, cassoc, cbsize, caddressable, repPol, cskew);
  cache = (CacheGeneric<StateGeneric<> >*)c;
}

void MESI_SMPCache::fillLine(uint64_t addr, uint64_t mesi_state){
  MESI_SMPCacheState *st = (MESI_SMPCacheState *)cache->findLine2Replace(addr); //this gets the contents of whateverline this would go into
  if(st==0){
    return;
  }
  //if(st->getTag() == llc_cache->calcTag(addr) && st->getTag() != 0)
  //std::cout << "WTFL1" << std::endl;

  if(st->getState() != 0x00000100 && optCache && (!hasLLC) && this->getCacheVector()->size() == 1) {
    //Find and use repl candidate computed using OPT
    std::vector<uint64_t> temp;
    uint64_t repl_addr;
    //Get other candidates from the set
    cache->getCandidates(addr, temp);
    //Compute the opt replacement candidate tag
    //computeOpt(temp, repl_addr);
    //Use the corresponding tags addres
    st = (MESI_SMPCacheState *)cache->findLine(repl_addr);
    if(!st || (st && !(st->isValid())) ){
      std::cout << "This is not possible!" << std::endl;
    }
    temp.clear();
  }
    
  //std::cout << "Evicting tag " << st->getTag() << " Inserting tag " << cache->calcTag(addr) << std::endl;  
  st->setTag(cache->calcTag(addr));
  st->changeStateTo((MESIState_t)mesi_state);
  return;
    
}
  
void MESI_SMPCache::fillLLC(uint64_t addr){
  MESI_SMPCacheState *st = (MESI_SMPCacheState *)llc_cache->findLine2Replace(addr); //this gets the contents of whateverline this would go into
  if(st==0){
    return;
  }

  //if(st->getTag() == llc_cache->calcTag(addr) && st->getTag() != 0)
  //  std::cout << "WTFL2" << std::endl;

  st->setTag(llc_cache->calcTag(addr));
  st->changeStateTo(MESI_EXCLUSIVE);
  return;
    
}

MESI_SMPCache::RemoteReadService MESI_SMPCache::readRemoteAction(uint64_t addr){

  std::vector<SMPCache * >::iterator cacheIter;
  std::vector<SMPCache * >::iterator lastCacheIter;
  for(cacheIter = this->getCacheVector()->begin(), lastCacheIter = this->getCacheVector()->end(); cacheIter != lastCacheIter; cacheIter++){
    MESI_SMPCache *otherCache = (MESI_SMPCache*)*cacheIter; 

    if(otherCache->getCPUId() == this->getCPUId()){
      continue;
    }
      
    MESI_SMPCacheState* otherState = (MESI_SMPCacheState *)otherCache->cache->findLine(addr);
    if(otherState){
      if(otherState->getState() == MESI_MODIFIED || otherState->getState() == MESI_EXCLUSIVE){

        otherState->changeStateTo(MESI_OWNER);     /*   #####        CHANGEED      ####*/
        return MESI_SMPCache::RemoteReadService(false,true);

      }
      else if (otherState->getState() == MESI_OWNER) { 
        
        return MESI_SMPCache::RemoteReadService(true,true);        
      
      }
      else if(otherState->getState() == MESI_SHARED){  //doesn't matter except that someone's got it

      }
      else if(otherState->getState() == MESI_INVALID){ //doesn't matter at all.

      }
    }

  }//done with other caches

  //fprintf(stderr,"Done with all caches\n");
  //This happens if everyone was MESI_INVALID
  return MESI_SMPCache::RemoteReadService(false,false);
}

void MESI_SMPCache::readLine(uint64_t rdPC, uint64_t addr){

  MESI_SMPCacheState *st = (MESI_SMPCacheState *)cache->findLine(addr);    
  //fprintf(stderr,"In MESI ReadLine\n");
  if(!st || (st && !(st->isValid())) ){//Read Miss -- i need to look in other peoples' caches for this data
    
    numReadMisses++;


    if(st){
      numReadOnInvalidMisses++;
    }

    //Query the other caches and get a remote read service object.
    MESI_SMPCache::RemoteReadService rrs = readRemoteAction(addr);
    numReadRequestsSent++;
      
    MESIState_t newMesiState = MESI_INVALID;
  
    if(rrs.providedData){
   
      //numReadMissesServicedByOthers++;

      if(rrs.isShared){
 
        numReadMissesServicedByShared++;
        accessLLC(addr, true);
         
      }else{ 
      
        numReadMissesServicedByModified++;
      } 

      newMesiState = MESI_SHARED;

    }else{

      newMesiState = MESI_EXCLUSIVE;
      accessLLC(addr, true);

    }
    //Fill the line
    fillLine(addr,newMesiState); 

      
  }else{ //Read Hit

    numReadHits++; 
    return; 

  }

}


MESI_SMPCache::InvalidateReply MESI_SMPCache::writeRemoteAction(uint64_t addr){
    
    bool empty = true;
    std::vector<SMPCache * >::iterator cacheIter;
    std::vector<SMPCache * >::iterator lastCacheIter;
    for(cacheIter = this->getCacheVector()->begin(), lastCacheIter = this->getCacheVector()->end(); cacheIter != lastCacheIter; cacheIter++){
      MESI_SMPCache *otherCache = (MESI_SMPCache*)*cacheIter; 
      if(otherCache->getCPUId() == this->getCPUId()){
        continue;
      }

      //Get the line from the current other cache 
      MESI_SMPCacheState* otherState = (MESI_SMPCacheState *)otherCache->cache->findLine(addr);

      //if it was actually in the other cache:
      if(otherState && otherState->isValid()){
          /*Invalidate the line, cause we're writing*/
          otherState->invalidate();
          empty = false;
      }

    }//done with other caches

    //Empty=true indicates that no other cache 
    //had the line or there were no other caches
    //
    //This data in this object is not used as is, 
    //but it might be useful if you plan to extend 
    //this simulator, so i left it in.
    return MESI_SMPCache::InvalidateReply(empty);
}


void MESI_SMPCache::writeLine(uint64_t wrPC, uint64_t addr){

  MESI_SMPCacheState * st = (MESI_SMPCacheState *)cache->findLine(addr);    
    
  if(!st || (st && !(st->isValid())) ){ //Write Miss
    
    numWriteMisses++;
  
    if(st){
      numWriteOnInvalidMisses++;
    }
  
    MESI_SMPCache::InvalidateReply inv_ack = writeRemoteAction(addr);

    if(inv_ack.empty)
        accessLLC(addr, true);
    else
        numInvalidatesSent++;

    //Fill the line with the new write
    fillLine(addr,MESI_MODIFIED);

    return;

  }else if(st->getState() == MESI_SHARED || st->getState() == MESI_OWNER){ //Coherence Miss
    
    numWriteMisses++;
    numWriteOnSharedMisses++;
      
    writeRemoteAction(addr);
    numInvalidatesSent++;

    st->changeStateTo(MESI_MODIFIED);
    return;

  }else{ //Write Hit

    numWriteHits++;
    st->changeStateTo(MESI_MODIFIED);

    return;

  }

}

void MESI_SMPCache::accessLLC(uint64_t addr, bool isShared){

   //if(!hasLLC || !appvec)
   if(!hasLLC)
     return;
 
   MESI_SMPCacheState *st = (MESI_SMPCacheState *)llc_cache->findLine(addr);    
   if(!st || (st && !(st->isValid()))){
     if(!isShared)
       llcref->numInclusionMisses++; 

     //appvec->push_back(cache->calcTag(addr));
     fillLLC(addr);
     llcref->numLLCMisses++;
   } else { 
     //appvec->push_back(cache->calcTag(addr));
     if(!isShared)
       llcref->numInclusionHits++;
     llcref->numLLCHits++;
   }
}

MESI_SMPCache::~MESI_SMPCache(){

}

char *MESI_SMPCache::Identify(){
  return (char *)"MESI Cache Coherence";
}


extern "C" SMPCache *Create(int num, std::vector<SMPCache*> *cvec, int csize, int casso, int bs, int addrble, const char *repl, bool skw){

  return new MESI_SMPCache(num,cvec,csize,casso,bs,addrble,repl,skw);

}
