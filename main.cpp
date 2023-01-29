#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <filesystem>
#include <fstream>
#include <charconv>
#include <cctype>


using boost::asio::ip::udp;
using boost::asio::ip::address;
using TUDPDataPtr = std::shared_ptr<std::vector<char>>;

namespace {

	class UdpSpammerServer {
	public:
		UdpSpammerServer(boost::asio::io_service& io_service, TUDPDataPtr&& messageToSpam)
			: m_socket(io_service)
			, m_spamMessage{ std::move(messageToSpam) }
		{
			m_socket.open(udp::v4());
		}

		void startSpamming()
		{
			spamMessage();
		}
	private:

		void spamMessage() {
				udp::endpoint remoteEndpoint{ udp::endpoint{address::from_string("127.0.0.1"),5555} };
				m_socket.async_send_to(boost::asio::buffer(*m_spamMessage), remoteEndpoint,
					
					boost::bind(&UdpSpammerServer::sendReady, this, m_spamMessage,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)
				);
		}

		void sendReady(TUDPDataPtr message,
			const boost::system::error_code& ec,
			std::size_t bytes_transferred)
		{
			spamMessage();
		}
		TUDPDataPtr m_spamMessage;
		udp::socket m_socket;
	};

}  // namespace


TUDPDataPtr readSpamData(const std::filesystem::path& filePathData)
{

	std::fstream fileData{ filePathData };
	std::string line;
	std::vector<char> resultVector{};
	while (std::getline(fileData, line))
	{
		if (line.empty())
			continue;

		const bool canParseAsDigit{ std::isdigit(static_cast<unsigned char>(line[0])) != 0 };
		if (!canParseAsDigit)
			continue;
		std::int32_t value{};
		std::string_view charsView{ line };

		auto [ptr, ec] = std::from_chars(charsView.data(), charsView.data() + charsView.length(), value);

		const bool canPushBack{
				ec == std::errc()
		};

		if (canPushBack)
		{
			resultVector.push_back(static_cast<char>(value));
		}

	}

	std::cout << "Parsed samples:" << resultVector.size() << std::endl;

	TUDPDataPtr pData = std::make_shared<std::vector<char>>(resultVector);

	return pData;
}

int main(int argc, char* argv[]) {
	try {
		boost::asio::io_service io_service;
		auto pSpamData{ readSpamData(std::filesystem::path(argv[1])) };

		UdpSpammerServer server{ io_service,std::move(pSpamData) };
		server.startSpamming();
		io_service.run();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}