@echo off
set ENGINE_PATH=%~dp0..
set BGFX_PATH=External/bgfx/bgfx/src
set INCLUDE_PATH=%ENGINE_PATH%/%BGFX_PATH%
shaderc -f %1/fs_%2.sc -o %1/fs_%2.bin --platform windows --type fragment -i %INCLUDE_PATH% --profile ps_5_0 -O 3
shaderc -f %1/vs_%2.sc -o %1/vs_%2.bin --platform windows --type vetex -i %INCLUDE_PATH% --profile vs_5_0 -O 3
shaderc -f %1/fs_%2.sc -o %1/fs_%2.lin.bin --platform linux --type fragment -i %INCLUDE_PATH% --profile 120
shaderc -f %1/vs_%2.sc -o %1/vs_%2.lin.bin --platform linux --type vetex -i %INCLUDE_PATH% --profile 120
shaderc -f %1/fs_%2.sc -o %1/fs_%2.esl.bin --platform nacl --type fragment -i %INCLUDE_PATH%
shaderc -f %1/vs_%2.sc -o %1/vs_%2.esl.bin --platform nacl --type vetex -i %INCLUDE_PATH%
shaderc -f %1/fs_%2.sc -o %1/fs_%2.and.bin --platform android --type fragment -i %INCLUDE_PATH%
shaderc -f %1/vs_%2.sc -o %1/vs_%2.and.bin --platform android --type vetex -i %INCLUDE_PATH%
