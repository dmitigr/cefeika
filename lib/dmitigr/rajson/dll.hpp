// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_RAJSON_DLL_HPP
#define DMITIGR_RAJSON_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_RAJSON_API __declspec(dllexport)
  #else
    #if DMITIGR_RAJSON_DLL
      #define DMITIGR_RAJSON_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_RAJSON_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_RAJSON_API
#endif

#ifndef DMITIGR_RAJSON_INLINE
  #if defined(DMITIGR_RAJSON_HEADER_ONLY) && !defined(DMITIGR_RAJSON_BUILDING)
    #define DMITIGR_RAJSON_INLINE inline
  #else
    #define DMITIGR_RAJSON_INLINE
  #endif
#endif  // DMITIGR_RAJSON_INLINE

#endif // DMITIGR_RAJSON_DLL_HPP
