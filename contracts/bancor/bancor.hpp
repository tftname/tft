/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#pragma once

#include <dnciolib/dncio.hpp>
#include <dnciolib/token.hpp>
#include <dnciolib/reflect.hpp>
#include <dnciolib/generic_currency.hpp>

#include <bancor/converter.hpp>
#include <currency/currency.hpp>

namespace bancor {
   typedef dncio::generic_currency< dncio::token<N(other),S(4,OTHER)> >  other_currency;
   typedef dncio::generic_currency< dncio::token<N(bancor),S(4,RELAY)> > relay_currency;
   typedef dncio::generic_currency< dncio::token<N(currency),S(4,CUR)> > cur_currency;

   typedef converter<relay_currency, other_currency, cur_currency > example_converter;
} /// bancor

