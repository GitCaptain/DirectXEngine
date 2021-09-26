
# Run cmake to generate build assimp

param ($DevEnvDir, $Platform, $Config)

$architecture = "x86"
if ( $Platform -ne "x86") {
	$architecture = "${architecture}_${Platform}"
}

# This is probably where cmake is located under visual studio
# useful if cmake is not set on Path
$cmakePath = "${DevEnvDir}CommonExtensions\Microsoft\CMake\CMake\bin"
$env:Path += ";$cmakePath"

$assimpDir = "..\assimp"
$assimpBuildDir = "$assimpDir\build_${Platform}_${Config}"
$assimpIncludeDir = "$assimpDir\include\assimp"
$cmakeVars = "-DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF"
if ($Config -eq "Release") {
	$cmakeVars += " -DASSIMP_INSTALL_PDB=OFF"
}

cmake --log-level VERBOSE -S $assimpDir -B $assimpBuildDir -G "Visual Studio 16 2019" -A $Platform $cmakeVars
cmake --build $assimpBuildDir --config $Config.ToLower()

# copy .lib files
Copy-Item "$assimpBuildDir\lib\$Config\*" "$PSScriptRoot\..\Libs\$Platform\$Config\" -Recurse

# Copy .dll files
# They are created and neede even if I build static exe
Copy-Item "$assimpBuildDir\bin\$Config\assimp-vc142-mtd.*" "$PSScriptRoot\..\Libs\$Platform\$Config\"

#copy include files
$dstDir = "$PSScriptRoot\..\..\ThirdParty\include\assimp\"
mkdir -Force "$dstDir"
Copy-Item "$assimpIncludeDir\*" "$dstDir" -Recurse
Copy-Item "$assimpBuildDir\include\assimp\*" "$dstDir" -Recurse
