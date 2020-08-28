/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#include <dnciolib/dncio.hpp>

#include "test_api.hpp"

void test_types::types_size() {

   dncio_assert( sizeof(int64_t) == 8, "int64_t size != 8");
   dncio_assert( sizeof(uint64_t) ==  8, "uint64_t size != 8");
   dncio_assert( sizeof(uint32_t) ==  4, "uint32_t size != 4");
   dncio_assert( sizeof(int32_t) ==  4, "int32_t size != 4");
   dncio_assert( sizeof(uint128_t) == 16, "uint128_t size != 16");
   dncio_assert( sizeof(int128_t) == 16, "int128_t size != 16");
   dncio_assert( sizeof(uint8_t) ==  1, "uint8_t size != 1");

   dncio_assert( sizeof(account_name) ==  8, "account_name size !=  8");
   dncio_assert( sizeof(table_name) ==  8, "table_name size !=  8");
   dncio_assert( sizeof(time) ==  4, "time size !=  4");
   dncio_assert( sizeof(dncio::key256) == 32, "key256 size != 32" );
}

void test_types::char_to_symbol() {

   dncio_assert( dncio::char_to_symbol('1') ==  1, "dncio::char_to_symbol('1') !=  1");
   dncio_assert( dncio::char_to_symbol('2') ==  2, "dncio::char_to_symbol('2') !=  2");
   dncio_assert( dncio::char_to_symbol('3') ==  3, "dncio::char_to_symbol('3') !=  3");
   dncio_assert( dncio::char_to_symbol('4') ==  4, "dncio::char_to_symbol('4') !=  4");
   dncio_assert( dncio::char_to_symbol('5') ==  5, "dncio::char_to_symbol('5') !=  5");
   dncio_assert( dncio::char_to_symbol('a') ==  6, "dncio::char_to_symbol('a') !=  6");
   dncio_assert( dncio::char_to_symbol('b') ==  7, "dncio::char_to_symbol('b') !=  7");
   dncio_assert( dncio::char_to_symbol('c') ==  8, "dncio::char_to_symbol('c') !=  8");
   dncio_assert( dncio::char_to_symbol('d') ==  9, "dncio::char_to_symbol('d') !=  9");
   dncio_assert( dncio::char_to_symbol('e') == 10, "dncio::char_to_symbol('e') != 10");
   dncio_assert( dncio::char_to_symbol('f') == 11, "dncio::char_to_symbol('f') != 11");
   dncio_assert( dncio::char_to_symbol('g') == 12, "dncio::char_to_symbol('g') != 12");
   dncio_assert( dncio::char_to_symbol('h') == 13, "dncio::char_to_symbol('h') != 13");
   dncio_assert( dncio::char_to_symbol('i') == 14, "dncio::char_to_symbol('i') != 14");
   dncio_assert( dncio::char_to_symbol('j') == 15, "dncio::char_to_symbol('j') != 15");
   dncio_assert( dncio::char_to_symbol('k') == 16, "dncio::char_to_symbol('k') != 16");
   dncio_assert( dncio::char_to_symbol('l') == 17, "dncio::char_to_symbol('l') != 17");
   dncio_assert( dncio::char_to_symbol('m') == 18, "dncio::char_to_symbol('m') != 18");
   dncio_assert( dncio::char_to_symbol('n') == 19, "dncio::char_to_symbol('n') != 19");
   dncio_assert( dncio::char_to_symbol('o') == 20, "dncio::char_to_symbol('o') != 20");
   dncio_assert( dncio::char_to_symbol('p') == 21, "dncio::char_to_symbol('p') != 21");
   dncio_assert( dncio::char_to_symbol('q') == 22, "dncio::char_to_symbol('q') != 22");
   dncio_assert( dncio::char_to_symbol('r') == 23, "dncio::char_to_symbol('r') != 23");
   dncio_assert( dncio::char_to_symbol('s') == 24, "dncio::char_to_symbol('s') != 24");
   dncio_assert( dncio::char_to_symbol('t') == 25, "dncio::char_to_symbol('t') != 25");
   dncio_assert( dncio::char_to_symbol('u') == 26, "dncio::char_to_symbol('u') != 26");
   dncio_assert( dncio::char_to_symbol('v') == 27, "dncio::char_to_symbol('v') != 27");
   dncio_assert( dncio::char_to_symbol('w') == 28, "dncio::char_to_symbol('w') != 28");
   dncio_assert( dncio::char_to_symbol('x') == 29, "dncio::char_to_symbol('x') != 29");
   dncio_assert( dncio::char_to_symbol('y') == 30, "dncio::char_to_symbol('y') != 30");
   dncio_assert( dncio::char_to_symbol('z') == 31, "dncio::char_to_symbol('z') != 31");

   for(unsigned char i = 0; i<255; i++) {
      if((i >= 'a' && i <= 'z') || (i >= '1' || i <= '5')) continue;
      dncio_assert( dncio::char_to_symbol((char)i) == 0, "dncio::char_to_symbol() != 0");
   }
}

void test_types::string_to_name() {

   dncio_assert( dncio::string_to_name("a") == N(a) , "dncio::string_to_name(a)" );
   dncio_assert( dncio::string_to_name("ba") == N(ba) , "dncio::string_to_name(ba)" );
   dncio_assert( dncio::string_to_name("cba") == N(cba) , "dncio::string_to_name(cba)" );
   dncio_assert( dncio::string_to_name("dcba") == N(dcba) , "dncio::string_to_name(dcba)" );
   dncio_assert( dncio::string_to_name("edcba") == N(edcba) , "dncio::string_to_name(edcba)" );
   dncio_assert( dncio::string_to_name("fedcba") == N(fedcba) , "dncio::string_to_name(fedcba)" );
   dncio_assert( dncio::string_to_name("gfedcba") == N(gfedcba) , "dncio::string_to_name(gfedcba)" );
   dncio_assert( dncio::string_to_name("hgfedcba") == N(hgfedcba) , "dncio::string_to_name(hgfedcba)" );
   dncio_assert( dncio::string_to_name("ihgfedcba") == N(ihgfedcba) , "dncio::string_to_name(ihgfedcba)" );
   dncio_assert( dncio::string_to_name("jihgfedcba") == N(jihgfedcba) , "dncio::string_to_name(jihgfedcba)" );
   dncio_assert( dncio::string_to_name("kjihgfedcba") == N(kjihgfedcba) , "dncio::string_to_name(kjihgfedcba)" );
   dncio_assert( dncio::string_to_name("lkjihgfedcba") == N(lkjihgfedcba) , "dncio::string_to_name(lkjihgfedcba)" );
   dncio_assert( dncio::string_to_name("mlkjihgfedcba") == N(mlkjihgfedcba) , "dncio::string_to_name(mlkjihgfedcba)" );
   dncio_assert( dncio::string_to_name("mlkjihgfedcba1") == N(mlkjihgfedcba2) , "dncio::string_to_name(mlkjihgfedcba2)" );
   dncio_assert( dncio::string_to_name("mlkjihgfedcba55") == N(mlkjihgfedcba14) , "dncio::string_to_name(mlkjihgfedcba14)" );

   dncio_assert( dncio::string_to_name("azAA34") == N(azBB34) , "dncio::string_to_name N(azBB34)" );
   dncio_assert( dncio::string_to_name("AZaz12Bc34") == N(AZaz12Bc34) , "dncio::string_to_name AZaz12Bc34" );
   dncio_assert( dncio::string_to_name("AAAAAAAAAAAAAAA") == dncio::string_to_name("BBBBBBBBBBBBBDDDDDFFFGG") , "dncio::string_to_name BBBBBBBBBBBBBDDDDDFFFGG" );
}

void test_types::name_class() {

   dncio_assert( dncio::name{dncio::string_to_name("azAA34")}.value == N(azAA34), "dncio::name != N(azAA34)" );
   dncio_assert( dncio::name{dncio::string_to_name("AABBCC")}.value == 0, "dncio::name != N(0)" );
   dncio_assert( dncio::name{dncio::string_to_name("AA11")}.value == N(AA11), "dncio::name != N(AA11)" );
   dncio_assert( dncio::name{dncio::string_to_name("11AA")}.value == N(11), "dncio::name != N(11)" );
   dncio_assert( dncio::name{dncio::string_to_name("22BBCCXXAA")}.value == N(22), "dncio::name != N(22)" );
   dncio_assert( dncio::name{dncio::string_to_name("AAAbbcccdd")} == dncio::name{dncio::string_to_name("AAAbbcccdd")}, "dncio::name == dncio::name" );

   uint64_t tmp = dncio::name{dncio::string_to_name("11bbcccdd")};
   dncio_assert(N(11bbcccdd) == tmp, "N(11bbcccdd) == tmp");
}
