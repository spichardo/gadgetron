//Source code taken from https://github.com/gadgetron/gadgetron/blob/master/gadgets/dicom/DicomFinishGadget.cpp
//Hyperthermia edits found at +PROTEUS (ctrl+f to find indexes)
//Details: Uses proteus_utility to keep pixel data in float.
//         Removed unused conditionals for data types other than float.
//Edited by: Eric Song

#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"

#include "ProteusFinishGadget.h"
#include "ismrmrd/xml.h"

namespace Gadgetron {

    int ProteusFinishGadget::process_config(ACE_Message_Block* mb)
    {
        ISMRMRD::IsmrmrdHeader h;
        deserialize(mb->rd_ptr(), h);

        xml = h;

        Gadgetron::fill_dicom_image_from_ismrmrd_header(h, dcmFile);

        ISMRMRD::MeasurementInformation meas_info = *h.measurementInformation;

        if (meas_info.seriesInstanceUIDRoot) {
            seriesIUIDRoot = *meas_info.seriesInstanceUIDRoot;
        }

        if (meas_info.initialSeriesNumber) {
            this->initialSeriesNumber = (long)*meas_info.initialSeriesNumber;
        }
        else {
            this->initialSeriesNumber = 0;
        }

        return GADGET_OK;
    }

    int ProteusFinishGadget::process(GadgetContainerMessage<ISMRMRD::ImageHeader>* m1)
    {
        if (!this->controller_) {
            GERROR("Cannot return result to controller, no controller set\n");
            return GADGET_FAIL;
        }

        // --------------------------------------------------
        ISMRMRD::ImageHeader *img = m1->getObjectPtr();

        uint16_t data_type = img->data_type;

        if (data_type == ISMRMRD::ISMRMRD_FLOAT) //+PROTEUS: data_type is always float (with proteus.xml).
        {
            GadgetContainerMessage< hoNDArray< float > >* datamb = AsContainerMessage< hoNDArray< float > >(m1->cont());
            if (!datamb)
            {
                GERROR("ProteusFinishGadget::process, invalid image message objects\n");
                return GADGET_FAIL;
            }

            if (this->write_data_attrib(m1, datamb) != GADGET_OK)
            {
                GERROR("ProteusFinishGadget::write_data_attrib failed for float ... \n");
                return GADGET_FAIL;
            }
        }
        else {
          GERROR("ProteusFinishGadget::data_type is not Float\n");
        }

        return GADGET_OK;
    }

    GADGET_FACTORY_DECLARE(ProteusFinishGadget)

} /* namespace Gadgetron */
