
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl ICEVENTSOURCE_SVNT
// ------------------------------
#ifndef ICEVENTSOURCE_SVNT_EXPORT_H
#define ICEVENTSOURCE_SVNT_EXPORT_H

#include "ace/config-all.h"

#if !defined (ICEVENTSOURCE_SVNT_HAS_DLL)
#  define ICEVENTSOURCE_SVNT_HAS_DLL 1
#endif /* ! ICEVENTSOURCE_SVNT_HAS_DLL */

#if defined (ICEVENTSOURCE_SVNT_HAS_DLL) && (ICEVENTSOURCE_SVNT_HAS_DLL == 1)
#  if defined (ICEVENTSOURCE_SVNT_BUILD_DLL)
#    define ICEVENTSOURCE_SVNT_Export ACE_Proper_Export_Flag
#    define ICEVENTSOURCE_SVNT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define ICEVENTSOURCE_SVNT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* ICEVENTSOURCE_SVNT_BUILD_DLL */
#    define ICEVENTSOURCE_SVNT_Export ACE_Proper_Import_Flag
#    define ICEVENTSOURCE_SVNT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define ICEVENTSOURCE_SVNT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* ICEVENTSOURCE_SVNT_BUILD_DLL */
#else /* ICEVENTSOURCE_SVNT_HAS_DLL == 1 */
#  define ICEVENTSOURCE_SVNT_Export
#  define ICEVENTSOURCE_SVNT_SINGLETON_DECLARATION(T)
#  define ICEVENTSOURCE_SVNT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* ICEVENTSOURCE_SVNT_HAS_DLL == 1 */

// Set ICEVENTSOURCE_SVNT_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (ICEVENTSOURCE_SVNT_NTRACE)
#  if (ACE_NTRACE == 1)
#    define ICEVENTSOURCE_SVNT_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define ICEVENTSOURCE_SVNT_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !ICEVENTSOURCE_SVNT_NTRACE */

#if (ICEVENTSOURCE_SVNT_NTRACE == 1)
#  define ICEVENTSOURCE_SVNT_TRACE(X)
#else /* (ICEVENTSOURCE_SVNT_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define ICEVENTSOURCE_SVNT_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (ICEVENTSOURCE_SVNT_NTRACE == 1) */

#endif /* ICEVENTSOURCE_SVNT_EXPORT_H */

// End of auto generated file.
