
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#ifndef FLARE_JSON2PB_JSON_TO_PB_H_
#define FLARE_JSON2PB_JSON_TO_PB_H_

#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream.h>    // ZeroCopyInputStream

namespace flare::json2pb {

    struct json_to_pb_options {
        json_to_pb_options();

        // Decode string in json using base64 decoding if the type of
        // corresponding field is bytes when this option is turned on.
        // Default: true.
        bool base64_to_bytes;
    };

    // Convert `json' to protobuf `message'.
    // Returns true on success. `error' (if not NULL) will be set with error
    // message on failure.
    bool json_to_proto_message(const std::string &json,
                            google::protobuf::Message *message,
                            const json_to_pb_options &options,
                            std::string *error = NULL);

// send output to ZeroCopyOutputStream instead of std::string.
    bool json_to_proto_message(google::protobuf::io::ZeroCopyInputStream *json,
                            google::protobuf::Message *message,
                            const json_to_pb_options &options,
                            std::string *error = NULL);

    // Using default json_to_pb_options.
    bool json_to_proto_message(const std::string &json,
                            google::protobuf::Message *message,
                            std::string *error = NULL);

    bool json_to_proto_message(google::protobuf::io::ZeroCopyInputStream *stream,
                            google::protobuf::Message *message,
                            std::string *error = NULL);
} // namespace flare::json2pb

#endif // FLARE_JSON2PB_JSON_TO_PB_H_
