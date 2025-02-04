// -*- C++ -*-

//=============================================================================
/**
 *  @file   FT_Storable_Naming_Context_Factory.h
 *
 *
 *  @author Kevin Stanley stanleyk@ociweb.com>
 */
//=============================================================================


#ifndef TAO_FT_STORABLE_NAMING_CONTEXT_FACTORY_H
#define TAO_FT_STORABLE_NAMING_CONTEXT_FACTORY_H
#include /**/ "ace/pre.h"

#include "orbsvcs/Naming/Storable_Naming_Context_Factory.h"
#include "tao/ORB.h"
#include "orbsvcs/Naming/nsconf.h"
#include "orbsvcs/Naming/FaultTolerant/ftnaming_export.h"
#include "orbsvcs/Naming/Hash_Naming_Context.h"
#include "orbsvcs/Naming/Storable_Naming_Context.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

class TAO_FT_Naming_Replication_Manager;

/**
 * @class TAO_Naming_Context_Factory
 *
 * @brief
 */
class TAO_FtNaming_Export TAO_FT_Storable_Naming_Context_Factory :
  public TAO_Storable_Naming_Context_Factory
{
public:
  /// Data structure used by TAO_Persistent_Context_Index -
  /// typedef for ease of use.
  typedef TAO_Storable_Naming_Context_Factory::HASH_MAP HASH_MAP;

  /// Constructor.
  TAO_FT_Storable_Naming_Context_Factory (
                                          size_t hash_table_size = ACE_DEFAULT_MAP_SIZE,
                                          TAO_FT_Naming_Replication_Manager * rep = 0);

  /// Destructor.  Does not deallocate the hash map: if an instance of
  /// this class goes out of scope, its hash_map remains in persistent storage.
  virtual ~TAO_FT_Storable_Naming_Context_Factory () = default;

  TAO_FT_Naming_Replication_Manager * replicator (void);

  /// Factory method for creating an implementation object for naming contexts.
  /// If an existing naming context implementation is being rebuilt,
  /// the map and counter parameters should be provided to the underlying
  /// HASH_MAP implementation
  virtual TAO_Storable_Naming_Context* create_naming_context_impl (
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr poa,
    const char *poa_id,
    TAO::Storable_Factory *factory);

  TAO_FT_Naming_Replication_Manager *replicator_;
};

TAO_END_VERSIONED_NAMESPACE_DECL

#include /**/ "ace/post.h"
#endif /* TAO_FT_STORABLE_NAMING_CONTEXT_FACTORY_H */
