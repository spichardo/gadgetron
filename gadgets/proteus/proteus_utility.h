/** \file   proteus_utility.h
    \brief  Implement some utility functions to convert ismrmrd image into dicom image
    \author Hui Xue
*/

#pragma once

#include <string>
#include <map>
#include <complex>

#include "Gadget.h"
#include "GadgetMRIHeaders.h"
#include "GadgetStreamController.h"

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstdinc.h"
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcostrmb.h"

#include "proteus_export.h"
#include "hoNDArray.h"
#include "hoNDImage.h"
#include "ismrmrd/meta.h"
#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"
#include "mri_core_def.h"

namespace Gadgetron
{
    // --------------------------------------------------------------------------
    /// fill dicom image from ismrmrd header
    // --------------------------------------------------------------------------
    EXPORTGADGETSPROTEUS void fill_dicom_image_from_ismrmrd_header(ISMRMRD::IsmrmrdHeader& h, DcmFileFormat& dcmFile);

    // --------------------------------------------------------------------------
    /// write a key and its value into dicom image
    // --------------------------------------------------------------------------
    EXPORTGADGETSPROTEUS void write_dcm_string(DcmDataset *dataset, DcmTagKey& key, const char* s);

    // --------------------------------------------------------------------------
    /// write ismrmrd image into a dcm image
    // --------------------------------------------------------------------------
    template<typename T> EXPORTGADGETSPROTEUS void write_ismrmd_image_into_dicom(ISMRMRD::ImageHeader& m1, hoNDArray<T>& m2, std::string& seriesIUID, DcmFileFormat& dcmFile);
    // with image attribute
    template<typename T> EXPORTGADGETSPROTEUS void write_ismrmd_image_into_dicom(ISMRMRD::ImageHeader& m1, hoNDArray<T>& m2, ISMRMRD::IsmrmrdHeader& h, ISMRMRD::MetaContainer& attrib, std::string& seriesIUID, DcmFileFormat& dcmFile);
}
