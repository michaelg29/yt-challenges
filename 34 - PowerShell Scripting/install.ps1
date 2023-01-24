# Set this module name
$MODULE_NAME="utils"

# Get profile
if (-Not (Test-Path $Profile))
{
    New-Item -Path $Profile -Type "file" -Force
}
echo "Profile exists at $Profile"

# Create module directory in profile directory
$dir=(get-item $Profile).Directory.FullName
$dir+="\Modules\$MODULE_NAME"
if (-Not (Test-Path $dir))
{
    New-Item -Path $dir -ItemType "directory"
}
echo "Module directory exists at $dir"

# Copy module script to module directory
Copy-Item .\utils.psm1 -Destination $dir\$MODULE_NAME.psm1

# Automatically import profile on startup
if (-Not ((Get-Content $Profile) -like "*Import-Module $MODULE_NAME*"))
{
    Add-Content -Path $Profile -Value "Import-Module $MODULE_NAME"
}
