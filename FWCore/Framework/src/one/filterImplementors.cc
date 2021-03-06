// -*- C++ -*-
//
// Package:     Package
// Class  :     producerImplementors
// 
// Implementation:
//     Explicitly instantiate implementor templates for EDFilterBase
//
// Original Author:  Chris Jones
//         Created:  Thu, 09 May 2013 20:14:06 GMT
// $Id: filterImplementors.cc,v 1.1 2013/05/17 14:49:46 chrjones Exp $
//

// system include files

// user include files
#include "FWCore/Framework/src/one/implementorsMethods.h"
#include "FWCore/Framework/interface/one/EDFilterBase.h"

namespace edm {
  namespace one {
    namespace impl {
      template class RunWatcher<edm::one::EDFilterBase>;
      template class LuminosityBlockWatcher<edm::one::EDFilterBase>;
      template class BeginRunProducer<edm::one::EDFilterBase>;
      template class EndRunProducer<edm::one::EDFilterBase>;
      template class BeginLuminosityBlockProducer<edm::one::EDFilterBase>;
      template class EndLuminosityBlockProducer<edm::one::EDFilterBase>;
    }
  }
}
