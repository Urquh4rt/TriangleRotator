$WebClient = New-Object System.Net.WebClient

function DownloadLibrary {
    param (
        $DownloadLink
    )    
    $WebClient.DownloadFile($DownloadLink,"C:\VSlibs\tmp.zip")
    Expand-Archive C:\VSlibs\tmp.zip C:\VSlibs
    mv C:\VSlibs\tmp\*\inlude\* .\SDL\include
    mv C:\VSlibs\tmp\*\lib\x64\* .\SDL\lib
    cp .\SDL\lib\*.dll .\out\build\x64-Debug
    Remove-Item C:\VSlibs\tmp.zip
}

DownloadLibrary -DownloadLink "https://www.libsdl.org/release/SDL2-devel-2.0.22-VC.zip"
DownloadLibrary -DownloadLink "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.1/SDL2_image-devel-2.6.1-VC.zip"
DownloadLibrary -DownloadLink "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.0/SDL2_ttf-devel-2.20.0-VC.zip"
