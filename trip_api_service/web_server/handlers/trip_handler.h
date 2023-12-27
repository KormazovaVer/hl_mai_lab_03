#ifndef TRIPHANDLER_H
#define TRIPHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../../database/trip.h"
#include "../../../helper.h"
#include "../clients/auth_service_client.h"

class TripHandler : public HTTPRequestHandler
{
public:
    TripHandler()
    {
    }
    AuthServiceClient _auth_service_client;

    AuthServiceClient& authServiceClient() {
        return _auth_service_client;
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        
        long user_id = authServiceClient().checkAccess(request);

        if (user_id == AuthServiceClient::NOT_AUTHORIZED) {
            unauthorized(response);
            return;
        }

        HTMLForm form(request, request.stream());
        try
        {
            // получить маршрут
            if (isGet(request)) {
                if(!form.has("routeId")) {
                    badRequest(response);
                    return;
                }
                long route_id = atol(form.get("routeId").c_str());
                auto trips = database:Trip::read_by_route_id(route_id);
                Poco::JSON::Object::Ptr content = new Poco::JSON::Object();
                Poco::JSON::Array::Ptr arr = new Poco::JSON::Array();
                for (auto s : routes) {
                    arr->add(s.toJSON());
                }
                content->set("trips", arr);
                content->set("routeId", route_id);
                ok(response, content);
                return;
            }
            // отправить сообщние о маршруте
            if (isPost(request)) {
                if(!form.has("routeId") ||
                   !form.has("text")) {
                    badRequest(response);
                    return;
                }
                long route_id = atol(form.get("routeId").c_str());
                std::string text = std::string(form.get("text").c_str());

                database::Trip trips;
                trip.route_id() = route_id;
                trip.trip() = text;
                trip.user_id() = user_id;
                trip.save_to_mysql();

                Poco::JSON::Object::Ptr content = new Poco::JSON::Object();
                content->set("id", trip.id());
                ok(response, content);
                return;
            } 
            // else if (isPut(request)) {
            //     return;
            // } else if (isDelete(request)) {
            //     return;
            // }
        }
        catch (Poco::Exception& ex) 
        {
            internalServerError(response, ex.what());
        }

        notFound(response, "/trip");
    }
};
#endif