/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#include <dncio/chain/dncio_contract.hpp>
#include <dncio/chain/contract_table_objects.hpp>

#include <dncio/chain/controller.hpp>
#include <dncio/chain/transaction_context.hpp>
#include <dncio/chain/apply_context.hpp>
#include <dncio/chain/transaction.hpp>
#include <dncio/chain/exceptions.hpp>
#include <dncio/chain/memory_db.hpp>

#include <dncio/chain/account_object.hpp>
#include <dncio/chain/permission_object.hpp>
#include <dncio/chain/permission_link_object.hpp>
#include <dncio/chain/global_property_object.hpp>
#include <dncio/chain/contract_types.hpp>
#include <dncio/chain/producer_object.hpp>

#include <dncio/chain/wasm_interface.hpp>
#include <dncio/chain/abi_serializer.hpp>

#include <dncio/chain/authorization_manager.hpp>
#include <dncio/chain/resource_limits.hpp>
// #include <dncio/chain/contract_table_objects.hpp>

namespace dncio { namespace chain {



uint128_t transaction_id_to_sender_id( const transaction_id_type& tid ) {
   fc::uint128_t _id(tid._hash[3], tid._hash[2]);
   return (unsigned __int128)_id;
}

void validate_authority_precondition( const apply_context& context, const authority& auth ) {
   for(const auto& a : auth.accounts) {
      auto* acct = context.db.find<account_object, by_name>(a.permission.actor);
      dnc_ASSERT( acct != nullptr, action_validate_exception,
                  "account '${account}' does not exist",
                  ("account", a.permission.actor)
                );

      if( a.permission.permission == config::owner_name || a.permission.permission == config::active_name )
         continue; // account was already checked to exist, so its owner and active permissions should exist

      if( a.permission.permission == config::dncio_code_name ) // virtual dncio.code permission does not really exist but is allowed
         continue;

      try {
         context.control.get_authorization_manager().get_permission({a.permission.actor, a.permission.permission});
      } catch( const permission_query_exception& ) {
         dnc_THROW( action_validate_exception,
                    "permission '${perm}' does not exist",
                    ("perm", a.permission)
                  );
      }
   }
}


  void accounts_table(account_name name, chainbase::database& cdb) {
      memory_db::account_info obj;
      obj.name = name;
      obj.available = asset(0);
      bytes data = fc::raw::pack(obj);
      auto pk = obj.primary_key();
      memory_db db(cdb);
      db.db_store_i64(N(dncio), N(dncio), N(accounts), name, pk, data.data(), data.size() );
   }

/**
 *  This method is called assuming precondition_system_newaccount succeeds a
 */
void apply_dncio_newaccount(apply_context& context) {
   auto create = context.act.data_as<newaccount>();
   try {
   context.require_authorization(create.creator);
//   context.require_write_lock( config::dncio_auth_scope );
   auto& authorization = context.control.get_mutable_authorization_manager();

   dnc_ASSERT( validate(create.owner), action_validate_exception, "Invalid owner authority");
   dnc_ASSERT( validate(create.active), action_validate_exception, "Invalid active authority");

   auto& db = context.db;

   auto name_str = name(create.name).to_string();

   dnc_ASSERT( !create.name.empty(), action_validate_exception, "account name cannot be empty" );
   dnc_ASSERT( name_str.size() <= 12, action_validate_exception, "account names can only be 12 chars long" );

   // Check if the creator is privileged
   const auto &creator = db.get<account_object, by_name>(create.creator);
   dnc_ASSERT(!creator.privileged, action_validate_exception, "not support privileged accounts");
   /*if( !creator.privileged ) {
      dnc_ASSERT( name_str.find( "dncio." ) != 0, action_validate_exception,
                  "only privileged accounts can have names that start with 'dncio.'" );
   }*/

   auto existing_account = db.find<account_object, by_name>(create.name);
   dnc_ASSERT(existing_account == nullptr, action_validate_exception,
              "Cannot create account named ${name}, as that name is already taken",
              ("name", create.name));

   const auto& new_account = db.create<account_object>([&](auto& a) {
      a.name = create.name;
      a.creation_date = context.control.pending_block_time();
   });

   db.create<account_sequence_object>([&](auto& a) {
      a.name = create.name;
   });
   accounts_table(create.name, db);

   for( const auto& auth : { create.owner, create.active } ){
      validate_authority_precondition( context, auth );
   }

   const auto& owner_permission  = authorization.create_permission( create.name, config::owner_name, 0,
                                                                    std::move(create.owner) );
   const auto& active_permission = authorization.create_permission( create.name, config::active_name, owner_permission.id,
                                                                    std::move(create.active) );

   context.control.get_mutable_resource_limits_manager().initialize_account(create.name);

   int64_t ram_delta = config::overhead_per_account_ram_bytes;
   ram_delta += 2*config::billable_size_v<permission_object>;
   ram_delta += owner_permission.auth.get_billable_size();
   ram_delta += active_permission.auth.get_billable_size();

   context.trx_context.add_ram_usage(create.name, ram_delta);

} FC_CAPTURE_AND_RETHROW( (create) ) }

static void copy_inline_row(const chain::key_value_object& obj, vector<char>& data) {
  data.resize( obj.value.size() );
  memcpy( data.data(), obj.value.data(), obj.value.size() );
}

// config::system_account_name

bool allow_setcode(apply_context& context, std::string code_id) {
  const auto& code_account = context.db.get<account_object,by_name>(N(dncio));
  chain::abi_def abi;
  if(abi_serializer::to_abi(code_account.abi, abi)) {
    abi_serializer abis(abi);
    const auto* t_id = context.db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple(config::system_account_name, config::system_account_name, N(contractstat)));
    if (t_id != nullptr) {
      const auto &idx = context.db.get_index<key_value_index, by_scope_primary>();
      auto it = idx.lower_bound(boost::make_tuple(t_id->id, config::system_account_name));
      if (it != idx.end()) {
        vector<char> data;
        copy_inline_row(*it, data);
        auto ct = abis.binary_to_variant("contractstat_info", data);
        auto& obj = ct.get_object();
        auto code_obj = obj["code"].get_object();
        auto cid = code_obj["code_id"].as_string();
        if (cid == code_id) {
          return true;
        } else {
          return false;
        }
      }
    }
  }
  return false;
}

bool allow_setabi(apply_context& context, std::string abi_id) {
  const auto& code_account = context.db.get<account_object,by_name>(N(dncio));
  chain::abi_def abi;
  if(abi_serializer::to_abi(code_account.abi, abi)) {
    abi_serializer abis(abi);
    const auto* t_id = context.db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple(config::system_account_name, config::system_account_name, N(contractstat)));
    if (t_id != nullptr) {
      const auto &idx = context.db.get_index<key_value_index, by_scope_primary>();
      auto it = idx.lower_bound(boost::make_tuple(t_id->id, config::system_account_name));
      if (it != idx.end()) {
        vector<char> data;
        copy_inline_row(*it, data);
        auto ct = abis.binary_to_variant("contractstat_info", data);
        auto& obj = ct.get_object();
        auto code_obj = obj["code"].get_object();
        auto aid = code_obj["abi_id"].as_string();
        if (aid == abi_id) {
          return true;
        } else {
          return false;
        }
      }
    }
  }
  return false;
}

void apply_dncio_setcode(apply_context& context) {
   const auto& cfg = context.control.get_global_properties().configuration;

   auto& db = context.db;

   auto  act = context.act.data_as<setcode>();
   context.setcode_require_authorization(act.account);
//   context.require_write_lock( config::dncio_auth_scope );

   FC_ASSERT( act.vmtype == 0 );
   FC_ASSERT( act.vmversion == 0 );

   fc::sha256 code_id; /// default ID == 0

   if( act.code.size() > 0 ) {
     code_id = fc::sha256::hash( act.code.data(), (uint32_t)act.code.size() );
     wasm_interface::validate(act.code);
   }

   const auto& account = db.get<account_object,by_name>(act.account);

   int64_t code_size = (int64_t)act.code.size();
   int64_t old_size  = (int64_t)account.code.size() * config::setcode_ram_bytes_multiplier;
   int64_t new_size  = code_size * config::setcode_ram_bytes_multiplier;

   // Only allow dncio contract to setcode
   if (act.account != dncio::chain::name{N(dncio)}) {
     // exit
     FC_THROW("only allow dncio to setcode");
   }

   // Not first time setcode
   if (account.code_version != fc::sha256::sha256()) {
     // get allow_setcode from system contract table
     if (!allow_setcode(context, code_id.str())) {
       // exit
       FC_THROW("The code_id '${code_id}' is not approved by the system contract", ("code_id", code_id));
     }
     // FC_THROW("setcode twice is not allowed");
   }

   FC_ASSERT( account.code_version != code_id, "contract is already running this version of code" );

   db.modify( account, [&]( auto& a ) {
      /** TODO: consider whether a microsecond level local timestamp is sufficient to detect code version changes*/
      // TODO: update setcode message to include the hash, then validate it in validate
      a.last_code_update = context.control.pending_block_time();
      a.code_version = code_id;
      a.code.resize( code_size );
      if( code_size > 0 )
         memcpy( a.code.data(), act.code.data(), code_size );

   });

   const auto& account_sequence = db.get<account_sequence_object, by_name>(act.account);
   db.modify( account_sequence, [&]( auto& aso ) {
      aso.code_sequence += 1;
   });

   if (new_size != old_size) {
      context.trx_context.add_ram_usage( act.account, new_size - old_size );
   }
}

void apply_dncio_setabi(apply_context& context) {
   auto& db  = context.db;
   auto  act = context.act.data_as<setabi>();

   context.setcode_require_authorization(act.account);

   // Only allow dncio contract.
   if (act.account != dncio::chain::name{N(dncio)}) {
     // exit
     FC_THROW("only allow dncio to setabi");
   }

   auto abi_id = fc::sha256::hash(act.abi.data(), (uint32_t)act.abi.size());

   const auto& account = db.get<account_object,by_name>(act.account);

   int64_t abi_size = act.abi.size();

   int64_t old_size = (int64_t)account.abi.size();
   int64_t new_size = abi_size;

   // Not first time setabi
   if (account.abi_version != fc::sha256::sha256()) {
      // get allow_setabi from system contract table
      if (!allow_setabi(context, abi_id.str())) {
        // exit
        FC_THROW("The abi_id '${abi_id}' is not approved by the system contract", ("abi_id", abi_id));
      }
      // FC_THROW("setabi twice is not allowed");
   }

   FC_ASSERT(account.abi_version != abi_id, "contract is already running this version of abi");

   db.modify( account, [&]( auto& a ) {
      a.abi_version = abi_id;
      a.abi.resize( abi_size );
      if( abi_size > 0 )
         memcpy( a.abi.data(), act.abi.data(), abi_size );
   });

   const auto& account_sequence = db.get<account_sequence_object, by_name>(act.account);
   db.modify( account_sequence, [&]( auto& aso ) {
      aso.abi_sequence += 1;
   });

   if (new_size != old_size) {
      context.trx_context.add_ram_usage( act.account, new_size - old_size );
   }
}

void apply_dncio_updateauth(apply_context& context) {

   auto update = context.act.data_as<updateauth>();
   context.require_authorization(update.account); // only here to mark the single authority on this action as used

   auto& authorization = context.control.get_mutable_authorization_manager();
   auto& db = context.db;

   dnc_ASSERT(!update.permission.empty(), action_validate_exception, "Cannot create authority with empty name");
   dnc_ASSERT( update.permission.to_string().find( "dncio." ) != 0, action_validate_exception,
               "Permission names that start with 'dncio.' are reserved" );
   dnc_ASSERT(update.permission != update.parent, action_validate_exception, "Cannot set an authority as its own parent");
   db.get<account_object, by_name>(update.account);
   dnc_ASSERT(validate(update.auth), action_validate_exception,
              "Invalid authority: ${auth}", ("auth", update.auth));
   if( update.permission == config::active_name )
      dnc_ASSERT(update.parent == config::owner_name, action_validate_exception, "Cannot change active authority's parent from owner", ("update.parent", update.parent) );
   if (update.permission == config::owner_name)
      dnc_ASSERT(update.parent.empty(), action_validate_exception, "Cannot change owner authority's parent");
   else
      dnc_ASSERT(!update.parent.empty(), action_validate_exception, "Only owner permission can have empty parent" );

   if( update.auth.waits.size() > 0 ) {
      auto max_delay = context.control.get_global_properties().configuration.max_transaction_delay;
      dnc_ASSERT( update.auth.waits.back().wait_sec <= max_delay, action_validate_exception,
                  "Cannot set delay longer than max_transacton_delay, which is ${max_delay} seconds",
                  ("max_delay", max_delay) );
   }

   validate_authority_precondition(context, update.auth);



   auto permission = authorization.find_permission({update.account, update.permission});

   // If a parent_id of 0 is going to be used to indicate the absence of a parent, then we need to make sure that the chain
   // initializes permission_index with a dummy object that reserves the id of 0.
   authorization_manager::permission_id_type parent_id = 0;
   if( update.permission != config::owner_name ) {
      auto& parent = authorization.get_permission({update.account, update.parent});
      parent_id = parent.id;
   }

   if( permission ) {
      dnc_ASSERT(parent_id == permission->parent, action_validate_exception,
                 "Changing parent authority is not currently supported");


      int64_t old_size = (int64_t)(config::billable_size_v<permission_object> + permission->auth.get_billable_size());

      authorization.modify_permission( *permission, update.auth );

      int64_t new_size = (int64_t)(config::billable_size_v<permission_object> + permission->auth.get_billable_size());

      context.trx_context.add_ram_usage( permission->owner, new_size - old_size );
   } else {
      const auto& p = authorization.create_permission( update.account, update.permission, parent_id, update.auth );

      int64_t new_size = (int64_t)(config::billable_size_v<permission_object> + p.auth.get_billable_size());

      context.trx_context.add_ram_usage( update.account, new_size );
   }
}

void apply_dncio_deleteauth(apply_context& context) {
//   context.require_write_lock( config::dncio_auth_scope );

   auto remove = context.act.data_as<deleteauth>();
   context.require_authorization(remove.account); // only here to mark the single authority on this action as used

   dnc_ASSERT(remove.permission != config::active_name, action_validate_exception, "Cannot delete active authority");
   dnc_ASSERT(remove.permission != config::owner_name, action_validate_exception, "Cannot delete owner authority");

   auto& authorization = context.control.get_mutable_authorization_manager();
   auto& db = context.db;



   { // Check for links to this permission
      const auto& index = db.get_index<permission_link_index, by_permission_name>();
      auto range = index.equal_range(boost::make_tuple(remove.account, remove.permission));
      dnc_ASSERT(range.first == range.second, action_validate_exception,
                 "Cannot delete a linked authority. Unlink the authority first");
   }

   const auto& permission = authorization.get_permission({remove.account, remove.permission});
   int64_t old_size = config::billable_size_v<permission_object> + permission.auth.get_billable_size();

   authorization.remove_permission( permission );

   context.trx_context.add_ram_usage( remove.account, -old_size );

}

void apply_dncio_linkauth(apply_context& context) {
//   context.require_write_lock( config::dncio_auth_scope );

   auto requirement = context.act.data_as<linkauth>();
   try {
      dnc_ASSERT(!requirement.requirement.empty(), action_validate_exception, "Required permission cannot be empty");

      context.require_authorization(requirement.account); // only here to mark the single authority on this action as used

      auto& db = context.db;
      const auto *account = db.find<account_object, by_name>(requirement.account);
      dnc_ASSERT(account != nullptr, account_query_exception,
                 "Failed to retrieve account: ${account}", ("account", requirement.account)); // Redundant?
      const auto *code = db.find<account_object, by_name>(requirement.code);
      dnc_ASSERT(code != nullptr, account_query_exception,
                 "Failed to retrieve code for account: ${account}", ("account", requirement.code));
      if( requirement.requirement != config::dncio_any_name ) {
         const auto *permission = db.find<permission_object, by_name>(requirement.requirement);
         dnc_ASSERT(permission != nullptr, permission_query_exception,
                    "Failed to retrieve permission: ${permission}", ("permission", requirement.requirement));
      }

      auto link_key = boost::make_tuple(requirement.account, requirement.code, requirement.type);
      auto link = db.find<permission_link_object, by_action_name>(link_key);

      if( link ) {
         dnc_ASSERT(link->required_permission != requirement.requirement, action_validate_exception,
                    "Attempting to update required authority, but new requirement is same as old");
         db.modify(*link, [requirement = requirement.requirement](permission_link_object& link) {
             link.required_permission = requirement;
         });
      } else {
         const auto& l =  db.create<permission_link_object>([&requirement](permission_link_object& link) {
            link.account = requirement.account;
            link.code = requirement.code;
            link.message_type = requirement.type;
            link.required_permission = requirement.requirement;
         });

         context.trx_context.add_ram_usage(
            l.account,
            (int64_t)(config::billable_size_v<permission_link_object>)
         );
      }

  } FC_CAPTURE_AND_RETHROW((requirement))
}

void apply_dncio_unlinkauth(apply_context& context) {
//   context.require_write_lock( config::dncio_auth_scope );

   auto& db = context.db;
   auto unlink = context.act.data_as<unlinkauth>();

   context.require_authorization(unlink.account); // only here to mark the single authority on this action as used

   auto link_key = boost::make_tuple(unlink.account, unlink.code, unlink.type);
   auto link = db.find<permission_link_object, by_action_name>(link_key);
   dnc_ASSERT(link != nullptr, action_validate_exception, "Attempting to unlink authority, but no link found");
   context.trx_context.add_ram_usage(
      link->account,
      -(int64_t)(config::billable_size_v<permission_link_object>)
   );

   db.remove(*link);
}

void apply_dncio_canceldelay(apply_context& context) {
   auto cancel = context.act.data_as<canceldelay>();
   context.require_authorization(cancel.canceling_auth.actor); // only here to mark the single authority on this action as used

   const auto& trx_id = cancel.trx_id;

   context.cancel_deferred_transaction(transaction_id_to_sender_id(trx_id), account_name());
}

} } // namespace dncio::chain
