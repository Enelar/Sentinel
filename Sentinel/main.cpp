#include <future>
#include <thread>
#include <string>        
#include <iostream>
#include <fstream>


using namespace std;

int GetServerOnline(string addr, int port)
{
  return 0;
}

struct abort
{

};

void Worker( const string &filename )
{
  ifstream file(filename);

  if (!file.is_open())
  {
    
  }

  packaged_task<int(string, int)> task(GetServerOnline);
  future<int> f = task.get_future();

  thread(move(task), "aaa", 2).detach();

  f.wait();

  cout << "Done" << f.get() << endl;
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
  catch (::abort &)
  {
    cout << "Execution aborted" << endl;
  }
}