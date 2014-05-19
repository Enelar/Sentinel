#include <future>
#include <thread>
#include <string>        
#include <iostream>
#include <fstream>
#include <deque>
#include <memory>
#include <sstream>
#include <codecvt>

#define _WIN32_WINDOWS 0x0501
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


using namespace ::std;                                                                      
using namespace ::boost::asio::ip;
using namespace ::boost::asio;

static_assert(sizeof(short) == 2, "Short should be 2 bytes size");

string GetServerAnswer(string addr, int port)
{
  auto TheMessage = async([=]()
  {
    stringstream res;

    auto MinecraftString = [](string origin)
    {
      stringstream res;
      res << short(origin.length());

      wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> convert;
      u16string converted = convert.from_bytes(origin);

      // Get bytearray of utf-16 string
      res << string((char *)converted.c_str());

      return res.str();
    };

    res << "\xFE\x01\xFA";
    res << MinecraftString("MC|PingHost");

    auto hostname = MinecraftString(addr);

    res << short(7 + hostname.length());
    res << char(73);
    res << hostname;
    res << int(port);
    return res.str();
  });

  boost::system::error_code error;
  try
  {
    static boost::asio::io_service io_service;
    tcp::iostream stream(addr, boost::lexical_cast<string>(port));
    stream << TheMessage.get();
    stream.flush();
    char kicked;
    short len;
    stream >> kicked >> len;

    u16string reason = [len, &stream]()
    {
      const int size = len * sizeof(short);
      string buf;
      buf.resize(size);

      stream.read(&buf[0], size);
      u16string ret;
      ret.resize(len);
      memcpy_s(&ret[0], size, &buf[0], size);
      return ret;
    }();

    wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> convert;
    return convert.to_bytes(reason);
  }
  catch (exception &e)
  {
    return e.what();
  }
}

struct abort_execution
{

};

void Worker( const string &filename )
{
  ifstream file(filename);

  if (!file.is_open())
    throw abort_execution();

  deque<future<string>> tasks;

  while (!file.eof())
  {
    string address;
    int port;

    tasks.push_back(async(std::launch::async, GetServerAnswer, address, port));
  }

  for (auto &result : tasks)
  {
    result.wait();
    cout << result.get() << endl;
  }
}

void main(int argc, const char* argv[])
{

  if (argc != 2)
  {
    cout << "Unexpected params count: " << argc;
    return;
  }
  try
  {
    Worker(argv[1]);
  }
  catch (abort_execution &)
  {
    cout << "Execution aborted" << endl;
  }
}