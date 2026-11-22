#include "package_reader.hpp"
#include "mod_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <unistd.h>
namespace {
void w16(std::vector<unsigned char>&b,unsigned v){b.push_back(v&255);b.push_back((v>>8)&255);}
void w32(std::vector<unsigned char>&b,unsigned v){w16(b,v&65535);w16(b,v>>16);}
struct File{std::string name,data;};
std::vector<unsigned char> zip(const std::vector<File>&fs){
 std::vector<unsigned char>b;struct C{File f;unsigned crc,offset;};std::vector<C>cs;
 for(auto f:fs){C c{f,unsigned(crc32(0L,(const Bytef*)f.data.data(),uInt(f.data.size()))),unsigned(b.size())};
 w32(b,0x04034b50);w16(b,20);w16(b,0);w16(b,0);w16(b,0);w16(b,0);w32(b,c.crc);w32(b,f.data.size());w32(b,f.data.size());w16(b,f.name.size());w16(b,0);b.insert(b.end(),f.name.begin(),f.name.end());b.insert(b.end(),f.data.begin(),f.data.end());cs.push_back(c);}
 unsigned offset=b.size();for(auto c:cs){w32(b,0x02014b50);w16(b,0x0314);w16(b,20);w16(b,0);w16(b,0);w16(b,0);w16(b,0);w32(b,c.crc);w32(b,c.f.data.size());w32(b,c.f.data.size());w16(b,c.f.name.size());w16(b,0);w16(b,0);w16(b,0);w16(b,0);w32(b,0100644u<<16);w32(b,c.offset);b.insert(b.end(),c.f.name.begin(),c.f.name.end());}
 unsigned size=b.size()-offset;w32(b,0x06054b50);w16(b,0);w16(b,0);w16(b,cs.size());w16(b,cs.size());w32(b,size);w32(b,offset);w16(b,0);return b;
}
std::string manifest(const std::string&lib){return "{\"format\":1,\"id\":\"author.examplemod\",\"name\":\"Example\",\"version\":\"1.0\",\"game\":\"2.2.145\",\"library\":\""+lib+"\"}";}
bool save(const std::string&p,const std::vector<unsigned char>&b){std::ofstream f(p,std::ios::binary);return f&&f.write((const char*)b.data(),b.size());}
bool ok(bool v,const char*msg){if(!v)std::cerr<<"FAILED: "<<msg<<'\n';return v;}
}
int main(){
 char temp[]="/tmp/amethisty-package-XXXXXX";char*d=mkdtemp(temp);if(!d)return 1;std::string root=d,archive=root+"/mods/test.amethyst",payload="fake-library-bytes",error;int fails=0;std::filesystem::create_directories(root+"/mods");
 auto good=zip({{"manifest.json",manifest("mod/libTest.so")},{"mod/libTest.so",payload}});
 fails+=!ok(save(archive,good),"write test ZIP");amethisty::ExtractedPackage result;
 fails+=!ok(amethisty::validateAndExtractPackage(archive,root+"/private",result,error),"valid package accepted");
 fails+=!ok(result.manifest.id=="author.examplemod"&&!result.libraryPath.empty(),"manifest and output path");
 std::ifstream f(result.libraryPath,std::ios::binary);std::string got((std::istreambuf_iterator<char>(f)),{});fails+=!ok(got==payload,"library extracted intact");
 amethisty::ModManager manager;fails+=!ok(manager.scanAndExtract(root+"/mods",root+"/private-manager"),"ModManager package scan");
 const auto& candidates=manager.candidates();fails+=!ok(candidates.size()==1&&candidates[0].valid&&candidates[0].id=="author.examplemod"&&!candidates[0].libraryPath.empty(),"ModManager uses manifest and extracts package");
 auto traversal=zip({{"manifest.json",manifest("mod/../escape.so")},{"mod/../escape.so",payload}});save(archive,traversal);
 fails+=!ok(!amethisty::validateAndExtractPackage(archive,root+"/bad",result,error),"traversal rejected");
 auto missing=zip({{"manifest.json",manifest("mod/missing.so")}});save(archive,missing);
 fails+=!ok(!amethisty::validateAndExtractPackage(archive,root+"/missing",result,error),"missing library rejected");
 std::filesystem::remove_all(root);if(fails)return 1;std::cout<<"Amethisty package reader tests passed.\n";return 0;
}