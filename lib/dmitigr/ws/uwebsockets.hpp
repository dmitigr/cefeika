// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_UWEBSOCKETS_HPP
#define DMITIGR_WS_UWEBSOCKETS_HPP

#ifdef __GNUG__
// Disable some warnings of uWebSockets.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wdocumentation"
#pragma GCC diagnostic ignored "-Wnewline-eof"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#endif

#include "../thirdparty/uwebsockets/App.h"
#include "../thirdparty/uwebsockets/HttpParser.h"
#include "../thirdparty/uwebsockets/HttpResponse.h"

#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif

#endif  // DMITIGR_WS_UWEBSOCKETS_HPP
