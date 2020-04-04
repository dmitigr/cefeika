// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rng.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_RNG_DLL_HPP
#define DMITIGR_RNG_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_RNG_API __declspec(dllexport)
  #else
    #if DMITIGR_RNG_DLL
      #define DMITIGR_RNG_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_RNG_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_RNG_API
#endif

#ifndef DMITIGR_RNG_INLINE
  #if defined(DMITIGR_RNG_HEADER_ONLY) && !defined(DMITIGR_RNG_BUILDING)
    #define DMITIGR_RNG_INLINE inline
  #else
    #define DMITIGR_RNG_INLINE
  #endif
#endif  // DMITIGR_RNG_INLINE

#endif // DMITIGR_RNG_DLL_HPP
