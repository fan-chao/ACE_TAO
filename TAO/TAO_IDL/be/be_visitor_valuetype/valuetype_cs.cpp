
//=============================================================================
/**
 *  @file    valuetype_cs.cpp
 *
 *  Visitor generating code for Valuetypes in the client stubs file.
 *
 *  @author Torsten Kuepper  <kuepper2@lfa.uni-wuppertal.de> based on code from Aniruddha Gokhale
 */
//=============================================================================

#include "valuetype.h"

// ************************************************************
// Valuetype visitor for client stubs.
// ************************************************************

be_visitor_valuetype_cs::be_visitor_valuetype_cs (be_visitor_context *ctx)
  : be_visitor_valuetype (ctx)
{
}

be_visitor_valuetype_cs::~be_visitor_valuetype_cs ()
{
}

int
be_visitor_valuetype_cs::visit_valuetype (be_valuetype *node)
{
  if (node->cli_stub_gen () || node->imported ())
    {
      return 0;
    }

  if (be_global->tc_support ())
    {
      be_visitor_context ctx (*this->ctx_);
      TAO::be_visitor_value_typecode tc_visitor (&ctx);

      if (tc_visitor.visit_valuetype (node) == -1)
        {
          ACE_ERROR_RETURN ((LM_ERROR,
                             "(%N:%l) be_visitor_valuetype_cs::"
                             "visit_valuetype - "
                             "TypeCode definition failed\n"),
                            -1);
        }
    }

  TAO_OutStream *os = this->ctx_->stream ();

  TAO_INSERT_COMMENT (os);

  if (node->is_defined ())
    {
      *os << be_nl
          << be_global->core_versioning_begin ();

      *os << be_nl_2
          << "void" << be_nl
          << "TAO::Value_Traits<" << node->name  () << ">::add_ref ("
          << node->name () << " * p)" << be_nl
          << "{" << be_idt_nl
          << "::CORBA::add_ref (p);" << be_uidt_nl
          << "}";

      *os << be_nl_2
          << "void" << be_nl
          << "TAO::Value_Traits<" << node->name () << ">::remove_ref ("
          << node->name () << " * p)" << be_nl
          << "{" << be_idt_nl
          << "::CORBA::remove_ref (p);" << be_uidt_nl
          << "}";

      *os << be_nl_2
          << "void" << be_nl
          << "TAO::Value_Traits<" << node->name () << ">::release ("
          << node->name () << " * p)" << be_nl
          << "{" << be_idt_nl
          << "::CORBA::remove_ref (p);" << be_uidt_nl
          << "}";

      *os << be_nl
          << be_global->core_versioning_end () << be_nl;
    }

  // The _downcast method.
  *os << be_nl_2
      << node->name () << " *" << be_nl << node->name ()
      << "::_downcast ( ::CORBA::ValueBase *v)" << be_nl
      << "{" << be_idt_nl
      << "return dynamic_cast< ::" << node->name ()
      << " * > (v);" << be_uidt_nl
      << "}" << be_nl_2;

  // The _tao_obv_repository_id method.
  *os << "const char *" << be_nl
      << node->name ()
      << "::_tao_obv_repository_id () const" << be_nl
      << "{" << be_idt_nl
      << "return this->_tao_obv_static_repository_id ();"
      << be_uidt_nl
      << "}" << be_nl_2;

  *os << "void" << be_nl
      << node->name ()
      << "::_tao_obv_truncatable_repo_ids (Repository_Id_List& ids) const"
      << be_nl
      << "{" << be_idt_nl
      << "ids.push_back (this->_tao_obv_static_repository_id ());";

  if (node->truncatable ())
    {
      *os << be_nl
          << node->inherits_concrete ()->name ()
          << "::_tao_obv_truncatable_repo_ids (ids);" << be_uidt_nl
          << "}" << be_nl_2;
    }
  else
    {
      *os << be_uidt_nl << "}" << be_nl_2;
    }

  if (be_global->any_support ())
    {
      *os << "void" << be_nl
          << node->name ()
          << "::_tao_any_destructor (void *_tao_void_pointer)" << be_nl
          << "{" << be_idt_nl
          << node->local_name () << " *_tao_tmp_pointer =" << be_idt_nl
          << "static_cast<" << be_idt
          << node->local_name () << " *> ("
          << "_tao_void_pointer);" << be_uidt << be_uidt_nl
          << "::CORBA::remove_ref (_tao_tmp_pointer);" << be_uidt_nl
          << "}" << be_nl_2;
    }

  // Switch streams to the *A.cpp file if we are using this option.
  if (be_global->gen_anyop_files ())
    {
      os = tao_cg->anyop_source ();
    }

  if (be_global->tc_support ())
    {
      *os << "// TAO extension - the virtual _type method." << be_nl;
      *os << "::CORBA::TypeCode_ptr " << node->name ()
          << "::_tao_type () const" << be_nl;
      *os << "{" << be_idt_nl;
      *os << "return ::" << node->tc_name () << ";" << be_uidt_nl;
      *os << "}" << be_nl_2;
    }

  // Make sure we are generating to *C.cpp regardless of the above.
  os = tao_cg->client_stubs ();

  // Generate destructor.
  //
  // @@ Do not inline this destructor.  It is virtual.  Inlining
  //    virtual functions, including virtual destructors, wreaks havoc
  //    with g++ >= 4.0 RTTI support when the
  //    "-fvisibility-inlines-hidden" command line option is used.
  *os << node->name () << "::~" << node->local_name () << " ()"
      << be_nl
      << "{}" << be_nl_2;

  bool is_an_amh_exception_holder =
    this->is_amh_exception_holder (node);

  // Nothing to marshal if abstract valuetype.
  if (!node->is_abstract () && !is_an_amh_exception_holder)
    {

      this->marshal_unmarshal_v (node);

      *os << "::CORBA::Boolean" << be_nl
          << node->name ()
          << "::_tao_match_formal_type (ptrdiff_t formal_type_id) const"
          << be_nl
          << "{" << be_idt_nl
          << "return formal_type_id == reinterpret_cast<ptrdiff_t> ("
          << node->name() << "::_downcast);" << be_uidt_nl
          << "}" << be_nl_2;

    }
  else if (is_an_amh_exception_holder)
    {
      // @@ Do not inline.  They're virtual.  Inlining virtual
      // functions, including virtual destructors, wreaks havoc with
      // g++ >= 4.0 RTTI support when the
      // "-fvisibility-inlines-hidden" command line option is used.

      // The virtual _copy_value method.
      *os << "::CORBA::ValueBase *" << be_nl
          << node->name () << "::_copy_value ()" << be_nl
          << "{" << be_idt_nl
          << "::CORBA::ValueBase *ret_val {};" << be_nl
          << "ACE_NEW_THROW_EX (" << be_idt_nl
          << "ret_val," << be_nl
          << node->local_name () << " ()," << be_nl
          << "::CORBA::NO_MEMORY ()" << be_uidt_nl
          << ");" << be_nl
          << "return ret_val;" << be_uidt_nl
          << "}" << be_nl_2;

      // The virtual _tao_marshal_v method.
      *os << "::CORBA::Boolean" << be_nl
          << node->name () << "::_tao_marshal_v (TAO_OutputCDR &) const"
          << be_nl
          << "{" << be_idt_nl
          << "return true;" << be_uidt_nl
          << "}" << be_nl_2;

      // The virtual _tao_unmarshal_v method.
      *os << "::CORBA::Boolean" << be_nl
          << node->name () << "::_tao_unmarshal_v (TAO_InputCDR &)"
          << be_nl
          << "{" << be_idt_nl
          << "return true;" << be_uidt_nl
          << "}" << be_nl_2;

      // The virtual _tao_match_formal_type method.
      *os << "::CORBA::Boolean" << be_nl
          << node->name ()
          << "::_tao_match_formal_type (ptrdiff_t) const"
          << be_nl
          << "{" << be_idt_nl
          << "return false;"<< be_uidt_nl
          << "}" << be_nl_2;


      if (!node->opt_accessor () && !node->is_abstract ())
        {
          *os << "::CORBA::Boolean" << be_nl
              << node->name () << "::_tao_marshal__" << node->flat_name ()
              << " (TAO_OutputCDR &, TAO_ChunkInfo&) const" << be_nl
              << "{" << be_idt_nl
              << "return true;" << be_uidt_nl
              << "}" << be_nl_2;

          *os << "::CORBA::Boolean" << be_nl
              << node->name () << "::_tao_unmarshal__" << node->flat_name ()
              << " (TAO_InputCDR &, TAO_ChunkInfo&)" << be_nl
              << "{" << be_idt_nl
              << "return true;" << be_uidt_nl
              << "}" << be_nl_2;
        }
    }

  if (be_global->gen_ostream_operators ())
    {
      *os << "std::ostream &" << be_nl
          << node->name () << "::_tao_stream_v (std::ostream &strm) const"
          << be_nl
          << "{" << be_idt_nl
          << "strm << \"" << node->name () << "(\"";

      unsigned long index = 0;
      this->gen_ostream_operator_r (node, index);

      *os << be_nl
          << "     << \")\";" << be_nl_2
          << "return strm;" << be_uidt_nl
          << "}" << be_nl_2;
    }

  // The static T::_tao_unmarshal method

  *os << "::CORBA::Boolean" << be_nl << node->name()
      << "::_tao_unmarshal (" << be_idt << be_idt_nl
      << "TAO_InputCDR &strm," << be_nl
      << node->local_name () << " *&new_object" << be_uidt_nl
      << ")" << be_uidt_nl
      << "{" << be_idt_nl
      << "::CORBA::ValueBase *base {};" << be_nl
      << "::CORBA::Boolean is_indirected = false;" << be_nl
      << "::CORBA::Boolean is_null_object = false;" << be_nl
      << "::CORBA::Boolean const retval =" << be_idt_nl
      << "::CORBA::ValueBase::_tao_unmarshal_pre ("
      << be_idt << be_idt_nl
      << "strm," << be_nl
      << "base," << be_nl
      << node->local_name () << "::_tao_obv_static_repository_id ()," << be_nl
      << "is_null_object," << be_nl
      << "is_indirected" << be_uidt_nl
      << ");" << be_uidt << be_uidt_nl << be_nl
      << "::CORBA::ValueBase_var owner (base);" << be_nl_2
      << "if (!retval)" << be_idt_nl
      << "return false;" << be_uidt_nl << be_nl
      << "if (is_null_object)" << be_idt_nl
      << "return true;" << be_uidt_nl << be_nl
      << "if (!is_indirected && !base->_tao_unmarshal_v (strm))" << be_idt_nl
      << "return false;" << be_uidt_nl << be_nl
      << "// Now base must point to the unmarshaled object." << be_nl
      << "// Align the pointer to the right subobject." << be_nl
      << "new_object = " << node->local_name () << "::_downcast (base);" << be_nl
      << "if (0 == new_object)" << be_idt_nl
      << "return false;" << be_uidt_nl << be_nl
      << "if (is_indirected)" << be_idt_nl
      << "new_object->_add_ref ();" << be_uidt_nl << be_nl
      << "owner._retn ();" << be_nl
      << "return true;" << be_uidt_nl
      << "}";

  // If we inherit from CORBA::Object and/or CORBA::AbstractBase
  // (in addition to CORBA::ValueBase) we have to add these
  // to avoid ambiguity.
  if (node->n_supports () > 0)
    {
      *os << be_nl_2
          << "::CORBA::ValueBase *" << be_nl
          << node->name () << "::_tao_to_value ()" << be_nl
          << "{" << be_idt_nl
          << "return this;" << be_uidt_nl
          << "}";
    }

  // Generate code for the elements of the valuetype.
  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_valuetype_cs::"
                         "visit_valuetype - "
                         "codegen for scope failed\n"),
                        -1);
    }

  // Generate the _init-related code.
  be_visitor_context ctx (*this->ctx_);
  be_visitor_valuetype_init_cs vi_visitor (&ctx);

  if (vi_visitor.visit_valuetype (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_valuetype_ch::"
                         "visit_valuetype - "
                         "failed to generate _init construct.\n"),
                        -1);
    }

  return 0;
}

void be_visitor_valuetype_cs::marshal_unmarshal_v (be_valuetype *node)
{
  TAO_OutStream *const os = this->ctx_->stream ();

  if (!be_global->cdr_support ())
    {
      *os << "::CORBA::Boolean" << be_nl
          << node->name ()
          << "::_tao_marshal_v (TAO_OutputCDR &) const" << be_nl
          << "{" << be_idt_nl
          << "return false;" << be_uidt_nl
          << "}" << be_nl_2
          << "::CORBA::Boolean" << be_nl
          << node->name ()
          << "::_tao_unmarshal_v (TAO_InputCDR &)" << be_nl
          << "{" << be_idt_nl
          << "return false;" << be_uidt_nl
          << "}" << be_nl_2;
      return;
    }

  // The virtual _tao_marshal_v method.
  *os << "::CORBA::Boolean" << be_nl
      << node->name ()
      << "::_tao_marshal_v (TAO_OutputCDR & strm) const" << be_nl
      << "{" << be_idt_nl
      << "TAO_ChunkInfo ci (this->is_truncatable_ || this->chunking_);"
      << be_nl
      << "return ";

  if (node->opt_accessor ())
    {
      be_decl *scope =
        dynamic_cast<be_scope*> (node->defined_in ())->decl ();

      *os << scope->name () << "::"
          << node->local_name ()
          << "::_tao_marshal_state (strm, ci);" << be_uidt_nl;
    }
  else
    {
      *os << "this->_tao_marshal__" << node->flat_name ()
          << " (strm, ci);" << be_uidt_nl;
    }

  *os << "}" << be_nl_2;

  // The virtual _tao_unmarshal_v method.
  *os << "::CORBA::Boolean" << be_nl
      << node->name ()
      << "::_tao_unmarshal_v (TAO_InputCDR & strm)"
      << be_nl
      << "{" << be_idt_nl
      << "TAO_ChunkInfo ci (this->is_truncatable_ || this->chunking_, 1);"
      << be_nl
      << "return ";

  if (node->opt_accessor ())
    {
      be_decl *scope =
        dynamic_cast<be_scope*> (node->defined_in ())->decl ();

      *os << scope->name () << "::"
          << node->local_name ()
          <<"::_tao_unmarshal_state (strm,ci);" << be_uidt_nl;
    }
  else
    {
      *os << "this->_tao_unmarshal__" << node->flat_name ()
          << " (strm,ci);" << be_uidt_nl;
    }

  *os << "}" << be_nl_2;
}

int
be_visitor_valuetype_cs::visit_eventtype (be_eventtype *node)
{
  return this->visit_valuetype (node);
}

int
be_visitor_valuetype_cs::visit_operation (be_operation *node)
{
  if (node->cli_stub_gen () || node->imported ())
    {
      return 0;
    }

  be_valuetype *parent =
    dynamic_cast<be_valuetype*> (node->defined_in ());

  if (parent == nullptr || ! this->is_amh_exception_holder (parent))
    {
      return 0;
    }

  TAO_OutStream *os = this->ctx_->stream ();
  this->ctx_->node (node);

  TAO_INSERT_COMMENT (os);

  // STEP I: Generate the return type.
  be_type *bt = dynamic_cast<be_type*> (node->return_type ());

  if (!bt)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_valuetype_cs::"
                         "visit_operation - "
                         "Bad return type\n"),
                        -1);
    }

  // Grab the right visitor to generate the return type.
  be_visitor_context ctx (*this->ctx_);
  be_visitor_operation_rettype or_visitor (&ctx);

  if (bt->accept (&or_visitor) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_valuetype_cs::"
                         "visit_operation - "
                         "codegen for return type failed\n"),
                        -1);
    }

  // STEP 2: Generate the operation name.
  *os << be_nl << parent->name () << "::" << node->local_name ();

  // STEP 3: Generate the argument list with the appropriate mapping.
  // For these, we grab a visitor that generates the parameter listing.
  ctx = *this->ctx_;
  ctx.state (TAO_CodeGen::TAO_OBV_OPERATION_ARGLIST_CS);
  be_visitor_obv_operation_arglist ooa_visitor (&ctx);

  if (node->accept (&ooa_visitor) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "(%N:%l) be_visitor_valuetype_cs::"
                         "visit_operation - "
                         "codegen for argument list failed\n"),
                        -1);
    }

  // We need to throw an exceptions that was assigned in a
  // different place (by the app-developer). ACE_THROW does
  // not fit the bill since the ACE_THROW macro constructs the
  // exception passed to it.  Also exception->_raise() is
  // ruled out since in platforms without native exception
  // support, the _raise() function does not do anything. Below we
  // explicitly take care of both cases (platforms with
  // and without native exception support).
  *os << be_nl
      << "{" << be_idt_nl
      << "std::unique_ptr< ::CORBA::Exception> safety (this->exception);" << be_nl
      << "this->exception->_raise ();" << be_uidt_nl
      << "}"
      << be_nl;

  return 0;
}

void
be_visitor_valuetype_cs::gen_ostream_operator_r (be_valuetype *node,
                                                 unsigned long &index)
{
  TAO_OutStream *os = this->ctx_->stream ();
  AST_Type *parent = node->inherits_concrete ();

  // Recurse up the parent chain.
  if (parent != nullptr)
    {
      this->gen_ostream_operator_r (dynamic_cast<be_valuetype*> (parent),
                                    index);
    }

  // Generate output for the members of whichever recursion we are in.
  for (UTL_ScopeActiveIterator i (node, UTL_Scope::IK_decls);
       !i.is_done ();
       i.next ())
    {
      be_field *f = dynamic_cast<be_field*> (i.item ());
      be_attribute *attr =
        dynamic_cast<be_attribute*> (i.item ());

      // No way to access the private members from generated code.
      if (f == nullptr
          || f->visibility () != AST_Field::vis_PUBLIC
          || attr != nullptr)
        {
          continue;
        }

      if (index++ != 0)
        {
          *os << " << \", \"";
        }

      *os << be_nl
          << "     << ";

      ACE_CString instance_name ("this->");
      instance_name += f->local_name ()->get_string ();
      f->gen_member_ostream_operator (os,
                                      instance_name.c_str (),
                                      false,
                                      true);
    }
}
