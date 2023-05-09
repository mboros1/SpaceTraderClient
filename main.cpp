#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/json.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include "register_db.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


const char* myName = "MyTestDudeMan2";

// Performs an HTTP POST and prints the response
boost::json::object do_post(ssl::context& ctx, net::io_context& ioc)
{
    std::string host = "api.spacetraders.io";
    std::string port = "443";
    std::string target = "/v2/register";

    // Setup HTTP POST request
    http::request<http::string_body> req{http::verb::post, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    // Create JSON object
    boost::json::object jv;
    jv["symbol"] = myName;
    jv["faction"] = "COSMIC";

    // Serialize to string
    std::string json_body = boost::json::serialize(jv);

    // Use in HTTP request
    req.body() = json_body;
    req.prepare_payload();

    // Resolve the host
    tcp::resolver resolver{ioc};

    // Make the connection on the IP address we get from a lookup
    ssl::stream<tcp::socket> stream{ioc, ctx};

    if(! SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
    {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }

    auto const results = resolver.resolve(host, port);
    net::connect(stream.next_layer(), results);

    stream.handshake(ssl::stream_base::client);

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Convert the body into a string
    std::string body_str = beast::buffers_to_string(res.body().data());

// Parse the response body into a JSON object
    boost::json::object json_res;
    try {
        boost::json::value val = boost::json::parse(body_str);
        if (val.is_object()) {
            json_res = val.as_object();
        } else {
            std::cerr << "Error: Parsed JSON value is not an object\n";
        }
    } catch(const boost::json::system_error & e) {
        std::cerr << "Error parsing JSON response: " << e.what() << "\n";
    }

    // Gracefully close the stream
    beast::error_code ec;
    stream.shutdown(ec);
    return json_res;
}

int main(int argc, char** argv)
{
    try
    {
        RegisterDb db;

        if (db.hasToken(myName)) {
            std::cout << "Already registered\n";
            std::string token = db.getToken(myName);
            std::cout << token;
            return EXIT_SUCCESS;
        }

        // The io_context is required for all I/O
        net::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx{ssl::context::sslv23_client};

        // These objects perform our I/O
        auto res = do_post(ctx, ioc);


        // The io_context::run() call will block until all asynchronous operations have finished.
        ioc.run();

        std::cout << res << std::endl;
        // get token from object, save to DB with name
        std::string token = res["data"].as_object()["token"].as_string().c_str();

        db.insertToken(myName, token);

    }
    catch(std::exception const& e)
    {
       std::cerr << "Error: " << e.what() << std::endl;
       return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
