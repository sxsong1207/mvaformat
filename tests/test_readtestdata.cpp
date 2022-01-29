#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

#include "mvaformat.h"

string TESTDATA_DIR = "";

TEST_CASE("TestDataExists")
{
    fs::path TestDataDir(TESTDATA_DIR);
    CHECK(fs::exists(TestDataDir/"version3.mva"));
    CHECK(fs::exists(TestDataDir/"version3_gzstream.mva"));
}

void test_reader(std::string filepath, int* pFormat, uint32_t* pVersion)
{
    MVSA::Scene scene;
    MVSA::MVAIO::SerializeLoad(scene,filepath, pFormat, pVersion);
    CHECK(scene.images.size()==25);
    CHECK(scene.platforms.size()==25);
    CHECK(scene.vertices.size()==5002);
    CHECK(scene.verticesColor.size()==5002);
    CHECK(scene.verticesNormal.size()==0);
    CHECK(scene.mesh.vertices.size()==3547);
    CHECK(scene.mesh.faces.size()==7090);
}

TEST_CASE("TEST_Reader"){
    fs::path TestDataDir(TESTDATA_DIR);
    int Format;
    uint32_t Version;
    test_reader((TestDataDir/"version3.mva"), &Format, &Version);
    CHECK(Format == MVSA::STDIO);
    CHECK(Version == 3);
    test_reader((TestDataDir/"version3_gzstream.mva"), &Format, &Version);
    CHECK(Format == MVSA::GZSTREAM);
    CHECK(Version == 3);
}

int main(int argc, char** argv) {
    doctest::Context context;

    std::vector<std::string> args(argv, argv+argc);
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--testdatadir" && i+1 < args.size()) {
            TESTDATA_DIR = args[i+1];
        }
    }
    
    int res = context.run(); // run
    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests

    int client_stuff_return_code = 0;
    // your program - if the testing framework is integrated in your production code
    
    return res + client_stuff_return_code; // the result from doctest is propagated here as well
}