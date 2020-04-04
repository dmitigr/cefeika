// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or cfg.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_CFG_DLL_HPP
#define DMITIGR_CFG_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_CFG_API __declspec(dllexport)
  #else
    #if DMITIGR_CFG_DLL
      #define DMITIGR_CFG_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_CFG_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_CFG_API
#endif

#ifndef DMITIGR_CFG_INLINE
  #if defined(DMITIGR_CFG_HEADER_ONLY) && !defined(DMITIGR_CFG_BUILDING)
    #define DMITIGR_CFG_INLINE inline
  #else
    #define DMITIGR_CFG_INLINE
  #endif
#endif  // DMITIGR_CFG_INLINE

#endif // DMITIGR_CFG_DLL_HPP
