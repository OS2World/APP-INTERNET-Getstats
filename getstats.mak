# IBM Developer's Workframe/2 Make File Creation run at 09:36:18 on 04/11/95

# Make File Creation run in directory:
#   D:\DEVEL\GETSTATS;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

GETSTATS.EXE:  \
  getstats.OBJ \
  GETSTATS.MAK
   ICC.EXE @<<
 /Fe"GETSTATS.EXE" dpi32dll.LIB ftpapi.LIB rpc32dll.LIB so32dll.LIB tcp32dll.LIB 
getstats.OBJ
<<

{.}.c.obj:
   ICC.EXE /O /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /O /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /O /C   .\$*.cxx

!include GETSTATS.DEP
