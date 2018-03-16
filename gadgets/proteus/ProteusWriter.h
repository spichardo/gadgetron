#ifndef ProteusWriter_H
#define ProteusWriter_H

#include "proteus_export.h"
#include "GadgetMessageInterface.h"
#include "GadgetMRIHeaders.h"
#include "ismrmrd/ismrmrd.h"
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>

namespace Gadgetron {

class EXPORTGADGETSPROTEUS ProteusWriter : public GadgetMessageWriter
{
 public:
  virtual int write(ACE_SOCK_Stream* sock, ACE_Message_Block* mb);

  GADGETRON_WRITER_DECLARE(ProteusWriter);
};

} /* namespace Gadgetron */

#endif
