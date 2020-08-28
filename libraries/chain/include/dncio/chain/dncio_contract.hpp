/**
 *  @file
 *  @copyright defined in dnc/LICENSE.txt
 */
#pragma once

#include <dncio/chain/types.hpp>
#include <dncio/chain/contract_types.hpp>

namespace dncio { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   void apply_dncio_newaccount(apply_context&);
   void apply_dncio_updateauth(apply_context&);
   void apply_dncio_deleteauth(apply_context&);
   void apply_dncio_linkauth(apply_context&);
   void apply_dncio_unlinkauth(apply_context&);

   /*
   void apply_dncio_postrecovery(apply_context&);
   void apply_dncio_passrecovery(apply_context&);
   void apply_dncio_vetorecovery(apply_context&);
   */

   void apply_dncio_setcode(apply_context&);
   void apply_dncio_setabi(apply_context&);

   void apply_dncio_canceldelay(apply_context&);
   ///@}  end action handlers

} } /// namespace dncio::chain
