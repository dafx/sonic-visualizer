@ECHO OFF
SET WIXPATH="C:\Program Files (x86)\WiX Toolset v3.7\bin"
DEL sonic-visualiser.msi
%WIXPATH%\candle.exe -v sonic-visualiser.wxs
%WIXPATH%\light.exe -b ..\.. -ext WixUIExtension -v sonic-visualiser.wixobj
PAUSE
DEL sonic-visualiser.wixobj
DEL sonic-visualiser.wixpdb