#include <future>
#include <thread>
#include <string>        
#include <iostream>
#include <fstream>
#include <deque>


using namespace std;

int GetServerOnline(string addr, int port)
{
  return 0;
}

struct abort_execution
{

};

void Worker( const string &filename )
{
  ifstream file(filename);

  if (!file.is_open())
    throw abort_execution();

  deque<future<int>> tasks;
  packaged_task<int(string, int)> task(GetServerOnline);

  while (!file.eof())
  {
    string address;
    int port;

    tasks.push_back(async(std::launch::async, GetServerOnline, address, port));
  }

  while (tasks.size())
  {
    auto &result = tasks.front();

    result.get();
    cout << result.get() << endl;

    tasks.pop_front();
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