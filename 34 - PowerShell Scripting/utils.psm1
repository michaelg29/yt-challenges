function guid {
    (New-Guid).ToString()
}
Export-ModuleMember -Function guid

function date {
    [System.DateTimeOffset]::Now.ToString()
}
Export-ModuleMember -Function date

function randStr {
param(
    [int] $n = 12
)
    -join ((33..33) + (38..46) + (48..59) + (65..90) + (97..122) | Get-Random -Count $n | % {[char]$_})
}
Export-ModuleMember -Function randStr

function b64encode {
param(
    [string] $str = $null
)
    if ($str.Length -eq 0)
    {
        $str = $input
    }

    # Get bytes
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($str)

    # Encode
    $encoded = [System.Convert]::ToBase64String($bytes)
    Write-Output $encoded
}
Export-ModuleMember -Function b64encode

function b64decode {
param(
    [string] $b64str = $null
)
    if ($b64str.Length -eq 0)
    {
        $b64str = $input
    }

    # Get bytes
    $bytes = [System.Convert]::FromBase64String($b64str)

    # Decode to string
    $decoded = [System.Text.Encoding]::UTF8.GetString($bytes)
    Write-Output $decoded
}
Export-ModuleMember b64decode

function usage {
    Write-Output "Loaded my module"
    Write-Output "usage"
    Write-Output "date"
    Write-Output "randStr"
    Write-Output "b64encode"
    Write-Output "b64decode"
}
Export-ModuleMember -Function usage
