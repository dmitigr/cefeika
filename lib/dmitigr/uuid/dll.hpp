// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or uuid.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_UUID_DLL_HPP
#define DMITIGR_UUID_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_UUID_API __declspec(dllexport)
  #else
    #if DMITIGR_UUID_DLL
      #define DMITIGR_UUID_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_UUID_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_UUID_API
#endif

#ifndef DMITIGR_UUID_INLINE
  #if defined(DMITIGR_UUID_HEADER_ONLY) && !defined(DMITIGR_UUID_BUILDING)
    #define DMITIGR_UUID_INLINE inline
  #else
    #define DMITIGR_UUID_INLINE
  #endif
#endif  // DMITIGR_UUID_INLINE

#endif // DMITIGR_UUID_DLL_HPP
