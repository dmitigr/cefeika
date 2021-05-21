// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wscl.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_WSCL_DLL_HPP
#define DMITIGR_WSCL_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_WSCL_DLL_BUILDING
    #define DMITIGR_WSCL_API __declspec(dllexport)
  #else
    #if DMITIGR_WSCL_DLL
      #define DMITIGR_WSCL_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_WSCL_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_WSCL_API
#endif

#ifndef DMITIGR_WSCL_INLINE
  #if defined(DMITIGR_WSCL_HEADER_ONLY) && !defined(DMITIGR_WSCL_BUILDING)
    #define DMITIGR_WSCL_INLINE inline
  #else
    #define DMITIGR_WSCL_INLINE
  #endif
#endif  // DMITIGR_WSCL_INLINE

#endif // DMITIGR_WSCL_DLL_HPP
