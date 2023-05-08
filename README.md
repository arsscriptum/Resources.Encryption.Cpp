# ResCrypt - Resource Encryption Tool

Windows executables contains various resources of different types (images, icons, strings and other assets...). They are use for a lot of features, like localisation. he system makes it easy for developers to easily get an asset reference programatically.

The issue I have with this is that when reverse-engineering an executable, a dev can list the resources in the exe to get a good idea of some behavior of the app. For example:

List of auto-elevate processes in Windows 10 after executing the command [strings.exe](https://learn.microsoft.com/en-us/sysinternals/downloads/strings): 

```
     strings.exe –s *.exe | findstr /I "<autoElevate>true</autoElevate>"
```

![Img]()

Or list the registry keys that are accessed by the executable...

## Resource Strings Encryption

This tool provides the functionality to encrypt resources and decrypt them on the fly while using the app.

...