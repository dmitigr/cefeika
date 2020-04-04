// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_DT_DLL_HPP
#define DMITIGR_DT_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_DT_API __declspec(dllexport)
  #else
    #if DMITIGR_DT_DLL
      #define DMITIGR_DT_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_DT_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_DT_API
#endif

#ifndef DMITIGR_DT_INLINE
  #if defined(DMITIGR_DT_HEADER_ONLY) && !defined(DMITIGR_DT_BUILDING)
    #define DMITIGR_DT_INLINE inline
  #else
    #define DMITIGR_DT_INLINE
  #endif
#endif  // DMITIGR_DT_INLINE

#endif // DMITIGR_DT_DLL_HPP
