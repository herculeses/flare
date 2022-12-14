
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <time.h>
#include <typeinfo>
#include <limits>
#include <google/protobuf/descriptor.h>
#include "flare/strings/str_format.h"
#include "flare/strings/numbers.h"
#include "flare/json2pb/json_to_pb.h"
#include "flare/json2pb/zero_copy_stream_reader.h"       // ZeroCopyStreamReader
#include "flare/json2pb/encode_decode.h"
#include "flare/json2pb/protobuf_map.h"
#include "flare/json2pb/rapidjson.h"
#include "flare/base/base64.h"

#define J2PERROR(perr, fmt, ...)                                        \
    if (perr) {                                                         \
        if (!perr->empty()) {                                           \
            perr->append(", ", 2);                                      \
        }                                                               \
        flare::string_appendf(perr, fmt, ##__VA_ARGS__);                 \
    } else { }

namespace flare::json2pb {

    json_to_pb_options::json_to_pb_options()
            : base64_to_bytes(true) {
    }

    enum MatchType {
        TYPE_MATCH = 0x00,
        REQUIRED_OR_REPEATED_TYPE_MISMATCH = 0x01,
        OPTIONAL_TYPE_MISMATCH = 0x02
    };

    static void string_append_value(const RAPIDJSON_NAMESPACE::Value &value,
                                    std::string *output) {
        if (value.IsNull()) {
            output->append("null");
        } else if (value.IsBool()) {
            output->append(value.GetBool() ? "true" : "false");
        } else if (value.IsInt()) {
            flare::string_appendf(output, "%d", value.GetInt());
        } else if (value.IsUint()) {
            flare::string_appendf(output, "%u", value.GetUint());
        } else if (value.IsInt64()) {
            flare::string_appendf(output, "%" PRId64, value.GetInt64());
        } else if (value.IsUint64()) {
            flare::string_appendf(output, "%" PRIu64, value.GetUint64());
        } else if (value.IsDouble()) {
            flare::string_appendf(output, "%f", value.GetDouble());
        } else if (value.IsString()) {
            output->push_back('"');
            output->append(value.GetString(), value.GetStringLength());
            output->push_back('"');
        } else if (value.IsArray()) {
            output->append("array");
        } else if (value.IsObject()) {
            output->append("object");
        }
    }

    //It will be called when type mismatch occurs, fg: convert string to uint,
    //and will also be called when invalid value appears, fg: invalid enum name,
    //invalid enum number, invalid string content to convert to double or float.
    //for optional field error will just append error into error message
    //and ends with ',' and return true.
    //otherwise will append error into error message and return false.
    inline bool value_invalid(const google::protobuf::FieldDescriptor *field, const char *type,
                              const RAPIDJSON_NAMESPACE::Value &value, std::string *err) {
        bool optional = field->is_optional();
        if (err) {
            if (!err->empty()) {
                err->append(", ");
            }
            err->append("Invalid value `");
            string_append_value(value, err);
            flare::string_appendf(err, "' for %sfield `%s' which SHOULD be %s",
                                  optional ? "optional " : "",
                                  field->full_name().c_str(), type);
        }
        if (!optional) {
            return false;
        }
        return true;
    }

    template<typename T>
    inline bool convert_string_to_double_float_type(
            void (google::protobuf::Reflection::*func)(
                    google::protobuf::Message *message,
                    const google::protobuf::FieldDescriptor *field, T value) const,
            google::protobuf::Message *message,
            const google::protobuf::FieldDescriptor *field,
            const google::protobuf::Reflection *reflection,
            const RAPIDJSON_NAMESPACE::Value &item,
            std::string *err) {
        const char *limit_type = item.GetString();  // MUST be string here
        if (std::numeric_limits<T>::has_quiet_NaN &&
            strcasecmp(limit_type, "NaN") == 0) {
            (reflection->*func)(message, field, std::numeric_limits<T>::quiet_NaN());
            return true;
        }
        if (std::numeric_limits<T>::has_infinity &&
            strcasecmp(limit_type, "Infinity") == 0) {
            (reflection->*func)(message, field, std::numeric_limits<T>::infinity());
            return true;
        }
        if (std::numeric_limits<T>::has_infinity &&
            strcasecmp(limit_type, "-Infinity") == 0) {
            (reflection->*func)(message, field, -std::numeric_limits<T>::infinity());
            return true;
        }
        return value_invalid(field, typeid(T).name(), item, err);
    }

    inline bool convert_float_type(const RAPIDJSON_NAMESPACE::Value &item, bool repeated,
                                   google::protobuf::Message *message,
                                   const google::protobuf::FieldDescriptor *field,
                                   const google::protobuf::Reflection *reflection,
                                   std::string *err) {
        if (item.IsNumber()) {
            if (repeated) {
                reflection->AddFloat(message, field, item.GetDouble());
            } else {
                reflection->SetFloat(message, field, item.GetDouble());
            }
        } else if (item.IsString()) {
            if (!convert_string_to_double_float_type(
                    (repeated ? &google::protobuf::Reflection::AddFloat
                              : &google::protobuf::Reflection::SetFloat),
                    message, field, reflection, item, err)) {
                return false;
            }
        } else {
            return value_invalid(field, "float", item, err);
        }
        return true;
    }

    inline bool convert_double_type(const RAPIDJSON_NAMESPACE::Value &item, bool repeated,
                                    google::protobuf::Message *message,
                                    const google::protobuf::FieldDescriptor *field,
                                    const google::protobuf::Reflection *reflection,
                                    std::string *err) {
        if (item.IsNumber()) {
            if (repeated) {
                reflection->AddDouble(message, field, item.GetDouble());
            } else {
                reflection->SetDouble(message, field, item.GetDouble());
            }
        } else if (item.IsString()) {
            if (!convert_string_to_double_float_type(
                    (repeated ? &google::protobuf::Reflection::AddDouble
                              : &google::protobuf::Reflection::SetDouble),
                    message, field, reflection, item, err)) {
                return false;
            }
        } else {
            return value_invalid(field, "double", item, err);
        }
        return true;
    }

    inline bool convert_enum_type(const RAPIDJSON_NAMESPACE::Value &item, bool repeated,
                                  google::protobuf::Message *message,
                                  const google::protobuf::FieldDescriptor *field,
                                  const google::protobuf::Reflection *reflection,
                                  std::string *err) {
        const google::protobuf::EnumValueDescriptor *enum_value_descriptor = NULL;
        if (item.IsInt()) {
            enum_value_descriptor = field->enum_type()->FindValueByNumber(item.GetInt());
        } else if (item.IsString()) {
            enum_value_descriptor = field->enum_type()->FindValueByName(item.GetString());
        }
        if (!enum_value_descriptor) {
            return value_invalid(field, "enum", item, err);
        }
        if (repeated) {
            reflection->AddEnum(message, field, enum_value_descriptor);
        } else {
            reflection->SetEnum(message, field, enum_value_descriptor);
        }
        return true;
    }

    inline bool convert_int64_type(const RAPIDJSON_NAMESPACE::Value &item, bool repeated,
                                   google::protobuf::Message *message,
                                   const google::protobuf::FieldDescriptor *field,
                                   const google::protobuf::Reflection *reflection,
                                   std::string *err) {

        int64_t num;
        if (item.IsInt64()) {
            if (repeated) {
                reflection->AddInt64(message, field, item.GetInt64());
            } else {
                reflection->SetInt64(message, field, item.GetInt64());
            }
        } else if (item.IsString()) {
            int64_t r;
            if (!flare::simple_atoi(std::string_view(item.GetString(), item.GetStringLength()), &r)) {
                return value_invalid(field, "INT64", item, err);
            }
            num = r;
            if (repeated) {
                reflection->AddInt64(message, field, num);
            } else {
                reflection->SetInt64(message, field, num);
            }
        } else {
            return value_invalid(field, "INT64", item, err);
        }
        return true;
    }

    inline bool convert_uint64_type(const RAPIDJSON_NAMESPACE::Value &item,
                                    bool repeated,
                                    google::protobuf::Message *message,
                                    const google::protobuf::FieldDescriptor *field,
                                    const google::protobuf::Reflection *reflection,
                                    std::string *err) {
        uint64_t num;
        if (item.IsUint64()) {
            if (repeated) {
                reflection->AddUInt64(message, field, item.GetUint64());
            } else {
                reflection->SetUInt64(message, field, item.GetUint64());
            }
        } else if (item.IsString()) {
            int64_t r;
            if (!!flare::simple_atoi(std::string_view(item.GetString(), item.GetStringLength()), &r)) {
                return value_invalid(field, "INT64", item, err);
            }
            num = r;
            if (repeated) {
                reflection->AddUInt64(message, field, num);
            } else {
                reflection->SetUInt64(message, field, num);
            }
        } else {
            return value_invalid(field, "UINT64", item, err);
        }
        return true;
    }

    bool JsonValueToProtoMessage(const RAPIDJSON_NAMESPACE::Value &json_value,
                                 google::protobuf::Message *message,
                                 const json_to_pb_options &options, std::string *err);

//Json value to protobuf convert rules for type:
//Json value type                 Protobuf type                convert rules
//int                             int uint int64_t uint64        valid convert is available
//uint                            int uint int64_t uint64        valid convert is available
//int64_t                           int uint int64_t uint64        valid convert is available
//uint64                          int uint int64_t uint64        valid convert is available
//int uint int64_t uint64           float double                 available
//"NaN" "Infinity" "-Infinity"    float double                 only "NaN" "Infinity" "-Infinity" is available
//int                             enum                         valid enum number value is available
//string                          enum                         valid enum name value is available
//string                          int64_t uint64                 valid convert is available
//other mismatch type convertion will be regarded as error.
#define J2PCHECKTYPE(value, cpptype, jsontype) ({                   \
            MatchType match_type = TYPE_MATCH;                      \
            if (!value.Is##jsontype()) {                            \
                match_type = OPTIONAL_TYPE_MISMATCH;                \
                if (!value_invalid(field, #cpptype, value, err)) {  \
                    return false;                                   \
                }                                                   \
            }                                                       \
            match_type;                                             \
        })


    static bool JsonValueToProtoField(const RAPIDJSON_NAMESPACE::Value &value,
                                      const google::protobuf::FieldDescriptor *field,
                                      google::protobuf::Message *message,
                                      const json_to_pb_options &options,
                                      std::string *err) {
        if (value.IsNull()) {
            if (field->is_required()) {
                J2PERROR(err, "Missing required field: %s", field->full_name().c_str());
                return false;
            }
            return true;
        }

        if (field->is_repeated()) {
            if (!value.IsArray()) {
                J2PERROR(err, "Invalid value for repeated field: %s",
                         field->full_name().c_str());
                return false;
            }
        }

        const google::protobuf::Reflection *reflection = message->GetReflection();
        switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, method, jsontype)                      \
        case google::protobuf::FieldDescriptor::CPPTYPE_##cpptype: {                      \
            if (field->is_repeated()) {                                 \
                const RAPIDJSON_NAMESPACE::SizeType size = value.Size();          \
                for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) { \
                    const RAPIDJSON_NAMESPACE::Value & item = value[index];       \
                    if (TYPE_MATCH == J2PCHECKTYPE(item, cpptype, jsontype)) { \
                        reflection->Add##method(message, field, item.Get##jsontype()); \
                    }                                                   \
                }                                                       \
            } else if (TYPE_MATCH == J2PCHECKTYPE(value, cpptype, jsontype)) { \
                reflection->Set##method(message, field, value.Get##jsontype()); \
            }                                                           \
            break;                                                      \
        }                                                               \

            CASE_FIELD_TYPE(INT32, Int32, Int);
            CASE_FIELD_TYPE(UINT32, UInt32, Uint);
            CASE_FIELD_TYPE(BOOL, Bool, Bool);
#undef CASE_FIELD_TYPE

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size;
                         ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (!convert_int64_type(item, true, message, field, reflection,
                                                err)) {
                            return false;
                        }
                    }
                } else if (!convert_int64_type(value, false, message, field, reflection,
                                               err)) {
                    return false;
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size;
                         ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (!convert_uint64_type(item, true, message, field, reflection,
                                                 err)) {
                            return false;
                        }
                    }
                } else if (!convert_uint64_type(value, false, message, field, reflection,
                                                err)) {
                    return false;
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (!convert_float_type(item, true, message, field,
                                                reflection, err)) {
                            return false;
                        }
                    }
                } else if (!convert_float_type(value, false, message, field,
                                               reflection, err)) {
                    return false;
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (!convert_double_type(item, true, message, field,
                                                 reflection, err)) {
                            return false;
                        }
                    }
                } else if (!convert_double_type(value, false, message, field,
                                                reflection, err)) {
                    return false;
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (TYPE_MATCH == J2PCHECKTYPE(item, string, String)) {
                            std::string str(item.GetString(), item.GetStringLength());
                            if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES &&
                                options.base64_to_bytes) {
                                std::string str_decoded;
                                if (!flare::base::base64_decode(str, &str_decoded)) {
                                    J2PERROR(err, "Fail to decode base64 string=%s", str.c_str());
                                    return false;
                                }
                                str = str_decoded;
                            }
                            reflection->AddString(message, field, str);
                        }
                    }
                } else if (TYPE_MATCH == J2PCHECKTYPE(value, string, String)) {
                    std::string str(value.GetString(), value.GetStringLength());
                    if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES &&
                        options.base64_to_bytes) {
                        std::string str_decoded;
                        if (!flare::base::base64_decode(str, &str_decoded)) {
                            J2PERROR(err, "Fail to decode base64 string=%s", str.c_str());
                            return false;
                        }
                        str = str_decoded;
                    }
                    reflection->SetString(message, field, str);
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (!convert_enum_type(item, true, message, field,
                                               reflection, err)) {
                            return false;
                        }
                    }
                } else if (!convert_enum_type(value, false, message, field,
                                              reflection, err)) {
                    return false;
                }
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                if (field->is_repeated()) {
                    const RAPIDJSON_NAMESPACE::SizeType size = value.Size();
                    for (RAPIDJSON_NAMESPACE::SizeType index = 0; index < size; ++index) {
                        const RAPIDJSON_NAMESPACE::Value &item = value[index];
                        if (TYPE_MATCH == J2PCHECKTYPE(item, message, Object)) {
                            if (!JsonValueToProtoMessage(
                                    item, reflection->AddMessage(message, field), options, err)) {
                                return false;
                            }
                        }
                    }
                } else if (!JsonValueToProtoMessage(
                        value, reflection->MutableMessage(message, field), options, err)) {
                    return false;
                }
                break;
        }
        return true;
    }

    bool JsonMapToProtoMap(const RAPIDJSON_NAMESPACE::Value &value,
                           const google::protobuf::FieldDescriptor *map_desc,
                           google::protobuf::Message *message,
                           const json_to_pb_options &options,
                           std::string *err) {
        if (!value.IsObject()) {
            J2PERROR(err, "Non-object value for map field: %s",
                     map_desc->full_name().c_str());
            return false;
        }

        const google::protobuf::Reflection *reflection = message->GetReflection();
        const google::protobuf::FieldDescriptor *key_desc =
                map_desc->message_type()->FindFieldByName(KEY_NAME);
        const google::protobuf::FieldDescriptor *value_desc =
                map_desc->message_type()->FindFieldByName(VALUE_NAME);

        for (RAPIDJSON_NAMESPACE::Value::ConstMemberIterator it =
                value.MemberBegin(); it != value.MemberEnd(); ++it) {
            google::protobuf::Message *entry = reflection->AddMessage(message, map_desc);
            const google::protobuf::Reflection *entry_reflection = entry->GetReflection();
            entry_reflection->SetString(
                    entry, key_desc, std::string(it->name.GetString(),
                                                 it->name.GetStringLength()));
            if (!JsonValueToProtoField(it->value, value_desc, entry, options, err)) {
                return false;
            }
        }
        return true;
    }

    bool JsonValueToProtoMessage(const RAPIDJSON_NAMESPACE::Value &json_value,
                                 google::protobuf::Message *message,
                                 const json_to_pb_options &options,
                                 std::string *err) {
        const google::protobuf::Descriptor *descriptor = message->GetDescriptor();
        if (!json_value.IsObject()) {
            J2PERROR(err, "`json_value' is not a json object. %s", descriptor->name().c_str());
            return false;
        }

        const google::protobuf::Reflection *reflection = message->GetReflection();

        std::vector<const google::protobuf::FieldDescriptor *> fields;
        fields.reserve(64);
        for (int i = 0; i < descriptor->extension_range_count(); ++i) {
            const google::protobuf::Descriptor::ExtensionRange *
                    ext_range = descriptor->extension_range(i);
            for (int tag_number = ext_range->start; tag_number < ext_range->end;
                 ++tag_number) {
                const google::protobuf::FieldDescriptor *field =
                        reflection->FindKnownExtensionByNumber(tag_number);
                if (field) {
                    fields.push_back(field);
                }
            }
        }
        for (int i = 0; i < descriptor->field_count(); ++i) {
            fields.push_back(descriptor->field(i));
        }

        std::string field_name_str_temp;
        const RAPIDJSON_NAMESPACE::Value *value_ptr = NULL;
        for (size_t i = 0; i < fields.size(); ++i) {
            const google::protobuf::FieldDescriptor *field = fields[i];

            const std::string &orig_name = field->name();
            bool res = decode_name(orig_name, field_name_str_temp);
            const std::string &field_name_str = (res ? field_name_str_temp : orig_name);

#ifndef RAPIDJSON_VERSION_0_1
            RAPIDJSON_NAMESPACE::Value::ConstMemberIterator member =
                    json_value.FindMember(field_name_str.data());
            if (member == json_value.MemberEnd()) {
                if (field->is_required()) {
                    J2PERROR(err, "Missing required field: %s", field->full_name().c_str());
                    return false;
                }
                continue;
            }
            value_ptr = &(member->value);
#else
            const RAPIDJSON_NAMESPACE::Value::Member* member =
                    json_value.FindMember(field_name_str.data());
            if (member == NULL) {
                if (field->is_required()) {
                    J2PERROR(err, "Missing required field: %s", field->full_name().c_str());
                    return false;
                }
                continue;
            }
            value_ptr = &(member->value);
#endif

            if (IsProtobufMap(field) && value_ptr->IsObject()) {
                // Try to parse json like {"key":value, ...} into protobuf map
                if (!JsonMapToProtoMap(*value_ptr, field, message, options, err)) {
                    return false;
                }
            } else {
                if (!JsonValueToProtoField(*value_ptr, field, message, options, err)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool ZeroCopyStreamToJson(RAPIDJSON_NAMESPACE::Document *dest,
                              google::protobuf::io::ZeroCopyInputStream *stream) {
        ZeroCopyStreamReader stream_reader(stream);
        dest->ParseStream<0, RAPIDJSON_NAMESPACE::UTF8<> >(stream_reader);
        return !dest->HasParseError();
    }

    inline bool JsonToProtoMessageInline(const std::string &json_string,
                                         google::protobuf::Message *message,
                                         const json_to_pb_options &options,
                                         std::string *error) {
        if (error) {
            error->clear();
        }
        RAPIDJSON_NAMESPACE::Document d;
        d.Parse<0>(json_string.c_str());
        if (d.HasParseError()) {
            J2PERROR(error, "Invalid json format");
            return false;
        }
        return json2pb::JsonValueToProtoMessage(d, message, options, error);
    }

    bool json_to_proto_message(const std::string &json_string,
                            google::protobuf::Message *message,
                            const json_to_pb_options &options,
                            std::string *error) {
        return JsonToProtoMessageInline(json_string, message, options, error);
    }

    bool json_to_proto_message(google::protobuf::io::ZeroCopyInputStream *stream,
                            google::protobuf::Message *message,
                            const json_to_pb_options &options,
                            std::string *error) {
        if (error) {
            error->clear();
        }
        RAPIDJSON_NAMESPACE::Document d;
        if (!json2pb::ZeroCopyStreamToJson(&d, stream)) {
            J2PERROR(error, "Invalid json format");
            return false;
        }
        return json2pb::JsonValueToProtoMessage(d, message, options, error);
    }

    bool json_to_proto_message(const std::string &json_string,
                            google::protobuf::Message *message,
                            std::string *error) {
        return JsonToProtoMessageInline(json_string, message, json_to_pb_options(), error);
    }

    bool json_to_proto_message(std::string json_string,
                            google::protobuf::Message *message,
                            std::string *error) {
        return JsonToProtoMessageInline(json_string, message, json_to_pb_options(), error);
    }

    bool json_to_proto_message(google::protobuf::io::ZeroCopyInputStream *stream,
                            google::protobuf::Message *message,
                            std::string *error) {
        if (error) {
            error->clear();
        }
        RAPIDJSON_NAMESPACE::Document d;
        if (!json2pb::ZeroCopyStreamToJson(&d, stream)) {
            J2PERROR(error, "Invalid json format");
            return false;
        }
        return json2pb::JsonValueToProtoMessage(d, message, json_to_pb_options(), error);
    }
} //namespace flare::json2pb

#undef J2PERROR
#undef J2PCHECKTYPE
