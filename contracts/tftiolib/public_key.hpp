#pragma once 
#include <dnciolib/varint.hpp>
#include <dnciolib/serialize.hpp>

namespace dncio {
   struct public_key {
      unsigned_int        type;
      std::array<char,33> data;

      friend bool operator == ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) == std::tie(b.type,b.data);
      }
      friend bool operator != ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) != std::tie(b.type,b.data);
      }
      dncLIB_SERIALIZE( public_key, (type)(data) )
   };
}
