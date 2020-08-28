/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#pragma once

#include <dncio/chain/types.hpp>
#include <dncio/chain/asset.hpp>

namespace dncio { namespace chain {

   class controller;

   class txfee_manager {
      public:

         explicit txfee_manager();

         bool check_transaction( const transaction& trx)const;

         asset get_required_fee( const transaction& trx)const;


      private:
        std::map<action_name, asset> fee_map;
   };
   class fee_paramter {
     public:
       account_name name;
       asset fee;
       account_name producer;
       fee_paramter(account_name name, asset fee, account_name producer) : name(name), fee(fee), producer(producer) {};
    };
} } /// namespace dncio::chain

FC_REFLECT(dncio::chain::fee_paramter, (name)(fee)(producer))
