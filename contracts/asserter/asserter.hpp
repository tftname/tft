/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */

#include <dnciolib/dncio.hpp>

namespace asserter {
   struct assertdef {
      int8_t      condition;
      std::string message;

      dncLIB_SERIALIZE( assertdef, (condition)(message) )
   };
}
