#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <fstream>
#include <podofo/podofo.h>

using namespace PoDoFo;
using namespace std;

TEST_CASE("document buffer response", "[document]")
{
  std::ifstream in("../fw9.pdf", std::ifstream::ate);
  long originalLength = in.tellg();
  cout << "File length: " << originalLength << endl;
}
