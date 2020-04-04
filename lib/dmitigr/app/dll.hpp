// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or app.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_APP_DLL_HPP
#define DMITIGR_APP_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_APP_API __declspec(dllexport)
  #else
    #if DMITIGR_APP_DLL
      #define DMITIGR_APP_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_APP_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_APP_API
#endif

#ifndef DMITIGR_APP_INLINE
  #if defined(DMITIGR_APP_HEADER_ONLY) && !defined(DMITIGR_APP_BUILDING)
    #define DMITIGR_APP_INLINE inline
  #else
    #define DMITIGR_APP_INLINE
  #endif
#endif  // DMITIGR_APP_INLINE

#endif // DMITIGR_APP_DLL_HPP
