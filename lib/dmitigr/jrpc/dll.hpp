// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_JRPC_DLL_HPP
#define DMITIGR_JRPC_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_JRPC_API __declspec(dllexport)
  #else
    #if DMITIGR_JRPC_DLL
      #define DMITIGR_JRPC_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_JRPC_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_JRPC_API
#endif

#ifndef DMITIGR_JRPC_INLINE
  #if defined(DMITIGR_JRPC_HEADER_ONLY) && !defined(DMITIGR_JRPC_BUILDING)
    #define DMITIGR_JRPC_INLINE inline
  #else
    #define DMITIGR_JRPC_INLINE
  #endif
#endif  // DMITIGR_JRPC_INLINE

#endif // DMITIGR_JRPC_DLL_HPP
