#include "route_to_trip.h"
#include "user.h"
#include "route.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void RouteToTrip::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `RouteToTrip` (`route_id` INT NOT NULL,"
                        << "`user_id` INT NOT NULL,"
                        << "PRIMARY KEY(route_id,user_id)-- sharding:0,"
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }

        std::cout << "Entity UserToChat initiated\n";
    }

    Poco::JSON::Object::Ptr RouteToTrip::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("route_id", _route_id);
        root->set("user_id", _user_id);

        return root;
    }

    RouteToTrip RouteToTrip::fromJSON(const std::string &str)
    {
        RouteToTrip routetotrip;
        Poco::JSON::Parser arser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        route_to_trip.route_id() = object->getValue<long>("route_id");
        route_to_trip.user_id() = object->getValue<long>("user_id");

        return route_to_trip;
    }


    std::vector<Route> RouteToTrip::read_routes_by_user_id(long user_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Route> result;
            Route a;
            select << "SELECT DISTINCT route.id, route.name FROM Route INNER JOIN RouteToTrip ON Route.id = RouteToTrip.route_id WHERE RouteToTrip.user_id=?--sharding:0",
                into(a.id()),
                into(a.name()),
                use(user_id),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    void RouteToTrip::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO RouteToTrip (route_id, user_id) VALUES(?, ?)--sharding:0",
                use(_route_id),
                use(_user_id);

            insert.execute();

            std::cout << "inserted: ";
            Poco::JSON::Stringifier::stringify(toJSON(), std::cout);
            std::cout << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long RouteToTrip::get_route_id() const
    {
        return _route_id;
    }

    long RouteToTrip::get_user_id() const
    {
        return _user_id;
    }

    long &URouteToTrip::route_id()
    {
        return _route_id;
    }

    long &RouteToTrip::user_id()
    {
        return _user_id;
    }
}