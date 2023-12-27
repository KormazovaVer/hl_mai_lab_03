#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Trip{
        private:
            long _id;
			long _route_id;
			long _user_id;
			std::string _trip;
            

        public:

            static Trip fromJSON(const std::string & str);
            long               get_id() const;
            long               get_route_id() const;
            long               get_user_id() const;
            const std::string &get_trip() const;

            long&        id();
            long&        route_id();
            long&        user_id();
            std::string &trip();

            static void init();
            static std::optional<Trip> read_by_id(long id);
            static std::optional<Trip> read_by_user_id(long id);
            static std::vector<Trip> read_by_route_id(long route_id);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif