#ifndef ROUTE_TO_TRIP_H
#define ROUTE_TO_TRIP_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include "route.h"
#include <optional>

namespace database
{
    class RoutetoTrip{
        private:
            long _route_id;
            long _user_id;

        public:

            static RoutetoTri fromJSON(const std::string & str);

            long             get_route_id() const;
            long             get_user_id() const;

            long&        route_id();
            long&        user_id();

            static void init();
            static std::vector<route> read_routes_by_user_id(long user_id);
            static std::optional<long> auth(std::string &login, std::string &password);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif