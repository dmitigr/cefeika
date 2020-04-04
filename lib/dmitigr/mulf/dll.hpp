// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mulf.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_MULF_DLL_HPP
#define DMITIGR_MULF_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_MULF_API __declspec(dllexport)
  #else
    #if DMITIGR_MULF_DLL
      #define DMITIGR_MULF_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_MULF_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_MULF_API
#endif

#ifndef DMITIGR_MULF_INLINE
  #if defined(DMITIGR_MULF_HEADER_ONLY) && !defined(DMITIGR_MULF_BUILDING)
    #define DMITIGR_MULF_INLINE inline
  #else
    #define DMITIGR_MULF_INLINE
  #endif
#endif  // DMITIGR_MULF_INLINE

#endif // DMITIGR_MULF_DLL_HPP
