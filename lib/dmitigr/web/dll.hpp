// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_WEB_DLL_HPP
#define DMITIGR_WEB_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_WEB_API __declspec(dllexport)
  #else
    #if DMITIGR_WEB_DLL
      #define DMITIGR_WEB_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_WEB_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_WEB_API
#endif

#ifndef DMITIGR_WEB_INLINE
  #if defined(DMITIGR_WEB_HEADER_ONLY) && !defined(DMITIGR_WEB_BUILDING)
    #define DMITIGR_WEB_INLINE inline
  #else
    #define DMITIGR_WEB_INLINE
  #endif
#endif  // DMITIGR_WEB_INLINE

#endif // DMITIGR_WEB_DLL_HPP
