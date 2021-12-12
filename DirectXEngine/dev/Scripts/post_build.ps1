param ($Platform, $Config, $OutDir)

# .... i love powershell
# didn't found proper way to pass $DevEnvDir var without all this quotes
function Fix-Parameter ($Param) {
    return $Param -replace "'", ""
}

$Platform = Fix-Parameter $Platform
$Config = Fix-Parameter $Config
$OutDir = Fix-Parameter $OutDir

#copy libraries to build directory
Copy-Item "$PSScriptRoot\..\Libs\$Platform\$Config\*" "$OutDir" -Recurse
Copy-Item "$PSScriptRoot\..\Libs\Any\*" "$OutDir" -Recurse

#copy resources to build directory
$ResourcesDir = "$PSScriptRoot\..\..\Resources"
$ResourceSubdirs = ('Fonts', 'Objects', 'Textures')
foreach ($ResourceSubdir in $ResourceSubdirs) {
    Copy-Item "$ResourcesDir\$ResourceSubdir" "$OutDir\Resources\$ResourceSubdir" -Recurse
}

#copy builded shaders to build directory
# save internal directory structure
$ShadersDir = "$OutDir\$Platform\$Config"
mkdir -Force "$ShadersDir"
Copy-Item "$OutDir\*.cso" "$ShadersDir" -Recurse
