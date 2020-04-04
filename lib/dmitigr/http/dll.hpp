// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_HTTP_DLL_HPP
#define DMITIGR_HTTP_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_HTTP_API __declspec(dllexport)
  #else
    #if DMITIGR_HTTP_DLL
      #define DMITIGR_HTTP_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_HTTP_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_HTTP_API
#endif

#ifndef DMITIGR_HTTP_INLINE
  #if defined(DMITIGR_HTTP_HEADER_ONLY) && !defined(DMITIGR_HTTP_BUILDING)
    #define DMITIGR_HTTP_INLINE inline
  #else
    #define DMITIGR_HTTP_INLINE
  #endif
#endif  // DMITIGR_HTTP_INLINE

#endif // DMITIGR_HTTP_DLL_HPP
