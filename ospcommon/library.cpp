// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "library.h"
#include "FileName.h"
#include "sysinfo.h"

// std
#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <sys/times.h>
#  include <dlfcn.h>
#endif


namespace ospcommon {

  Library::Library(const std::string& name)
  {
#ifdef OSPRAY_TARGET_MIC
    std::string file = name+"_mic";
#else
    std::string file = name;
#endif
#ifdef _WIN32
    std::string fullName = file+".dll";
    FileName executable = getExecutableFileName();
    lib = LoadLibrary((executable.path() + fullName).c_str());
#else
#if defined(__MACOSX__)
    std::string fullName = "lib"+file+".dylib";
#else
    std::string fullName = "lib"+file+".so";
#endif
    lib = dlopen(fullName.c_str(), RTLD_NOW);
    if (!lib) {
      FileName executable = getExecutableFileName();
      lib = dlopen((executable.path() + fullName).c_str(), RTLD_NOW);
    }
#endif

    if (lib == NULL) {
#ifdef _WIN32
      // TODO: Must use GetLastError and FormatMessage on windows
      // to log out the error that occurred when calling LoadLibrary
      throw std::runtime_error("could not open module lib "+name);
#else
      std::string error = dlerror();
      throw std::runtime_error("could not open module lib "+name
          +" due to "+error);
#endif
    }
  }

  Library::Library(void* const lib) : lib(lib) {};

  void* Library::getSymbol(const std::string& sym) const
  {
#ifdef _WIN32
    return GetProcAddress((HMODULE)lib, sym.c_str());
#else
    return dlsym(lib, sym.c_str());
#endif
  }


  LibraryRepository* LibraryRepository::instance = NULL;

  LibraryRepository* LibraryRepository::getInstance()
  {
    if (instance == NULL)
      instance = new LibraryRepository;

    return instance;
  }

  void LibraryRepository::add(const std::string& name)
  {
    if (repo.find(name) != repo.end())
      return; // lib already loaded.

    repo[name] = new Library(name);
  }

  void* LibraryRepository::getSymbol(const std::string& name) const
  {
    void *sym = NULL;
    for (auto lib = repo.cbegin(); sym == NULL && lib != repo.end(); ++lib)
      sym = lib->second->getSymbol(name);

    return sym;
  }

  LibraryRepository::LibraryRepository()
  {
    // already populate the repo with "virtual" libs, representing the default OSPRay core lib
#ifdef _WIN32
    // look in exe (i.e. when OSPRay is linked statically into the application)
    repo["exedefault"] = new Library(GetModuleHandle(0));

    // look in ospray.dll (i.e. when linked dynamically)
#if 0
    // we cannot get a function from ospray.dll, because this would create a
    // cyclic dependency between ospray.dll and ospray_common.dll

    // only works when ospray_common is liked statically into ospray
    const void * functionInOSPRayDLL = ospcommon::getSymbol;
    // get handle to current dll via a known function
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(functionInOSPRayDLL, &mbi, sizeof(mbi));
    repo["dlldefault"] = new Library(mbi.AllocationBase);
#else
    repo["ospray"] = new Library(std::string("ospray"));
#endif
#else
    repo["ospray"] = new Library(RTLD_DEFAULT);
#endif
  }
}
