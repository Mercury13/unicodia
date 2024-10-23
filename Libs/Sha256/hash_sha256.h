///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright Iliass Mahjoub 2022 - 2023.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
//                                                               //
///////////////////////////////////////////////////////////////////

#ifndef HASH_SHA256_2022_06_02_H
  #define HASH_SHA256_2022_06_02_H

  #include <algorithm>
  #include <array>
  #include <cstdint>

  using sha256_type = std::array<std::uint8_t, 32U>;

  class hash_sha256
  {
  public:
    hash_sha256()                   = default;
    hash_sha256(const hash_sha256&) = delete;
    hash_sha256(hash_sha256&&)      = delete;
    virtual ~hash_sha256()          = default; // LCOV_EXCL_LINE

    auto operator=(hash_sha256&&)      -> hash_sha256& = delete;
    auto operator=(const hash_sha256&) -> hash_sha256& = delete;

    auto sha256_init() -> void
    {
      datalen = 0U;
      bitlen  = 0U;

      init_hash_val[0U] = UINT32_C(0x6A09E667);
      init_hash_val[1U] = UINT32_C(0xBB67AE85);
      init_hash_val[2U] = UINT32_C(0x3C6EF372);
      init_hash_val[3U] = UINT32_C(0xA54FF53A);
      init_hash_val[4U] = UINT32_C(0x510E527F);
      init_hash_val[5U] = UINT32_C(0x9B05688C);
      init_hash_val[6U] = UINT32_C(0x1F83D9AB);
      init_hash_val[7U] = UINT32_C(0x5BE0CD19);
    }

    auto sha256_update(const std::uint8_t* msg, const size_t length) -> void
    {
      for (std::size_t i = 0U; i < length; ++i)
      {
        data[datalen] = msg[i];
        datalen++;

        if(datalen == 64U)
        {
          sha256_transform();
          datalen = 0U;
          bitlen += 512U;
        }
      }
    }

    auto sha256_final() -> sha256_type
    {
      std::size_t i = 0U;
      sha256_type hash_result = {0U};
      i = datalen;

      // Pad whatever data is left in the buffer.
      if(datalen < 56U)
      {
        data[i++] = 0x80U;
        std::fill((data.begin() + i), (data.begin() + 56U), 0U);
      }

      else
      {
        data[i++] = 0x80U;
        std::fill((data.begin() + i), data.end(), 0U);
        sha256_transform();
        std::fill_n(data.begin(), 56U, 0U);
      }

      // Append to the padding the total message's length in bits and transform.
      bitlen   += static_cast<std::uint64_t>(datalen * UINT8_C(8));

      data[63U] = static_cast<std::uint8_t>(bitlen >> UINT8_C( 0));
      data[62U] = static_cast<std::uint8_t>(bitlen >> UINT8_C( 8));
      data[61U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(16));
      data[60U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(24));
      data[59U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(32));
      data[58U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(40));
      data[57U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(48));
      data[56U] = static_cast<std::uint8_t>(bitlen >> UINT8_C(56));

      sha256_transform();

      // Since this implementation uses little endian byte ordering and SHA uses big endian,
      // reverse all the bytes when copying the final init_hash_val to the output hash.
      for(std::size_t j = 0U; j < 4U; ++j)
      {
        hash_result[j +  0U] = ((init_hash_val[0U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j +  4U] = ((init_hash_val[1U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j +  8U] = ((init_hash_val[2U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j + 12U] = ((init_hash_val[3U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j + 16U] = ((init_hash_val[4U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j + 20U] = ((init_hash_val[5U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j + 24U] = ((init_hash_val[6U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
        hash_result[j + 28U] = ((init_hash_val[7U] >> (24U - (j * 8U))) & UINT32_C(0x000000FF));
      }

      return hash_result;
    }

  private:
    std::uint32_t datalen;
    std::uint64_t bitlen;
    std::array<std::uint8_t, 64U> data;
    std::array<std::uint32_t, 8U> init_hash_val;

    static constexpr std::array<std::uint32_t, 64U> K =
    {
      UINT32_C(0x428A2F98), UINT32_C(0x71374491), UINT32_C(0xB5C0FBCF), UINT32_C(0xE9B5DBA5),
      UINT32_C(0x3956C25B), UINT32_C(0x59F111F1), UINT32_C(0x923F82A4), UINT32_C(0xAB1C5ED5),
      UINT32_C(0xD807AA98), UINT32_C(0x12835B01), UINT32_C(0x243185BE), UINT32_C(0x550C7DC3),
      UINT32_C(0x72BE5D74), UINT32_C(0x80DEB1FE), UINT32_C(0x9BDC06A7), UINT32_C(0xC19BF174),
      UINT32_C(0xE49B69C1), UINT32_C(0xEFBE4786), UINT32_C(0x0FC19DC6), UINT32_C(0x240CA1CC),
      UINT32_C(0x2DE92C6F), UINT32_C(0x4A7484AA), UINT32_C(0x5CB0A9DC), UINT32_C(0x76F988DA),
      UINT32_C(0x983E5152), UINT32_C(0xA831C66D), UINT32_C(0xB00327C8), UINT32_C(0xBF597FC7),
      UINT32_C(0xC6E00BF3), UINT32_C(0xD5A79147), UINT32_C(0x06CA6351), UINT32_C(0x14292967),
      UINT32_C(0x27B70A85), UINT32_C(0x2E1B2138), UINT32_C(0x4D2C6DFC), UINT32_C(0x53380D13),
      UINT32_C(0x650A7354), UINT32_C(0x766A0ABB), UINT32_C(0x81C2C92E), UINT32_C(0x92722C85),
      UINT32_C(0xA2BFE8A1), UINT32_C(0xA81A664B), UINT32_C(0xC24B8B70), UINT32_C(0xC76C51A3),
      UINT32_C(0xD192E819), UINT32_C(0xD6990624), UINT32_C(0xF40E3585), UINT32_C(0x106AA070),
      UINT32_C(0x19A4C116), UINT32_C(0x1E376C08), UINT32_C(0x2748774C), UINT32_C(0x34B0BCB5),
      UINT32_C(0x391C0CB3), UINT32_C(0x4ED8AA4A), UINT32_C(0x5B9CCA4F), UINT32_C(0x682E6FF3),
      UINT32_C(0x748F82EE), UINT32_C(0x78A5636F), UINT32_C(0x84C87814), UINT32_C(0x8CC70208),
      UINT32_C(0x90BEFFFA), UINT32_C(0xA4506CEB), UINT32_C(0xBEF9A3F7), UINT32_C(0xC67178F2)
    };

    auto sha256_transform() -> void
    {
      std::uint32_t tmp1 = 0U;
      std::uint32_t tmp2 = 0U;

      std::array<std::uint32_t, 8U> state = {0U};
      std::array<std::uint32_t, 64U> m    = {0U};

      for(std::size_t i = 0U, j = 0U; i < 16U; ++i, j += 4U)
      {
        m[i] = static_cast<std::uint32_t>
        (
            static_cast<std::uint32_t>(static_cast<std::uint32_t>(data[j + 0U]) << 24U)
          | static_cast<std::uint32_t>(static_cast<std::uint32_t>(data[j + 1U]) << 16U)
          | static_cast<std::uint32_t>(static_cast<std::uint32_t>(data[j + 2U]) <<  8U)
          | static_cast<std::uint32_t>(static_cast<std::uint32_t>(data[j + 3U]) <<  0U)
        );
      }

      for(std::size_t i = 16U ; i < 64U; ++i)
      {
        m[i] = ssig1(m[i - 2U]) + m[i - 7U] + ssig0(m[i - 15U]) + m[i - 16U];
      }

      std::copy(init_hash_val.begin(), init_hash_val.end() , state.begin());

      for(std::size_t i = 0U; i < 64U; ++i)
      {
        tmp1 = state[7U] + bsig1(state[4U]) + ch(state[4U], state[5U], state[6U]) + K[i] + m[i];

        tmp2 = bsig0(state[0U]) + maj(state[0U], state[1U], state[2U]);

        state[7U] = state[6U];
        state[6U] = state[5U];
        state[5U] = state[4U];
        state[4U] = state[3U] + tmp1;
        state[3U] = state[2U];
        state[2U] = state[1U];
        state[1U] = state[0U];
        state[0U] = tmp1 + tmp2;
      }

      init_hash_val[0U] += state[0U];
      init_hash_val[1U] += state[1U];
      init_hash_val[2U] += state[2U];
      init_hash_val[3U] += state[3U];
      init_hash_val[4U] += state[4U];
      init_hash_val[5U] += state[5U];
      init_hash_val[6U] += state[6U];
      init_hash_val[7U] += state[7U];
    }

    // circular left shift ROTR^n(x)
    static inline auto rotl(std::uint32_t a, std::uint32_t b) -> std::uint32_t
    {
      return (static_cast<std::uint32_t>(a << b) | static_cast<std::uint32_t>(a >> (32U - b)));
    }

    // circular right shift ROTR^n(x)
    static inline auto rotr(std::uint32_t a, std::uint32_t b) -> std::uint32_t
    {
      return (static_cast<std::uint32_t>(a >> b) | static_cast<std::uint32_t>(a << (32U - b)));
    }

    static inline auto ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t
    {
      return (static_cast<std::uint32_t>(x & y) ^ static_cast<std::uint32_t>(~x & z));
    }

    static inline auto maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) -> std::uint32_t
    {
      return (static_cast<std::uint32_t>(x & y) ^ static_cast<std::uint32_t>(x & z) ^ static_cast<std::uint32_t>(y & z));
    }

    static inline auto bsig0(std::uint32_t x) -> std::uint32_t
    {
      return (rotr(x, 2U) ^ rotr(x, 13U) ^ rotr(x, 22U));
    }

    static inline auto bsig1(std::uint32_t x) -> std::uint32_t
    {
      return (rotr(x, 6U) ^ rotr(x, 11U) ^ rotr(x, 25U));
    }

    static inline auto ssig0(std::uint32_t x) -> std::uint32_t
    {
      return (rotr(x, 7U) ^ rotr(x, 18U) ^ (x >> 3U));
    }

    static inline auto ssig1(std::uint32_t x) -> std::uint32_t
    {
      return (rotr(x, 17U) ^ rotr(x, 19U) ^ (x >> 10U));
    }
  };
#endif // HASH_SHA256_2022_06_02_H
