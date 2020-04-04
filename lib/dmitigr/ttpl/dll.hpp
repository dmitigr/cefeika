// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ttpl.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_TTPL_DLL_HPP
#define DMITIGR_TTPL_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_TTPL_API __declspec(dllexport)
  #else
    #if DMITIGR_TTPL_DLL
      #define DMITIGR_TTPL_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_TTPL_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_TTPL_API
#endif

#ifndef DMITIGR_TTPL_INLINE
  #if defined(DMITIGR_TTPL_HEADER_ONLY) && !defined(DMITIGR_TTPL_BUILDING)
    #define DMITIGR_TTPL_INLINE inline
  #else
    #define DMITIGR_TTPL_INLINE
  #endif
#endif  // DMITIGR_TTPL_INLINE

#endif // DMITIGR_TTPL_DLL_HPP
