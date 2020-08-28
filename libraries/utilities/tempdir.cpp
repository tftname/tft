/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */

#include <dncio/utilities/tempdir.hpp>

#include <cstdlib>

namespace dncio { namespace utilities {

fc::path temp_directory_path()
{
   const char* dnc_tempdir = getenv("dnc_TEMPDIR");
   if( dnc_tempdir != nullptr )
      return fc::path( dnc_tempdir );
   return fc::temp_directory_path() / "dnc-tmp";
}

} } // dncio::utilities
