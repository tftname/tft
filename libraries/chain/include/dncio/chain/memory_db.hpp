/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#pragma once
#include <dncio/chain/contract_table_objects.hpp>
#include <dncio/chain/controller.hpp>
#include <fc/utility.hpp>
#include <sstream>
#include <algorithm>
#include <set>

namespace chainbase { class database; }

namespace dncio { namespace chain {

class controller;

class memory_db {
   /// Constructor
   public:
      memory_db(controller& con)
      :db(con.db())
      {
      }
      memory_db(chainbase::database& db)
      :db(db)
      {
      }
   /// Database methods:
   public:
      int  db_store_i64( uint64_t code, uint64_t scope, uint64_t table, const account_name& payer, uint64_t id, const char* buffer, size_t buffer_size );

   private:

      const table_id_object* find_table( name code, name scope, name table );
      const table_id_object& find_or_create_table( name code, name scope, name table, const account_name &payer );
      void                   remove_table( const table_id_object& tid );

   /// Fields:
   public:
      chainbase::database&          db;  ///< database where state is stored
      struct account_info {
         account_name         name;
         asset                available;

         uint64_t primary_key()const { return name; }
      };

      struct bp_info {
         account_name     name;
         public_key_type  producer_key;
         uint32_t         commission_rate;
         int64_t          total_staked;
         asset            rewards_pool;
         int64_t          total_voteage;
         uint32_t         voteage_update_height;
         std::string      url;
         bool             emergency;

         uint64_t primary_key() const { return name; }
      };

      struct chain_status {
         account_name     name;
         bool             emergency;

         uint64_t primary_key() const { return name; }
      };
};
} } // namespace dncio::chain

FC_REFLECT(dncio::chain::memory_db::account_info, (name)(available))
FC_REFLECT(dncio::chain::memory_db::bp_info, (name)(producer_key)
  (commission_rate)(total_staked)(rewards_pool)(total_voteage)(voteage_update_height)(url)(emergency))
FC_REFLECT(dncio::chain::memory_db::chain_status, (name)(emergency))

