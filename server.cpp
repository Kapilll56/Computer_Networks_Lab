#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

void handle_request(tcp::socket& socket) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    http::read(socket, buffer, req);

    std::cout << "Received body size: " << req.body().size() << " bytes" << std::endl;

    if (req.method() == http::verb::post && req.target() == "/upload") {
        std::string image_data = req.body();
        if (image_data.size() > 0) {
            std::ofstream file("/home/nitdelhi/Desktop/uploaded_image.bmp", std::ios::binary);
            file.write(image_data.data(), image_data.size());
            std::cout << "Image saved: uploaded_image.bmp" << std::endl;
        } else {
            std::cout << "No image data received" << std::endl;
        }


        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "C++ HTTP Server");
        res.set(http::field::content_type, "text/plain");
        res.body() = "Image uploaded successfully!";
        res.prepare_payload();
        http::write(socket, res);
    } else {

        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Not Found";
        res.prepare_payload();
        http::write(socket, res);
    }
}

int main() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 5000));
    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        handle_request(socket);
    }
    return 0;
}

