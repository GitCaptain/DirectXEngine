
param ($Platform, $Config)

$deps = ('DirectXTK.lib', 'DirectXTK.pdb')

$dxDir = "$PSScriptRoot\..\..\..\DirectXTK"
$prefix = "$dxDir\Bin\Desktop_2019_Win10\$Platform\$Config"

# copy .lib files
foreach ($dependency in $deps) {
    Copy-Item "$prefix\$dependency" "$PSScriptRoot\..\Libs\$Platform\$Config"
}

#copy include files
$dstDir = "$PSScriptRoot\..\..\ThirdParty\include\DirectXTK"
mkdir -Force "$dstDir"
Copy-Item "$dxDir\Inc\*" "$dstDir" -Recurse
