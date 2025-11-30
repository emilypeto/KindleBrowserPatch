# Kindle Browser Patch
This is a patch for the built-in web browser on Kindle devices. It provides the following features:
- Remove the restriction on what kind of filetypes you can download (downloads are found in `/mnt/us/documents`)
- Remove the restriction on what protocols you can browse, enabling the use of `file://`
- Option to change the search engine to DuckDuckGo, FrogFind or Bing. The text still says "Search with Google", but in reality the search engine will change.

It likely works on all Kindle devices running firmware >= 5.16.4. In any case, there is very little risk in giving it a try. Please edit this README if you get it working on your device.

Known working devices:
- KS (5.18.5.0.1 && 5.17.3)
- PW6 (5.18.1)
- PW5 (5.17.0)
- PW4 (5.17.1.0.3)
- KT5 (5.17.1.0.3)
- KOA3 (5.17.1.0.3)

## Installation
Prerequisites:
- Jailbreak with root
- KUAL

Method:
1. Download the [latest release](https://github.com/emilypeto/KindleBrowserPatch/releases/download/v2.0.1/kindle_browser_patch-2.0.1-armhf.zip) and extract it to `/mnt/us/extensions` on your Kindle, such that you now have a folder at `/mnt/us/extensions/kindle_browser_patch`. It must be named exactly this or it won't install.
2. Open KUAL and select Kindle Browser Patch --> Install, selecting the search engine of your choice.
3. Don't touch anything while it installs. You will see log messages at the top of the screen.
4. If the patch succeeds, the browser will open and you can test it out. Otherwise, you will see a message saying `Failed to install` at the top of the screen. If this is the case, proceed to the Troubleshooting section below.
5. If you successfully installed it, and you wish to uninstall, you MUST do so through the KUAL menu first. Do not simply delete the files off your device, or you will be left without a functional browser until you put the files back on and uninstall.

## Troubleshooting
First, double check that your device is on firmware >= `5.16.4`. This can be found by going to Kindle settings --> 3 dot menu --> Device info.

If it is, please follow these steps to submit a bug report:
1. Open the KUAL menu and select Kindle Browser Patch --> Bug Report
2. Don't touch anything and give it some time to generate the report. You will see messages at the top of the screen, letting you know when it's finished.
3. Plug your Kindle into your computer, and transfer `bugreport.tar.gz` from the root of its storage to your computer. You may delete the file from your Kindle after it has been copied over.
4. Upload it to an external file host such as Dropbox or Google Drive
5. [Create an issue](https://github.com/emilypeto/KindleBrowserPatch/issues/new) with the following information:
- Your [device nickname](https://wiki.mobileread.com/wiki/Kindle_Serial_Numbers) such as `PW5` or `KOA2`
- Your device firmware version, which can be found in Kindle settings --> 3 dot menu --> Device info
- A link to a `bugreport.tar.gz`

## Compiling
Build [koxtoolchain](https://github.com/koreader/koxtoolchain) for the `kindlehf` platform, then compile `kindle_browser_patch.c` using `~/x-tools/arm-kindlehf-linux-gnueabihf/bin/arm-kindlehf-linux-gnueabihf-gcc`. Place the compiled binary in `extensions/kindle_browser_patch/bin`.

## Technical explanation
Please see [this post](https://www.mobileread.com/forums/showpost.php?p=4495677&postcount=7) and [this post](https://www.mobileread.com/forums/showpost.php?p=4546095&postcount=18) on the MobileRead forums.

## Notes
The ability to change search engines is intended to remedy the issue of Google search not working on Kindle. Google ended support for older browsers, leaving many users with the following error message:
```
Update your browser
Your browser isn't supported anymore. To continue your search, upgrade to a recent version.
```
