..\..\Tools\leg.exe -oMessageCompiler.cpp MessageCompiler.leg && cl /nologo /wd4551 /wd4530  MessageCompiler.cpp && MessageCompiler.exe < Test.idl