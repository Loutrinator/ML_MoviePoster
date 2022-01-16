@echo off
set VCPKG_DEFAULT_TRIPLET=x64-windows-static
git clone https://github.com/Microsoft/vcpkg
cd vcpkg
git checkout b934807c799fd84c2e1e168b8d3472d4904a6f90
call bootstrap-vcpkg.bat
vcpkg install nlohmann-json
rmdir /s /q .git
cd ..
pause