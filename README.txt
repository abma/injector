# About
injector creates a hook for GetComputerNameA which returns a fake value for the computername.

# COMPILE:

to compile use cmake / tdm-gcc / ninja. configure with
  cmake -G Ninja
and compile with
  ninja

# USAGE
Usage: inject.exe program <parameters>
Installs a hook to the program, that fakes the System call GetComputerNameA which returns the env var of CLIENTNAME or COMPUTERNAME if set
can be used to fake the computername to the clientname on a terminal server

to see results:
  set COMPUTERNAME=fakename
  test.exe
  inject.exe test.exe

# LICENSE

GPLv3 and later
