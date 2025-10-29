# Kindle Browser Patch
This is a patch for the built-in web browser on Kindle devices. It provides the following features:
- Remove the restriction on what kind of filetypes you can download (downloads are found in `/mnt/us/documents`)
- Remove the restriction on what protocols you can browse, enabling the use of `file://`

It likely works on all Kindle devices running firmware >= 5.16.4. In any case, there is very little risk in giving it a try. Please edit this README if you get it working on your device.

Known working devices:
- PW6 (5.18.1)
- PW5 (WinterBreak, 5.17.0)
- KT5 (WinterBreak, 5.17.1.0.3)
- KOA3 (WinterBreak, 5.17.1.0.3)
- KS (WinterBreak, 5.17.3)
- PW4 (WinterBreak, 5.17.1.0.3)

## Installation
Prerequisites:
- Jailbreak with root
- KUAL

Method:
1. Download the [latest release](https://github.com/emilypeto/KindleBrowserPatch/releases/download/v1.0.3/kindle_browser_patch-1.0.3-armhf.zip) and extract it to `/mnt/us/extensions` on your Kindle, such that you now have a folder at `/mnt/us/extensions/kindle_browser_patch`. It must be named exactly this or it won't install.
2. Open KUAL and select Kindle Browser Patch --> Install
3. Don't touch anything for about 3 minutes while it installs. You will not see any kind of visual indication of progress (sorry, I was too lazy to make a progress bar).
4. When it's finished, the browser should open automatically if it was successful. Browse to `file:///` to test it out and see that it's working! If after 3 minutes the browser doesn't appear, your device likely isn't compatible. There is no harm to your device in this case, you can simply delete the files. See the troubleshooting section below, I can probably get it working for your device.
5. If you successfully installed it, and you wish to uninstall, you MUST do so through the KUAL menu first. Do not simply delete the files off your device, or you will be left without a functional browser until you put the files back on and uninstall.

## Troubleshooting
### If it won't install
First, check the following:
- Your device is on firmware >= 5.16.4 (Kindle settings --> 3 dot menu --> Device info)
- You followed the above steps correctly, especially the part about waiting 3 minutes without touching anything after pressing install

If the above are true, then follow these steps:
1. If you don't already have kterm installed, [download the latest armhf release](https://github.com/bfabiszewski/kterm/releases) and install it to the extensions folder on your Kindle
2. Open kterm from the KUAL menu on your Kindle, and type the following commands. Some of them may take a couple minutes to complete.
```
mkdir /mnt/us/bugreport
cp -r /usr/bin/chromium /mnt/us/bugreport
cp /usr/bin/browser /mnt/us/bugreport
cp /var/local/appreg.db /mnt/us/bugreport
cp /mnt/us/extensions/kindle_browser_patch/kindle_browser_patch.log /mnt/us/bugreport
tar czf /mnt/us/bugreport.tar.gz /mnt/us/bugreport
rm -r /mnt/us/bugreport
```
3. Plug your Kindle into your computer, and transfer `bugreport.tar.gz` to your computer. You may delete the file from your Kindle after it has been copied over.
4. Upload it to an external file host such as Dropbox or Google Drive
5. Create an issue with the following information
- Your [device nickname](https://wiki.mobileread.com/wiki/Kindle_Serial_Numbers) such as `PW5` or `KOA2`
- The jailbreak you used, such as `WinterBreak` or `AdBreak`
- Your device firmware, which can be found in Kindle settings --> 3 dot menu --> Device info
- A link to a `bugreport.tar.gz`

In most cases, given this information, I'll be able to adjust the code to work for your device.

### If you removed the files without properly uninstalling
Put the files back on your device, then create an empty file named `installed` at `extensions/kindle_browser_patch/installed`

## Compiling
Build [koxtoolchain](https://github.com/koreader/koxtoolchain) for the `kindlehf` platform, then compile `kindle_browser_patch.c` using `~/x-tools/arm-kindlehf-linux-gnueabihf/bin/arm-kindlehf-linux-gnueabihf-gcc`

## Technical explanation
Please visit [https://www.mobileread.com/forums/showthread.php?p=4495677#post4495677](https://www.mobileread.com/forums/showthread.php?p=4495677#post4495677).
