
/****************************************************************
 * Copyright (c) 2022, liyinbin
 * All rights reserved.
 * Author by liyinbin (jeff.li) lijippy@163.com
 *****************************************************************/

#ifndef FLARE_BASE_SHA512_H_
#define FLARE_BASE_SHA512_H_

#include <cstdint>
#include <string>

namespace flare::base {

    /*!
     * SHA-512 processor without external dependencies.
     */
    class SHA512 {
      public:
        //! construct empty object.
        SHA512();

        //! construct context and process data range
        SHA512(const void *data, uint32_t size);

        //! construct context and process string
        explicit SHA512(const std::string &str);

        //! process more data
        void process(const void *data, uint32_t size);

        //! process more data
        void process(const std::string &str);

        //! digest length in bytes
        static constexpr size_t kDigestLength = 64;

        //! finalize computation and output 64 byte (512 bit) digest
        void finalize(void *digest);

        //! finalize computation and return 64 byte (512 bit) digest
        std::string digest();

        //! finalize computation and return 64 byte (512 bit) digest hex encoded
        std::string digest_hex();

        //! finalize computation and return 64 byte (512 bit) digest upper-case hex
        std::string digest_hex_uc();

      private:
        uint64_t _length;
        uint64_t _state[8];
        uint32_t _curlen;
        uint8_t _buf[128];
    };

    //! process data and return 64 byte (512 bit) digest hex encoded
    std::string sha512_hex(const void *data, uint32_t size);

    //! process data and return 64 byte (512 bit) digest hex encoded
    std::string sha512_hex(const std::string &str);

    //! process data and return 64 byte (512 bit) digest upper-case hex encoded
    std::string sha512_hex_uc(const void *data, uint32_t size);

    //! process data and return 64 byte (512 bit) digest upper-case hex encoded
    std::string sha512_hex_uc(const std::string &str);

}  // namespace flare::base

#endif  // FLARE_BASE_SHA512_H_
