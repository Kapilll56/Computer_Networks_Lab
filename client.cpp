#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <fstream>
#include <vector>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::string(buffer.begin(), buffer.end());
}

int main() {
    try {
        // Read the image file from the Desktop
        std::string image_data = read_file("/home/nitdelhi/Desktop/sample_image.bmp");
        
        // Check the size of the image (it should be greater than 0)
        std::cout << "Image size: " << image_data.size() << " bytes" << std::endl;

        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);
        asio::connect(socket, resolver.resolve("127.0.0.1", "5000"));

        // Send the image data to the server
        http::request<http::string_body> req{http::verb::post, "/upload", 11};
        req.set(http::field::host, "127.0.0.1");
        req.set(http::field::content_type, "application/octet-stream");
        req.body() = image_data;
        req.prepare_payload();

        std::cout << "Sending request: " << req.body().size() << " bytes" << std::endl;

        http::write(socket, req);

        // Get the server's response
        http::response<http::string_body> res;
        beast::flat_buffer buffer;
        http::read(socket, buffer, res);

        std::cout << "Server response: " << res.body() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

