// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_WS_DLL_HPP
#define DMITIGR_WS_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_WS_DLL_BUILDING
    #define DMITIGR_WS_API __declspec(dllexport)
  #else
    #if DMITIGR_WS_DLL
      #define DMITIGR_WS_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_WS_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_WS_API
#endif

#ifndef DMITIGR_WS_INLINE
  #if defined(DMITIGR_WS_HEADER_ONLY) && !defined(DMITIGR_WS_BUILDING)
    #define DMITIGR_WS_INLINE inline
  #else
    #define DMITIGR_WS_INLINE
  #endif
#endif  // DMITIGR_WS_INLINE

#endif // DMITIGR_WS_DLL_HPP
