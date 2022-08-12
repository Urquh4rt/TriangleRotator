$WebClient = New-Object System.Net.WebClient

function DownloadLibrary {
    param (
        $DownloadLink
    )    
    $WebClient.DownloadFile($DownloadLink,"C:\VSlibs\tmp.zip")
    Expand-Archive C:\VSlibs\tmp.zip C:\VSlibs
    Remove-Item C:\VSlibs\tmp.zip
}

DownloadLibrary -DownloadLink "https://www.libsdl.org/release/SDL2-devel-2.0.22-VC.zip"
DownloadLibrary -DownloadLink "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.1/SDL2_image-devel-2.6.1-VC.zip"
DownloadLibrary -DownloadLink "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.0/SDL2_ttf-devel-2.20.0-VC.zip"

Get-ChildItem -Directory "C:\VSlibs" | Foreach-Object {

    #Do something with $_.FullName
        Copy-Item "$($_.FullName)\lib\x64\*.dll" .
    
    }

#$WebClient.DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.22-VC.zip","C:\VSlibs\sdl.zip")
#Expand-Archive -Force C:\VSlibs\sdl.zip C:\VSlibs

#$WebClient.DownloadFile("https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.0/SDL2_ttf-devel-2.20.0-VC.zip","C:\VSlibs\sdl_ttf.zip")
#Expand-Archive -Force C:\VSlibs\sdl_ttf.zip C:\VSlibs

#$WebClient.DownloadFile("https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.1/SDL2_image-devel-2.6.1-VC.zip","C:\VSlibs\sdl_img.zip")
#Expand-Archive -Force C:\VSlibs\sdl_img.zip C:\VSlibs