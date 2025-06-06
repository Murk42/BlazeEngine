#include "Internet.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <nlohmann/json.hpp>

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8Impl.h"
#include "BlazeEngineCore/File/Stream/BufferStream.h"

using namespace Blaze;

static std::string extractDomain(const std::string& url) {
	std::regex domainRegex(R"(^(?:https?:\/\/)?(?:www\.)?([^\/]+))");
	std::smatch match;

	if (std::regex_search(url, match, domainRegex) && match.size() > 1) {
		return match[0];
	}
	return "";
}


std::string GetImageURL()
{
	httplib::Client client("https://api.nasa.gov");

	auto result = client.Get("/planetary/apod?api_key=PWg3OLCprtvphvJXuAvT3wfc5QahRfsqiB4lxJ1t&count=1&thumbs=True");

	if (!result)
	{
		auto err = result.error();
		Debug::Logger::LogError("Client", "HTTP error: {}\n", httplib::to_string(err));
		return { };
	}

	if (result->status != httplib::StatusCode::OK_200)
	{
		Debug::Logger::LogError("Client", "HTTP status {} \"{}\"\n", result->status, result->reason);
		return { };
	}

	nlohmann::json json = nlohmann::json::parse(result->body);
	return json[0]["url"];
}

static bool GetImageData(const std::string& imageURL, WriteStream& stream, StringUTF8& imageMIMEType)
{
	std::string domain = extractDomain(imageURL);
	std::string path = imageURL.substr(domain.size());

	httplib::Client client(domain);

	auto result = client.Get(path, [&](const char* data, uintMem data_length) {
		stream.Write(data, data_length);
		return true;
		});

	if (!result)
	{
		auto err = result.error();
		Debug::Logger::LogError("Client", "HTTP error: {}\n", httplib::to_string(err));
		return false;
	}

	if (result->status != httplib::StatusCode::OK_200)
	{
		Debug::Logger::LogError("Client", "HTTP status {} \"{}\"\n", result->status, result->reason);
		return false;
	}

	auto contentTypeIt = result->headers.find("Content-Type");

	if (contentTypeIt == result->headers.end())
	{
		Debug::Logger::LogError("Client", "The HTTP response doesn't have a \"Content-Type\" header");
		return false;
	}

	imageMIMEType = StringViewUTF8(contentTypeIt->second.data(), contentTypeIt->second.size());

	return true;
}

Blaze::Bitmap LoadAPOD()
{
	std::string imageURL = GetImageURL();

	if (imageURL.empty())
		return {};

	BufferStream stream;
	StringUTF8 imageMIMEType;

	if (!GetImageData(imageURL, stream, imageMIMEType))
		return {};

	if (stream.Empty())
		return {};

	Console::Write("Loading image with MIME \"{}\"\n", imageMIMEType);

	Bitmap bitmap;
	if (!bitmap.LoadWithMIME(stream, imageMIMEType, true))
		return {};

	return bitmap;	
}