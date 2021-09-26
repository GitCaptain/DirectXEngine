# place all includes and libraries to the specific directories before the build

param ($DevEnvDir, $Platform, $Config)

# .... i love powershell
# didn't found proper way to pass $DevEnvDir var without all this quotes
function Fix-Parameter ($Param) {
	return $Param -replace "'", ""
}

$DevEnvDir = Fix-Parameter $DevEnvDir
$Platform = Fix-Parameter $Platform
$Config = Fix-Parameter $Config

mkdir -Force "$PSScriptRoot\..\Libs\"
mkdir -Force "$PSScriptRoot\..\Libs\Any"
mkdir -Force "$PSScriptRoot\..\Libs\$Platform\$Config"

& $PSScriptRoot\prepare_assimp.ps1 -DevEnvDir $DevEnvDir -Platform $Platform -Config $Config
& $PSScriptRoot\prepare_dx.ps1 -Platform $Platform -Config $Config
& $PSScriptRoot\prepare_imgui.ps1
