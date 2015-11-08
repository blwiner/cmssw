#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "GTCollections.h"

namespace l1t {
   namespace stage2 {
      class GlobalAlgBlkUnpacker : public Unpacker {
         public:
            virtual bool unpack(const Block& block, UnpackerCollections *coll) override;
      };
   }
}

// Implementation

namespace l1t {
namespace stage2 {
   bool
   GlobalAlgBlkUnpacker::unpack(const Block& block, UnpackerCollections *coll)
   {

     LogDebug("L1T") << "Block ID  = " << block.header().getID() << " size = " << block.header().getSize();

     unsigned int wdPerBX = 6; //Should this be configured someplace?
     int nBX = int(ceil(block.header().getSize() / 6.)); // FOR GT Not sure what we have here...put at 6 because of 6 frames 

     // Find the central, first and last BXs
     int firstBX = -(ceil((double)nBX/2.)-1);
     int lastBX;
     if (nBX % 2 == 0) {
       lastBX = ceil((double)nBX/2.);
     } else {
       lastBX = ceil((double)nBX/2.)-1;
     }

     auto res_ = static_cast<GTCollections*>(coll)->getAlgs();
     res_->setBXRange(firstBX, lastBX);

     LogDebug("L1T")  << "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX  << endl;

     // Loop over multiple BX and then number of EG cands filling collection
     int numBX = 0;  //positive int to count BX
     for (int bx=firstBX; bx<=lastBX; bx++){

       
        // If this is the first block, instantiate GlobalAlg so it is there to fill from mult. blocks
       if(block.header().getID()==1) {

	  LogDebug("L1T") << "Creating GT Algorithm Block for BX =" << bx;
          GlobalAlgBlk talg = GlobalAlgBlk();
          res_->push_back(bx,talg);

       }

       //fetch
       GlobalAlgBlk alg = res_->at(bx,0);

       //Determine offset of algorithm bits based on block.ID
       // ID=1  offset = 0;  ID=3  offset=192;  ID=5  offset=384=2*192; (before prescale)
       // ID=7  offset = 0;  ID=9  offset=192;  ID=11 offset=384=2*192; (after prescale)
       // ID=13 offset = 0;  ID=15 offset=192;  ID=17 offset=384=2*192; (after mask (Final))
       int algOffset = block.header().getID()/2;
       algOffset = (algOffset%3)*192;

       for(unsigned int wd=0;  wd<wdPerBX; wd++) {
         uint32_t raw_data = block.payload()[wd+numBX*wdPerBX];
	 LogDebug("L1T") << "BX "<<bx << " payload word " << wd << " 0x" << hex << raw_data << " offset=" << dec << algOffset  << std::endl;

         //parse these 32 bits into algorithm bits (perhaps needs a more efficient way of doing this?
         if( (block.header().getID()!=5 && block.header().getID()!=11 && block.header().getID()!=17 ) || wd<4) {
           for(unsigned int bt=0; bt<32; bt++) {
	     int val = ((raw_data >> bt) & 0x1);
	     int algBit = bt+wd*32+algOffset;
             if(val==1 && algBit < 128) { //FIX ME...get dimension from object
	         LogDebug("L1T") << "Found valid alg bit ("<< algBit <<") on bit ("<<bt<<") word ("<<wd<<") algOffset ("<<algOffset<<") block ID ("<< block.header().getID() <<")" <<std::endl;
	        if(block.header().getID()<6) {
		  alg.setAlgoDecisionInitial(algBit,true);
		} else if(block.header().getID()<12) {  
		  alg.setAlgoDecisionPreScaled(algBit,true);
		} else {  
		  alg.setAlgoDecisionFinal(algBit,true);
		}  
	     } else if(val==1) {
	         LogDebug("L1T") << "Found invalid alg bit ("<< algBit <<") out of range (128) on bit ("<<bt<<") word ("<<wd<<") algOffset ("<<algOffset<<") block ID ("<< block.header().getID() <<")" <<std::endl;
	     }
           }
	 } else if(block.header().getID()==17 && wd==4) {
           //This is the FINOR
           alg.setFinalOR(raw_data);
           LogDebug("L1T")  << " Packing the FinalOR " << wd << " 0x" << hex << raw_data << endl;	
         }
       }

       // Put the object back into place (Must be better way)
       res_->set(bx,0,alg);

       //alg.print(std::cout);
       
       //increment counter of which BX we are processing
       numBX++; 
     }

     return true;
   }
}
}

DEFINE_L1T_UNPACKER(l1t::stage2::GlobalAlgBlkUnpacker);
