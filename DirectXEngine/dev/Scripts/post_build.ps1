#copy libraries to build directory

param ($Platform, $Config, $OutDir)

# .... i love powershell
# didn't found proper way to pass $DevEnvDir var without all this quotes
function Fix-Parameter ($Param) {
	return $Param -replace "'", ""
}

$Platform = Fix-Parameter $Platform
$Config = Fix-Parameter $Config
$OutDir = Fix-Parameter $OutDir

Copy-Item "$PSScriptRoot\..\Libs\$Platform\$Config\*" "$OutDir" -Recurse
Copy-Item "$PSScriptRoot\..\Libs\Any\*" "$OutDir" -Recurse
