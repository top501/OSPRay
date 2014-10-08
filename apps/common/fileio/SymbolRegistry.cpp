//
//                 INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license agreement or
//    nondisclosure agreement with Intel Corporation and may not be copied
//    or disclosed except in accordance with the terms of that agreement.
//    Copyright (C) 2014 Intel Corporation. All Rights Reserved.
//

#include "apps/common/fileio/OSPObjectFile.h"
#include "apps/common/fileio/RawVolumeFile.h"

namespace ospray {

    //! Loader for XML object files.
    OSP_REGISTER_OBJECT_FILE(OSPObjectFile, osp);

    //! Loader for RAW volume files.
    OSP_REGISTER_VOLUME_FILE(RawVolumeFile, raw);

} // namespace ospray
