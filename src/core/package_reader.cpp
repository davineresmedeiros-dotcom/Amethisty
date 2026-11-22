#include "package_reader.hpp"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <zlib.h>
namespace amethisty { namespace {
constexpr size_t AZ=128u*1024u*1024u, AF=32u*1024u*1024u, AT=64u*1024u*1024u, AM=64u*1024u, AE=128;
struct E{std::string n;unsigned m=0,f=0,crc=0,cs=0,us=0,off=0;bool dir=false;};
unsigned r16(const std::vector<unsigned char>&b,size_t p){return unsigned(b[p])|(unsigned(b[p+1])<<8);}
unsigned r32(const std::vector<unsigned char>&b,size_t p){return r16(b,p)|(r16(b,p+2)<<16);}
bool range(size_t p,size_t n,size_t lim){return p<=lim&&n<=lim-p;}
bool safe(const std::string&s,bool&d){d=!s.empty()&&s.back()=='/';if(s.empty()||s.size()>1024||s[0]=='/'||s.find('\\')!=s.npos||s.find(':')!=s.npos)return false;size_t p=0,l=d?s.size()-1:s.size();while(p<l){size_t q=s.find('/',p);if(q==s.npos||q>l)q=l;auto a=s.substr(p,q-p);if(a.empty()||a=="."||a=="..")return false;p=q+1;}return true;}
bool read(const std::string&p,std::vector<unsigned char>&b,std::string&e){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f){e="Cannot open package.";return false;}auto n=f.tellg();if(n<22||uint64_t(n)>AZ){e="Package size invalid (max 128 MiB).";return false;}b.resize(size_t(n));f.seekg(0);if(!f.read((char*)b.data(),n)){e="Cannot read package.";return false;}return true;}
bool parse(const std::vector<unsigned char>&b,std::vector<E>&v,size_t&cd,std::string&e){
 size_t low=b.size()>65557?b.size()-65557:0,ep=b.size();for(size_t p=b.size()-22;;--p){if(r32(b,p)==0x06054b50&&p+22+r16(b,p+20)==b.size()){ep=p;break;}if(p==low)break;}if(ep==b.size()){e="ZIP end record missing.";return false;}
 if(r16(b,ep+4)||r16(b,ep+6)||r16(b,ep+8)!=r16(b,ep+10)){e="Multi-disk ZIP unsupported.";return false;}
 unsigned count=r16(b,ep+10),sz=r32(b,ep+12),off=r32(b,ep+16);if(count>AE||count==65535||sz==0xffffffff||off==0xffffffff||!range(off,sz,ep)){e="Invalid ZIP directory/ZIP64.";return false;}
 cd=off;size_t p=off,end=off+sz,total=0;for(unsigned i=0;i<count;i++){if(!range(p,46,end)||r32(b,p)!=0x02014b50){e="Malformed ZIP directory.";return false;}
 unsigned fl=r16(b,p+8),m=r16(b,p+10),cs=r32(b,p+20),us=r32(b,p+24),nl=r16(b,p+28),xl=r16(b,p+30),cl=r16(b,p+32),ext=r32(b,p+38),lo=r32(b,p+42);size_t rec=46u+nl+xl+cl;
 if(!nl||nl>1024||!range(p,rec,end)||(fl&~0x0808u)||(m!=0&&m!=8)||cs==0xffffffff||us==0xffffffff||lo==0xffffffff||cs>AF||us>AF||total>AT-us){e="Unsupported/oversized ZIP entry.";return false;}
 E x;x.n.assign((char*)b.data()+p+46,nl);if(!safe(x.n,x.dir)){e="Unsafe ZIP path.";return false;}unsigned t=(ext>>16)&0170000u;if(t==0120000u||(t&&t!=0100000u&&t!=0040000u)){e="ZIP symlink/special file rejected.";return false;}if(x.dir&&us){e="Directory entry contains data.";return false;}for(auto&a:v)if(a.n==x.n){e="Duplicate ZIP entry.";return false;}x.f=fl;x.m=m;x.crc=r32(b,p+16);x.cs=cs;x.us=us;x.off=lo;v.push_back(x);total+=us;p+=rec;}
 if(p!=end){e="ZIP directory size mismatch.";return false;}return true;
}
bool bytesOf(const std::vector<unsigned char>&b,const E&x,size_t cd,size_t max,std::vector<unsigned char>&o,std::string&e){
 if(x.dir||x.us>max||!range(x.off,30,cd)||r32(b,x.off)!=0x04034b50){e="Invalid ZIP local header/size.";return false;}size_t p=x.off;unsigned f=r16(b,p+6),m=r16(b,p+8),nl=r16(b,p+26),xl=r16(b,p+28),d=p+30u+nl+xl;
 if(f!=x.f||m!=x.m||nl!=x.n.size()||!range(p+30,nl+xl,cd)||memcmp(b.data()+p+30,x.n.data(),nl)||!range(d,x.cs,cd)){e="ZIP local header mismatch.";return false;}
 o.assign(x.us,0);auto*in=b.data()+d;if(x.m==0){if(x.cs!=x.us){e="Stored entry size mismatch.";return false;}if(x.us)memcpy(o.data(),in,x.us);}else{z_stream z{};z.next_in=const_cast<Bytef*>((const Bytef*)in);z.avail_in=x.cs;z.next_out=(Bytef*)o.data();z.avail_out=x.us;if(inflateInit2(&z,-MAX_WBITS)!=Z_OK){e="Deflate init failed.";return false;}int r=inflate(&z,Z_FINISH);bool ok=r==Z_STREAM_END&&z.total_in==x.cs&&z.total_out==x.us;inflateEnd(&z);if(!ok){e="Invalid deflate stream.";return false;}}
 if(uint32_t(crc32(0L,o.empty()?Z_NULL:o.data(),uInt(o.size())))!=x.crc){e="ZIP CRC mismatch.";return false;}return true;
}
bool makeDirs(const std::string&p,std::string&e){if(p.empty()||p[0]!='/'){e="Extraction root must be absolute.";return false;}std::string cur="/";size_t i=1;while(i<p.size()){size_t j=p.find('/',i);if(j==p.npos)j=p.size();auto a=p.substr(i,j-i);if(a.empty()||a=="."||a==".."){e="Invalid extraction path.";return false;}if(cur.size()>1)cur+='/';cur+=a;if(mkdir(cur.c_str(),0700)&&errno!=EEXIST){e="Cannot create private directory.";return false;}struct stat st{};if(lstat(cur.c_str(),&st)||!S_ISDIR(st.st_mode)||S_ISLNK(st.st_mode)){e="Extraction path symlink/non-directory.";return false;}i=j+1;}return true;}
bool writeFile(const std::string&p,const std::vector<unsigned char>&b,std::string&e){int fd=open(p.c_str(),O_WRONLY|O_CREAT|O_TRUNC
#ifdef O_NOFOLLOW
|O_NOFOLLOW
#endif
,0600);if(fd<0){e="Cannot create extracted library.";return false;}size_t i=0;while(i<b.size()){ssize_t n=write(fd,b.data()+i,b.size()-i);if(n<=0){close(fd);unlink(p.c_str());e="Library write failed.";return false;}i+=size_t(n);}int a=fsync(fd),z=close(fd);if(a||z){unlink(p.c_str());e="Could not finalize library.";return false;}return true;}
}}

bool amethisty::readPackageManifest(const std::string&path,ModManifest&manifest,std::string&error){
 manifest=ModManifest{};error.clear();std::vector<unsigned char>b;if(!read(path,b,error))return false;
 std::vector<E>es;size_t cd=0;if(!parse(b,es,cd,error))return false;
 const E*man=nullptr;for(auto&x:es)if(x.n=="manifest.json"&&!x.dir)man=&x;
 if(!man||man->us>AM){error="Root manifest.json missing/too large.";return false;}
 std::vector<unsigned char>mb;if(!bytesOf(b,*man,cd,AM,mb,error))return false;
 return parseManifest(std::string(mb.begin(),mb.end()),manifest,error);
}

bool amethisty::validateAndExtractPackage(const std::string&path,const std::string&privateRoot,ExtractedPackage&out,std::string&error){
 out=ExtractedPackage{};error.clear();std::vector<unsigned char>b;if(!read(path,b,error))return false;std::vector<E>es;size_t cd=0;if(!parse(b,es,cd,error))return false;
 const E*man=nullptr;for(auto&x:es)if(x.n=="manifest.json"&&!x.dir)man=&x;if(!man||man->us>AM){error="Root manifest.json missing/too large.";return false;}std::vector<unsigned char>mb;if(!bytesOf(b,*man,cd,AM,mb,error))return false;if(!parseManifest(std::string(mb.begin(),mb.end()),out.manifest,error))return false;
 const E*lib=nullptr;for(auto&x:es)if(x.n==out.manifest.library&&!x.dir)lib=&x;if(!lib){error="Manifest library missing.";return false;}std::vector<unsigned char>data;if(!bytesOf(b,*lib,cd,AF,data,error))return false;
 std::string root=privateRoot;while(root.size()>1&&root.back()=='/')root.pop_back();std::string mod=root+"/"+out.manifest.id,rel=out.manifest.library;auto slash=rel.rfind('/');std::string parent=slash==rel.npos?mod:mod+"/"+rel.substr(0,slash);if(!makeDirs(parent,error))return false;out.libraryPath=mod+"/"+rel;if(!writeFile(out.libraryPath,data,error)){out.libraryPath.clear();return false;}return true;
}