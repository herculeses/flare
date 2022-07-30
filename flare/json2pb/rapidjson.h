
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#ifndef  FLARE_JSON2PB_RAPIDJSON_H_
#define  FLARE_JSON2PB_RAPIDJSON_H_


#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#endif

#include "flare/rapidjson/allocators.h"
#include "flare/rapidjson/document.h"
#include "flare/rapidjson/encodedstream.h"
#include "flare/rapidjson/encodings.h"
#include "flare/rapidjson/filereadstream.h"
#include "flare/rapidjson/filewritestream.h"
#include "flare/rapidjson/prettywriter.h"
#include "flare/rapidjson/rapidjson.h"
#include "flare/rapidjson/reader.h"
#include "flare/rapidjson/stringbuffer.h"
#include "flare/rapidjson/writer.h"
#include "flare/rapidjson/optimized_writer.h"

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#pragma GCC diagnostic pop
#endif

#endif  // FLARE_JSON2PB_RAPIDJSON_H_
