// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mp.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_MP_DLL_HPP
#define DMITIGR_MP_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_MP_API __declspec(dllexport)
  #else
    #if DMITIGR_MP_DLL
      #define DMITIGR_MP_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_MP_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_MP_API
#endif

#ifndef DMITIGR_MP_INLINE
  #if defined(DMITIGR_MP_HEADER_ONLY) && !defined(DMITIGR_MP_BUILDING)
    #define DMITIGR_MP_INLINE inline
  #else
    #define DMITIGR_MP_INLINE
  #endif
#endif  // DMITIGR_MP_INLINE

#endif // DMITIGR_MP_DLL_HPP
