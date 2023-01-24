# PowerShell Scripting
### Installing PowerShell
To determine if you have PowerShell installed, open up a command prompt, then enter the following command:
```
powershell
```

If this command does not work, go to [this link](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell-on-windows?view=powershell-7.3) to install PowerShell. You can even use PowerShell on Linux or MacOS. Make sure PowerShell can be opened before proceeding.

### Installing Terminal
If you are on Windows, using the Windows Terminal application has many benefits as opposed to a normal PowerShell window. You can find that download [here](https://apps.microsoft.com/store/detail/windows-terminal/9N0DX20HK701?hl=en-us&gl=us).

### Using the module
To use the module, you can set it up to be automatically imported when you start up PowerShell. In a PowerShell window open to the current directory, run the install script using the following command:
```
.\install
```
Make sure it does not have any errors.

### Testing your own code
If you would like to revise and test the module, use the following sequence of commands.
1. Navigate to the current directory in powershell.
2. Import the module: `Import-Module .\utils.psm1`.
3. Get the name of the module: `Get-Module`. It should match the name of the file.
4. If something does not work, or you make any changes, you have to remove, then re-import the module. `Remove-Module utils` to remove, then `Import-Module .\utils.psm1` to re-import.

### Helpful References
* [Microsoft documentation](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.management/copy-item?view=powershell-7.2), click through on the left bar to see a whole bunch of functions.
* [Condition documentation](https://learn.microsoft.com/en-us/powershell/scripting/learn/deep-dives/everything-about-if?view=powershell-7.3)
* [Conditionals with an example](https://adamtheautomator.com/powershell-if-else/)
