// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or str.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_STR_DLL_HPP
#define DMITIGR_STR_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_STR_API __declspec(dllexport)
  #else
    #if DMITIGR_STR_DLL
      #define DMITIGR_STR_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_STR_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_STR_API
#endif

#ifndef DMITIGR_STR_INLINE
  #if defined(DMITIGR_STR_HEADER_ONLY) && !defined(DMITIGR_STR_BUILDING)
    #define DMITIGR_STR_INLINE inline
  #else
    #define DMITIGR_STR_INLINE
  #endif
#endif  // DMITIGR_STR_INLINE

#endif // DMITIGR_STR_DLL_HPP
