#include "Echo.h"
#include "Server_ORBInitializer.h"

#include "tao/ORBInitializer_Registry.h"

#include "ace/Get_Opt.h"
#include "ace/OS_NS_stdio.h"


ACE_RCSID (Portable_Interceptors,
           AMI,
           "$Id$")


const char *ior_output_file = "test.ior";

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "o:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
        ior_output_file = get_opts.opt_arg ();
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-o <iorfile>"
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  try
    {
      {
        PortableInterceptor::ORBInitializer_var initializer(
            new Server_ORBInitializer);
        PortableInterceptor::register_orb_initializer(initializer.in());
      }

      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv);

      CORBA::Object_var poa_object =
        orb->resolve_initial_references("RootPOA");

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in ());

      if (CORBA::is_nil (root_poa.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                           " (%P|%t) Panic: nil RootPOA\n"),
                          1);

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager ();

      if (parse_args (argc, argv) != 0)
        return 1;

      Echo *echo_impl;
      ACE_NEW_RETURN (echo_impl,
                      Echo (orb.in ()),
                      1);
      PortableServer::ServantBase_var owner_transfer(echo_impl);

      PortableServer::ObjectId_var id =
        root_poa->activate_object (echo_impl);

      CORBA::Object_var object = root_poa->id_to_reference (id.in ());

      Test::Echo_var echo =
        Test::Echo::_narrow (object.in ());

      CORBA::String_var ior =
        orb->object_to_string (echo.in ());

      // Output the IOR to the <ior_output_file>
      FILE *output_file = ACE_OS::fopen (ior_output_file, "w");
      if (output_file == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Cannot open output file for writing IOR: %s",
                           ior_output_file),
                              1);
      ACE_OS::fprintf (output_file, "%s", ior.in ());
      ACE_OS::fclose (output_file);

      poa_manager->activate ();

      orb->run ();

      ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - event loop finished\n"));

      root_poa->destroy (1, 1);

      orb->destroy ();
    }
  catch (const CORBA::Exception& ex)
    {
      ex._tao_print_exception ("Exception caught:");
      return 1;
    }

  return 0;
}
