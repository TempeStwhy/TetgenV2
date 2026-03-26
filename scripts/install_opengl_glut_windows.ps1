param(
  [string]$VcpkgRoot = "$env:USERPROFILE\vcpkg"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $VcpkgRoot)) {
  git clone https://github.com/microsoft/vcpkg $VcpkgRoot
  & "$VcpkgRoot\bootstrap-vcpkg.bat"
}

& "$VcpkgRoot\vcpkg.exe" install freeglut:x64-windows

Write-Host "Installed freeglut via vcpkg."
Write-Host "Configure CMake with:"
Write-Host "  -DCMAKE_TOOLCHAIN_FILE=$VcpkgRoot\scripts\buildsystems\vcpkg.cmake"
