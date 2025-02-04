// -*- C++ -*-

//=============================================================================
/**
 *  @file    Dirent_Selector.h
 *
 *  Define a portable C++ interface to the <ACE_OS::scandir> method.
 *
 *  @author Rich Newman <RNewman@directv.com>
 */
//=============================================================================

#ifndef ACE_DIRENT_SELECTOR_H
#define ACE_DIRENT_SELECTOR_H
#include /**/ "ace/pre.h"

#include /**/ "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/OS_NS_dirent.h"    /* Need ACE_SCANDIR_SELECTOR, COMPARATOR */
#include "ace/os_include/os_dirent.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
 * @class ACE_Dirent_Selector
 *
 * @brief Define a portable C++ directory-entry iterator based on the
 * POSIX scandir API.
 */
class ACE_Export ACE_Dirent_Selector
{
public:
  /// Constructor
  ACE_Dirent_Selector ();

  /// Destructor.
  virtual ~ACE_Dirent_Selector ();

  /// Return the length of the list of matching directory entries.
  int length () const;

  /// Return the entry at @a index.
  ACE_DIRENT *operator[] (const int index) const;

  /// Free up resources.
  int close ();

  /// Open the directory @a dir and populate the current list of names with
  /// directory entries that match the @a selector and @a comparator.
  int open (const ACE_TCHAR *dir,
            ACE_SCANDIR_SELECTOR selector = 0,
            ACE_SCANDIR_COMPARATOR comparator = 0);

protected:
  /// Ptr to the namelist array.
  ACE_DIRENT **namelist_;

  /// Number of entries in the array.
  int n_;
};

ACE_END_VERSIONED_NAMESPACE_DECL

#if defined (__ACE_INLINE__)
#include "ace/Dirent_Selector.inl"
#endif /* __ACE_INLINE__ */

#include /**/ "ace/post.h"
#endif /* ACE_DIRENT_SELECTOR_H */
