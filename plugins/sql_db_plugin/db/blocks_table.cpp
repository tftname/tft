#include "blocks_table.h"

namespace dncio {

blocks_table::blocks_table(std::shared_ptr<soci::session> session):
    m_session(session)
{

}

} // namespace
