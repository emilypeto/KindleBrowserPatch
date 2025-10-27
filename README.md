# Kindle Browser Patch
This is a patch for the built-in web browser on Kindle devices. It provides the following features:
- Remove the restriction on what kind of filetypes you can download (downloads are found in `/mnt/us/documents`)
- Remove the restriction on what protocols you can browse, enabling the use of `file://`

It likely works on all Kindle devices running firmware >= 5.16.4. In any case, there is very little risk in giving it a try. Please edit this README if you get it working on your device.

Known working devices:
- PW6 (5.18.1)
- KT5 (Winterbreak, 5.17.1.0.3)
- KOA3 (Winterbreak, 5.17.1.0.3)
- KS (Winterbreak, 5.17.3)
- PW4 (Winterbreak, 5.17.1.0.3)

Known NOT WORKING devices:
- PW5 (WinterBreak, 5.17.0, Paperwhite SE 12th Gen)
## Installation
Prerequisites:
- Jailbreak with root
- KUAL

Method:
1. Download the [latest release](https://github.com/emilypeto/KindleBrowserPatch/releases/download/v1.0.2/kindle_browser_patch-1.0.2-armhf.zip) and extract it to `/mnt/us/extensions` on your Kindle, such that you now have a folder at `/mnt/us/extensions/kindle_browser_patch`. It must be named exactly this or it won't install.
2. Open KUAL and select Kindle Browser Patch --> Install
3. Don't touch anything for about 3 minutes while it installs. You will not see any kind of visual indication of progress (sorry, I was too lazy to make a progress bar).
4. When it's finished, the browser should open automatically if it was successful. Browse to `file:///` to test it out and see that it's working! If after 3 minutes the browser doesn't appear, your device likely isn't compatible. There is no harm to your device in this case, simply delete the files.
5. If you successfully installed it, and you wish to uninstall, you MUST do so through the KUAL menu first. Do not simply delete the files off your device, or you will be left without a functional browser until you put the files back on and uninstall.

## Troubleshooting
Check the install/uninstall logs at `/mnt/us/extensions/kindle_browser_patch/kindle_browser_patch.log`.
## Technical explanation
Please visit [https://www.mobileread.com/forums/showthread.php?p=4495677#post4495677](https://www.mobileread.com/forums/showthread.php?p=4495677#post4495677).
