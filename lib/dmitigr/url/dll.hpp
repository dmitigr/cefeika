// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or url.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_URL_DLL_HPP
#define DMITIGR_URL_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_URL_API __declspec(dllexport)
  #else
    #if DMITIGR_URL_DLL
      #define DMITIGR_URL_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_URL_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_URL_API
#endif

#ifndef DMITIGR_URL_INLINE
  #if defined(DMITIGR_URL_HEADER_ONLY) && !defined(DMITIGR_URL_BUILDING)
    #define DMITIGR_URL_INLINE inline
  #else
    #define DMITIGR_URL_INLINE
  #endif
#endif  // DMITIGR_URL_INLINE

#endif // DMITIGR_URL_DLL_HPP
