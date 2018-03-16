//Source code taken from https://github.com/gadgetron/gadgetron/blob/master/gadgets/dicom/DicomFinishGadget.h
//Hyperthermia edits found at +PROTEUS (ctrl+f to find indexes)
//Details: Does not generate filename or meta attribs.
//         Send an ACE_INET_Addr (based on xml properties) to the writer for outgoing socket configuration.
//Edited by: Eric Song

#ifndef PROTEUSFINISHGADGET_H
#define PROTEUSFINISHGADGET_H

#include "proteus_export.h"

#include "Gadget.h"
#include "hoNDArray.h"
#include "ismrmrd/meta.h"
#include "GadgetMRIHeaders.h"
#include "ismrmrd/ismrmrd.h"
#include "GadgetStreamController.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstdinc.h"
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcostrmb.h"

#include <ace/INET_Addr.h> //+PROTEUS

#include "mri_core_def.h"

#include "proteus_utility.h"

#include <string>
#include <map>
#include <complex>

namespace Gadgetron
{
    class EXPORTGADGETSPROTEUS ProteusFinishGadget : public Gadget1< ISMRMRD::ImageHeader >
    {
    public:

        typedef Gadget1<ISMRMRD::ImageHeader> BaseClass;

        GADGET_DECLARE(ProteusFinishGadget);

        ProteusFinishGadget()
            : BaseClass()
            , dcmFile()
            , initialSeriesNumber(0)
            , seriesIUIDRoot()
        { }

    protected:

        //+PROTEUS: Gadget properties for socket configuration in ProteusWriter
        GADGET_PROPERTY(ip, std::string, "IP Address", "192.168.56.1");
        GADGET_PROPERTY(port, unsigned short, "Port Number", 44444);

        virtual int process_config(ACE_Message_Block * mb);
        virtual int process(GadgetContainerMessage<ISMRMRD::ImageHeader>* m1);
        virtual int send_message(ACE_Message_Block *mb)
        {
            return this->controller_->output_ready(mb);
        }

        template <typename T>
        int write_data_attrib(GadgetContainerMessage<ISMRMRD::ImageHeader>* m1, GadgetContainerMessage< hoNDArray< T > >* m2)
        {
            //+PROTEUS: Do not create filename or meta attribs

            unsigned short series_number = m1->getObjectPtr()->image_series_index + 1;

            // Try to find an already-generated Series Instance UID in our map
            std::map<unsigned int, std::string>::iterator it = seriesIUIDs.find(series_number);

            if (it == seriesIUIDs.end()) {
                // Didn't find a Series Instance UID for this series number
                char prefix[32];
                char newuid[96];
                if (seriesIUIDRoot.length() > 20) {
                    memcpy(prefix, seriesIUIDRoot.c_str(), 20);
                    prefix[20] = '\0';
                    dcmGenerateUniqueIdentifier(newuid, prefix);
                }
                else {
                    dcmGenerateUniqueIdentifier(newuid);
                }
                seriesIUIDs[series_number] = std::string(newuid);
            }

            // --------------------------------------------------

            ISMRMRD::MetaContainer attrib;
            Gadgetron::write_ismrmd_image_into_dicom(*m1->getObjectPtr(), *m2->getObjectPtr(), seriesIUIDs[series_number], dcmFile);

            // --------------------------------------------------
            /* release the old data array */
            m2->cont(NULL); // still need m3
            m1->release();

            GadgetContainerMessage<GadgetMessageIdentifier>* mb =
                new GadgetContainerMessage<GadgetMessageIdentifier>();
            mb->getObjectPtr()->id = GADGET_MESSAGE_DICOM_WITHNAME;

            //+PROTEUS: create socket address and attach to mb.
            GadgetContainerMessage<ACE_INET_Addr>* mbAddr = new GadgetContainerMessage<ACE_INET_Addr>();
            ACE_INET_Addr addr (port.value(), ip.value().c_str());
            *mbAddr->getObjectPtr() = addr;

            GadgetContainerMessage<DcmFileFormat>* mdcm = new GadgetContainerMessage<DcmFileFormat>();
            *mdcm->getObjectPtr() = dcmFile;

            mb->cont(mbAddr);
            mbAddr->cont(mdcm);
            //+PROTEUS: No filename or attribs container to attach

            int ret = this->send_message(mb);
            if ((ret < 0))
            {
                GDEBUG("Failed to return message to controller\n");
                return GADGET_FAIL;
            }
            return GADGET_OK;
        }

    private:
        ISMRMRD::IsmrmrdHeader xml;
        DcmFileFormat dcmFile;
        std::string seriesIUIDRoot;
        long initialSeriesNumber;
        std::map <unsigned int, std::string> seriesIUIDs;
    };

} /* namespace Gadgetron */

#endif // PROTEUSFINISHGADGET_H
