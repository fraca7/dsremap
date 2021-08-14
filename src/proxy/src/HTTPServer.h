
/**
 * @file HTTPServer.h
 */

/**********************************************************************

  Created: 13 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include <string>

#include <libsoup/soup.h>

#include <src/utils/Logger.h>

namespace dsremap
{
  class Application;

  class HTTPServer : public Logger
  {
  public:
    HTTPServer(Application&, unsigned int port, const std::string& config_path);
    ~HTTPServer();

  private:
    Application& _app;
    SoupServer* _server;
    std::string _config_path;

    static void static_request_callback(SoupServer*, SoupMessage*, const char*, GHashTable*, SoupClientContext*, gpointer);
    void request_callback(SoupMessage*, const char*, GHashTable*, SoupClientContext*);
  };
}

#endif /* _HTTPSERVER_H */
