#ifndef ROUTEHANDLER_H
#define ROUTEHANDLER_H

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

#include "../../../database/route.h"
#include "../../../database/route_to_trip.h"
#include "../../../helper.h"
#include "../clients/auth_service_client.h"

class RouteHandler : public HTTPRequestHandler
{
private:
    AuthServiceClient _auth_service_client;
public:
    RouteHandler()
    {
    }

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
            // получить маршруты по user_id и получить мвршрут по route_id
            // GET /route?route_id={route_id}
            // GET /route?user_id={user_id}
            if (isGet(request)) {
                if (contains(request.getURI(), "searchByRouteId")) {
                    if (!form.has("routeId")) {
                        badRequest(response, "routeId not exist's");
                    }
                    auto routeId = atol(form.get("routeId").c_str());
                    auto route = database::Route::read_by_id(routeId);
                    if (route) {
                        auto jsonRoute = route->toJSON();
                        ok(response, jsonRoute);
                    } else {
                        notFound(response, "cannot find route with id " + std::to_string(routeId));
                    }
                    return;
                }

                if (contains(request.getURI(), "searchByUserId")) {
                    if (!form.has("userId")) {
                        badRequest(response, "userId not exist's");
                        return;
                    }
                    long query_param_id = atol(form.get("userId").c_str());

                    auto users_to_routes = database::UserToChat::read_routes_by_user_id(query_param_id);
                    Poco::JSON::Object::Ptr content = new Poco::JSON::Object();
                    Poco::JSON::Array::Ptr arr = new Poco::JSON::Array();
                    for (auto s : routes_to_trips) {
                        arr->add(s.toJSON());
                    }
                    content->set("routes", arr);
                    ok(response, content);
                    return;
                }
            }
            
            if (isPost(request)) { // создать маршрут
                if (contains(request.getURI(), "addMember")) {
                    if (!form.has("routeId") || !form.has("userId")) {
                        badRequest(response, "do not have valid parameters routeId and userId");
                        return;
                    }
                    long routeId = atol(form.get("routeId").c_str());
                    long userId = atol(form.get("userId").c_str());
                    database::RouteToTrip routeToTrip;
                    routeToTrip.route_id() = routeId;
                    routeToTrip.user_id() = userId;

                    try {
                        routeToTrip.save_to_mysql();
                        Poco::JSON::Object::Ptr content = new Poco::JSON::Object();
                        ok(response, content);
                    } catch (Poco::Exception& ex) {
                        std::cout << "Exception: " << ex.what() << std::endl;
                        unprocessableEntity(response, std::string(ex.what()));
                    }
                    return;
                }
                if(form.has("routeName")) {
                    try {
                        database::Route route;
                        database::RouteToTrip routeToTrip;
                        route.name() = form.get("routeName");
                        route.creator_id() = user_id;
                        std::string message;
                        std::string reason;
                        Poco::JSON::Object::Ptr content = new Poco::JSON::Object();
                        route.save_to_mysql();
                        content->set("route_id", route.get_id());
                        routeToTrip.route_id() = route.get_id();
                        routeToTrip.user_id() = user_id;
                        routeToTrip.save_to_mysql();
                        ok(response, content);
                    } catch (const Poco::Exception& ex) {
                        std::cout << "Exception: " << ex.what() << std::endl;
                        internalServerError(response, std::string(ex.what()));
                    }
                    return;
                }
            }
        }
        catch (...)
        {
            std::cout << "Unexpected error";
            internalServerError(response);
            return;
        }

        notFound(response, "/route");
    }
};
#endif