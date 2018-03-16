//Source code taken from https://github.com/gadgetron/gadgetron/blob/master/gadgets/dicom/DicomImageWriter.cpp
//Hyperthermia edits found at +PROTEUS (ctrl+f to find indexes)
//Details: Configure new port (55555) and send all data there instead of back to original "input" port.
//         If connection fails, send to original "input" port (i.e. default behavior)
//         Only sends DICOM data (no filename or attribs)
//Edited by: Eric Song

#include <complex>
#include <fstream>
#include <time.h>

// Gadgetron includes
#include "GadgetIsmrmrdReadWrite.h"
#include "ProteusWriter.h"
#include "GadgetContainerMessage.h"
#include "hoNDArray.h"
#include "ismrmrd/meta.h"

// DCMTK includes
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstdinc.h"
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcostrmb.h"

namespace Gadgetron {

int ProteusWriter::write(ACE_SOCK_Stream* sock, ACE_Message_Block* mb)
{
    //+PROTEUS: Create new output sock, where address is defined in xml. Default addr is ip="192.168.56.1", port=44444.
    GadgetContainerMessage<ACE_INET_Addr>* addr_message = AsContainerMessage<ACE_INET_Addr>(mb);
    ACE_INET_Addr addr = *addr_message->getObjectPtr();
    ACE_SOCK_Stream outSock;
    ACE_SOCK_Connector conn;
    if (conn.connect(outSock, addr) == -1) {
      GERROR("Failed to connect to outgoing port\n");
      return -1;
    }
    else {  //Only replace sock if the outgoing sock is connected.
      sock = &outSock;
    }

    GadgetContainerMessage<DcmFileFormat>* dcm_file_message = AsContainerMessage<DcmFileFormat>(mb->cont());
    if (!dcm_file_message)
    {
      GERROR("ProteusWriter::write, invalid image message objects\n");
      return -1;
    }

    DcmFileFormat *dcmFile = dcm_file_message->getObjectPtr();

    // Initialize transfer state of DcmDataset
    dcmFile->transferInit();

    // Calculate size of DcmFileFormat and create a SUFFICIENTLY sized buffer
    long buffer_length = dcmFile->calcElementLength(EXS_LittleEndianExplicit, EET_ExplicitLength) * 2;
    std::vector<char> bufferChar(buffer_length);
    char* buffer = &bufferChar[0];

    DcmOutputBufferStream out_stream(buffer, buffer_length);

    OFCondition status;

    status = dcmFile->write(out_stream, EXS_LittleEndianExplicit, EET_ExplicitLength, NULL);
    if (!status.good()) {
      GERROR("Failed to write DcmFileFormat to DcmOutputStream(%s)\n", status.text());
      return GADGET_FAIL;
    }

    void *serialized = NULL;
    offile_off_t serialized_length = 0;
    out_stream.flushBuffer(serialized, serialized_length);

    // finalize transfer state of DcmDataset
    dcmFile->transferEnd();

    ssize_t send_cnt = 0;

    uint32_t nbytes = (uint32_t)serialized_length;
    if ((send_cnt = sock->send_n (&nbytes, sizeof(nbytes))) <= 0)
    {
      GERROR("Unable to send DICOM bytes length\n");
      return -1;
    }

    if ((send_cnt = sock->send_n (serialized, serialized_length)) <= 0)
    {
      GERROR("Unable to send DICOM bytes\n");
      return -1;
    }

    //+PROTEUS: Do not send filename or meta attribs

    return 0;
}

GADGETRON_WRITER_FACTORY_DECLARE(ProteusWriter)

} /* namespace Gadgetron */
