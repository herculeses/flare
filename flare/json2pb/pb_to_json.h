
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#ifndef FLARE_JSON2PB_PB_TO_JSON_H_
#define FLARE_JSON2PB_PB_TO_JSON_H_

#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream.h> // ZeroCopyOutputStream

namespace flare::json2pb {

    enum enum_options {
        OUTPUT_ENUM_BY_NAME = 0,          // Output enum by its name
        OUTPUT_ENUM_BY_NUMBER = 1,        // Output enum by its value
    };

    struct proto_to_json_options {
        proto_to_json_options();

        // Control how enum fields are output
        // Default: OUTPUT_ENUM_BY_NAME
        enum_options enum_option;

        // Use rapidjson::PrettyWriter to generate the json when this option is on.
        // NOTE: currently PrettyWriter is not optimized yet thus the conversion
        // functions may be slower when this option is turned on.
        // Default: false
        bool pretty_json;

        // Convert "repeated { required string key = 1; required string value = 2; }"
        // to a map object of json and vice versa when this option is turned on.
        // Default: true
        bool enable_protobuf_map;

        // Encode the field of type bytes to string in json using base64
        // encoding when this option is turned on.
        // Default: true.
        bool bytes_to_base64;

        // Convert the repeated field that has no entry
        // to a empty array of json when this option is turned on.
        // Default: false
        bool jsonify_empty_array;

        // Whether to always print primitive fields. By default proto3 primitive
        // fields with default values will be omitted in JSON output. For example, an
        // int32 field set to 0 will be omitted. Set this flag to true will override
        // the default behavior and print primitive fields regardless of their values.
        bool always_print_primitive_fields;
    };

    // Convert protobuf `messge' to `json' according to `options'.
    // Returns true on success. `error' (if not NULL) will be set with error
    // message on failure.
    bool proto_message_to_json(const google::protobuf::Message &message,
                            std::string *json,
                            const proto_to_json_options &options,
                            std::string *error = NULL);

    // send output to ZeroCopyOutputStream instead of std::string.
    bool proto_message_to_json(const google::protobuf::Message &message,
                            google::protobuf::io::ZeroCopyOutputStream *json,
                            const proto_to_json_options &options,
                            std::string *error = NULL);

    // Using default proto_to_json_options.
    bool proto_message_to_json(const google::protobuf::Message &message,
                            std::string *json,
                            std::string *error = NULL);

    bool proto_message_to_json(const google::protobuf::Message &message,
                            google::protobuf::io::ZeroCopyOutputStream *json,
                            std::string *error = NULL);
} // namespace flare::json2pb

#endif // FLARE_JSON2PB_PB_TO_JSON_H_
