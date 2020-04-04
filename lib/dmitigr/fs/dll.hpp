// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fs.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_FS_DLL_HPP
#define DMITIGR_FS_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_FS_API __declspec(dllexport)
  #else
    #if DMITIGR_FS_DLL
      #define DMITIGR_FS_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_FS_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_FS_API
#endif

#ifndef DMITIGR_FS_INLINE
  #if defined(DMITIGR_FS_HEADER_ONLY) && !defined(DMITIGR_FS_BUILDING)
    #define DMITIGR_FS_INLINE inline
  #else
    #define DMITIGR_FS_INLINE
  #endif
#endif  // DMITIGR_FS_INLINE

#endif // DMITIGR_FS_DLL_HPP
