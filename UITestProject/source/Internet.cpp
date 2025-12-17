#include "Internet.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <nlohmann/json.hpp>

#include "BlazeEngine/Console/Console.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/Common/BufferStream.h"

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
		auto err = httplib::to_string(result.error());
		Debug::Logger::LogError("Client", Format("HTTP error: {}\n", StringView(err.data(), err.size())));
		return { };
	}

	if (result->status != httplib::StatusCode::OK_200)
	{
		Debug::Logger::LogError("Client", Format("HTTP status {} \"{}\"\n", result->status, StringView(result->reason.data(), result->reason.size())));
		return { };
	}

	nlohmann::json json = nlohmann::json::parse(result->body);
	return json[0]["url"];
}

static bool GetImageData(const std::string& imageURL, WriteStream& stream, u8String& imageMIMEType)
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
		auto err = httplib::to_string(result.error());
		Debug::Logger::LogError("Client", Format("HTTP error: {}\n", StringView(err.data(), err.size())));
		return false;
	}

	if (result->status != httplib::StatusCode::OK_200)
	{
		Debug::Logger::LogError("Client", Format("HTTP status {} \"{}\"\n", result->status, StringView(result->reason.data(), result->reason.size())));
		return false;
	}

	auto contentTypeIt = result->headers.find("Content-Type");

	if (contentTypeIt == result->headers.end())
	{
		Debug::Logger::LogError("Client", Format("The HTTP response doesn't have a \"Content-Type\" header"));
		return false;
	}

	imageMIMEType = u8StringView(contentTypeIt->second.data(), contentTypeIt->second.size());

	return true;
}

Blaze::Bitmap LoadAPOD()
{
	std::string imageURL = GetImageURL();

	if (imageURL.empty())
		return {};

	BufferStream stream;
	u8String imageMIMEType;

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