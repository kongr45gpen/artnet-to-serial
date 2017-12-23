#include "stdafx.h"
#include "enumser.h"


void main()
{
  //Initialize COM (Required by CEnumerateSerial::UsingWMI)
  HRESULT hr = CoInitialize(nullptr);
  if (FAILED(hr))
  {
    _tprintf(_T("Failed to initialize COM, Error:%x\n"), hr);
    return;  
  }

  //Initialize COM security (Required by CEnumerateSerial::UsingWMI)
  hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
  if (FAILED(hr))
  {
    _tprintf(_T("Failed to initialize COM security, Error:%08X\n"), hr);
    CoUninitialize();
    return;  
  }
  
  CEnumerateSerial::CPortsArray ports;
  CEnumerateSerial::CNamesArray names;
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  size_t i = 0;
  UNREFERENCED_PARAMETER(i);
#elif defined _AFX
  INT_PTR i = 0;
#else
  int i = 0;
#endif

#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE
  _tprintf(_T("CreateFile method reports\n"));
  if (CEnumerateSerial::UsingCreateFile(ports))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingCreateFile failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE

#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE
  _tprintf(_T("QueryDosDevice method reports\n"));
  if (CEnumerateSerial::UsingQueryDosDevice(ports))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingQueryDosDevice failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE

#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG
  _tprintf(_T("GetDefaultCommConfig method reports\n"));
  if (CEnumerateSerial::UsingGetDefaultCommConfig(ports))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingGetDefaultCommConfig failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
  _tprintf(_T("Device Manager (SetupAPI - GUID_DEVINTERFACE_COMPORT) reports\n"));
  if (CEnumerateSerial::UsingSetupAPI1(ports, names))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingSetupAPI1 failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
  _tprintf(_T("Device Manager (SetupAPI - Ports Device information set) reports\n"));
  if (CEnumerateSerial::UsingSetupAPI2(ports, names))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingSetupAPI2 failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2

#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS
  _tprintf(_T("EnumPorts method reports\n"));
  if (CEnumerateSerial::UsingEnumPorts(ports, names))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i = 0; i<ports.size(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
  #else
    for (i = 0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingEnumPorts failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS

#ifndef NO_CENUMERATESERIAL_USING_WMI
  _tprintf(_T("WMI method reports\n"));
  hr = CEnumerateSerial::UsingWMI(ports, names);
  if (SUCCEEDED(hr))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingWMI failed, Error:%08X\n"), hr);
#endif //#ifndef NO_CENUMERATESERIAL_USING_WMI

#ifndef NO_CENUMERATESERIAL_USING_COMDB
  _tprintf(_T("ComDB method reports\n"));
  if (CEnumerateSerial::UsingComDB(ports))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<ports.size(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #else
    for (i=0; i<ports.GetSize(); i++)
      _tprintf(_T("COM%u\n"), ports[i]);
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingComDB failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_COMDB

#ifndef NO_CENUMERATESERIAL_USING_REGISTRY
  _tprintf(_T("Registry method reports\n"));
  if (CEnumerateSerial::UsingRegistry(names))
  {
  #ifndef CENUMERATESERIAL_MFC_EXTENSIONS
    for (i=0; i<names.size(); i++)
      _tprintf(_T("%s\n"), names[i].c_str());
  #else
    for (i=0; i<names.GetSize(); i++)
      _tprintf(_T("%s\n"), names[i].GetString());
  #endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
  }
  else
    _tprintf(_T("CEnumerateSerial::UsingRegistry failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_REGISTRY

  //Close down COM
  CoUninitialize();
}