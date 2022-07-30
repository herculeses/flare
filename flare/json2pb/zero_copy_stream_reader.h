
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#ifndef  FLARE_JSON2PB_ZERO_COPY_STREAM_READER_H_
#define  FLARE_JSON2PB_ZERO_COPY_STREAM_READER_H_

#include <google/protobuf/io/zero_copy_stream.h> // ZeroCopyInputStream

namespace flare::json2pb {

    class ZeroCopyStreamReader {
    public:
        typedef char Ch;

        ZeroCopyStreamReader(google::protobuf::io::ZeroCopyInputStream *stream)
                : _data(NULL), _data_size(0), _nread(0), _stream(stream) {
        }

        //Take a charactor and return its address.
        const char *PeekAddr() {
            if (!ReadBlockTail()) {
                return _data;
            }
            while (_stream->Next((const void **) &_data, &_data_size)) {
                if (!ReadBlockTail()) {
                    return _data;
                }
            }
            return NULL;
        }

        const char *TakeWithAddr() {
            const char *c = PeekAddr();
            if (c) {
                ++_nread;
                --_data_size;
                return _data++;
            }
            return NULL;
        }

        char Take() {
            const char *c = PeekAddr();
            if (c) {
                ++_nread;
                --_data_size;
                ++_data;
                return *c;
            }
            return '\0';
        }

        char Peek() {
            const char *c = PeekAddr();
            return (c ? *c : '\0');
        }

        //Tell whether read the end of this block.
        bool ReadBlockTail() {
            return !_data_size;
        }

        size_t Tell() { return _nread; }

        void Put(char) {}

        void Flush() {}

        char *PutBegin() { return NULL; }

        size_t PutEnd(char *) { return 0; }

    private:
        const char *_data;
        int _data_size;
        size_t _nread;
        google::protobuf::io::ZeroCopyInputStream *_stream;
    };

} // namespace flare::json2pb

#endif  // FLARE_JSON2PB_ZERO_COPY_STREAM_READER_H_
