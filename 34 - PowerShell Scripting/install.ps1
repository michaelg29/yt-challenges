$MODULE_NAME="utils"

# Get profile
if (-Not (Test-Path $Profile))
{
    New-Item -Path $Profile -Type "file" -Force
}
echo "Profile exists at $Profile"

# Create module directory
$dir=(Get-Item $Profile).Directory.FullName
$dir+="\Modules\$MODULE_NAME"
if (-Not (Test-Path $dir))
{
    New-Item -Path $dir -ItemType "directory"
}
echo "Module directory exists at $dir"

# Copy module to directory
Copy-Item .\$MODULE_NAME.psm1 -Destination $dir\$MODULE_NAME.psm1

# Automatically import profile on startup
$import="Import-Module $MODULE_NAME"
if (-Not ((Get-Content $Profile) -like "*$import*"))
{
    Add-Content -Path $Profile -Value $import
}
