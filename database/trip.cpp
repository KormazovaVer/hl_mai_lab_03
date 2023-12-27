#include "trip.h"
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

    void Trip::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Trip` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`route_id` INT NOT NULL,"
                        << "`user_id` INT NOT NULL,"
                        << "`trip` VARCHAR(1024) NOT NULL,"
                        << "PRIMARY KEY (`id`))-- sharding:0",
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
        std::cout << "Entity Trip initiated\n";
    }

    Poco::JSON::Object::Ptr Trip::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("route_id", _route_id);
        root->set("user_id", _user_id);
        root->set("trip", _trip);

        return root;
    }

    Trip Trip::fromJSON(const std::string &str)
    {
        Trip trip;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        trip.id() = object->getValue<long>("id");
        trip.route_id() = object->getValue<long>("route_id");
        trip.user_id() = object->getValue<long>("user_id");
        trip.trip() = object->getValue<std::string>("trip");

        return trip;
    }

    std::optional<Trip> Trip::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Trip a;
            select << "SELECT id, route_id, user_id, trip FROM Trip where id=?-- sharding:0",   
                into(a._id),
                into(a._route_id),
                into(a._user_id),
                into(a._trip),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
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

    void Trip::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Trip (route_id, user_id, trip) VALUES(?, ?, ?)-- sharding:0",
                use(_route_id),
                use(_user_id),
                use(_trip);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()-- sharding:0",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
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

    long Trip::get_id() const
    {
        return _id;
    }

    long Trip::get_route_id() const
    {
        return _route_id;
    }

    long Trip::get_user_id() const
    {
        return _user_id;
    }

    const std::string &Trip::get_trip() const
    {
        return _trip;
    }

    long &Trip::id()
    {
        return _id;
    }

    long &Trip::trip_id()
    {
        return _trip_id;
    }

    long &Trip::user_id()
    {
        return _user_id;
    }

    std::string &Trip::trip()
    {
        return _trip;
    }

    
    std::vector<Trip> trip::read_by_route_id(long route_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Trip> result;
            Trip a;

            select << "SELECT id, route_id, user_id, trip FROM Trip WHERE Trip.route_id=?-- sharding:0",
                into(a.id()),
                into(a.route_id()),
                into(a.user_id()),
                into(a.trip()),
                use(route_id),
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
}